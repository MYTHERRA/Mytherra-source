// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>

#include <algorithm>

namespace {
constexpr int64_t LWMA_AVERAGING_WINDOW = 60;
}

unsigned int CalculateNextWorkRequiredLWMA(const CBlockIndex* pindexLast, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    if (params.fPowNoRetargeting || pindexLast->nHeight < LWMA_AVERAGING_WINDOW) {
        return pindexLast->nBits;
    }

    const arith_uint256 pow_limit = UintToArith256(params.powLimit);
    const int64_t max_solvetime = 6 * params.nPowTargetSpacing;
    int64_t weighted_solvetime = 0;
    arith_uint256 average_target;
    uint32_t target_remainder = 0;
    const CBlockIndex* current = pindexLast;

    for (int64_t weight = LWMA_AVERAGING_WINDOW; weight >= 1; --weight) {
        assert(current != nullptr && current->pprev != nullptr);
        const int64_t solvetime = std::clamp(current->GetBlockTime() - current->pprev->GetBlockTime(), int64_t{1}, max_solvetime);
        weighted_solvetime += weight * solvetime;

        arith_uint256 target;
        target.SetCompact(current->nBits);
        if (target > pow_limit) target = pow_limit;
        const arith_uint256 target_quotient = target / arith_uint256{static_cast<uint64_t>(LWMA_AVERAGING_WINDOW)};
        average_target += target_quotient;
        target_remainder += (target - target_quotient * static_cast<uint32_t>(LWMA_AVERAGING_WINDOW)).GetLow64();
        current = current->pprev;
    }
    average_target += target_remainder / LWMA_AVERAGING_WINDOW;

    const uint32_t denominator = static_cast<uint32_t>(LWMA_AVERAGING_WINDOW * (LWMA_AVERAGING_WINDOW + 1) * params.nPowTargetSpacing / 2);
    const arith_uint256 divisor{denominator};
    const arith_uint256 weighted{static_cast<uint64_t>(weighted_solvetime)};
    const arith_uint256 average_quotient = average_target / divisor;
    const arith_uint256 average_remainder = average_target - average_quotient * denominator;

    if (average_quotient != 0 && average_quotient > pow_limit / weighted) {
        return pow_limit.GetCompact();
    }

    const arith_uint256 base = average_quotient * static_cast<uint32_t>(weighted_solvetime);
    const arith_uint256 correction = (average_remainder * static_cast<uint32_t>(weighted_solvetime)) / divisor;
    if (correction > pow_limit - base) {
        return pow_limit.GetCompact();
    }
    const arith_uint256 next_target = base + correction;
    if (next_target == 0) return arith_uint256{1}.GetCompact();
    return next_target.GetCompact();
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    const int64_t next_height = static_cast<int64_t>(pindexLast->nHeight) + 1;
    if (next_height >= params.LWMAHeight) {
        return CalculateNextWorkRequiredLWMA(pindexLast, params);
    }

    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Only change once per difficulty adjustment interval
    if (next_height % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than twice the target spacing
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by one difficulty adjustment interval
    int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

// Before LWMA activation, check that difficulty adjustments stay within the
// permitted limits. LWMA transitions are verified against full block history
// during block validation instead.
bool PermittedDifficultyTransition(const Consensus::Params& params, int64_t height, uint32_t old_nbits, uint32_t new_nbits)
{
    if (height >= params.LWMAHeight || params.fPowAllowMinDifficultyBlocks) return true;

    if (height % params.DifficultyAdjustmentInterval() == 0) {
        int64_t smallest_timespan = params.nPowTargetTimespan/4;
        int64_t largest_timespan = params.nPowTargetTimespan*4;

        const arith_uint256 pow_limit = UintToArith256(params.powLimit);
        arith_uint256 observed_new_target;
        observed_new_target.SetCompact(new_nbits);

        // Calculate the largest difficulty value possible:
        arith_uint256 largest_difficulty_target;
        largest_difficulty_target.SetCompact(old_nbits);
        largest_difficulty_target *= largest_timespan;
        largest_difficulty_target /= params.nPowTargetTimespan;

        if (largest_difficulty_target > pow_limit) {
            largest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 maximum_new_target;
        maximum_new_target.SetCompact(largest_difficulty_target.GetCompact());
        if (maximum_new_target < observed_new_target) return false;

        // Calculate the smallest difficulty value possible:
        arith_uint256 smallest_difficulty_target;
        smallest_difficulty_target.SetCompact(old_nbits);
        smallest_difficulty_target *= smallest_timespan;
        smallest_difficulty_target /= params.nPowTargetTimespan;

        if (smallest_difficulty_target > pow_limit) {
            smallest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 minimum_new_target;
        minimum_new_target.SetCompact(smallest_difficulty_target.GetCompact());
        if (minimum_new_target > observed_new_target) return false;
    } else if (old_nbits != new_nbits) {
        return false;
    }
    return true;
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
