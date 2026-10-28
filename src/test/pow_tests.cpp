// Copyright (c) 2015-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chain.h>
#include <chainparams.h>
#include <pow.h>
#include <test/util/random.h>
#include <test/util/setup_common.h>

#include <boost/test/unit_test.hpp>

#include <limits>
#include <vector>

BOOST_FIXTURE_TEST_SUITE(pow_tests, BasicTestingSetup)

/* Test calculation of next difficulty target with no constraints applying */
BOOST_AUTO_TEST_CASE(get_next_work)
{
    const auto chainParams = CreateChainParams(*m_node.args, CBaseChainParams::MAIN);
    int64_t nLastRetargetTime = 1261130161; // Block #30240
    CBlockIndex pindexLast;
    pindexLast.nHeight = 32255;
    pindexLast.nTime = 1262152739;  // Block #32255
    pindexLast.nBits = 0x1d00ffff;

    // Here (and below): expected_nbits is calculated in
    // CalculateNextWorkRequired(); redoing the calculation here would be just
    // reimplementing the same code that is written in pow.cpp. Rather than
    // copy that code, we just hardcode the expected result.
    unsigned int expected_nbits = 0x1d00d86aU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, chainParams->GetConsensus()), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, expected_nbits));
}

/* Test the constraint on the upper bound for next work */
BOOST_AUTO_TEST_CASE(get_next_work_pow_limit)
{
    const auto chainParams = CreateChainParams(*m_node.args, CBaseChainParams::MAIN);
    int64_t nLastRetargetTime = 1231006505; // Block #0
    CBlockIndex pindexLast;
    pindexLast.nHeight = 2015;
    pindexLast.nTime = 1233061996;  // Block #2015
    pindexLast.nBits = 0x1d00ffff;
    unsigned int expected_nbits = 0x1d00ffffU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, chainParams->GetConsensus()), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, expected_nbits));
}

/* Test the constraint on the lower bound for actual time taken */
BOOST_AUTO_TEST_CASE(get_next_work_lower_limit_actual)
{
    const auto chainParams = CreateChainParams(*m_node.args, CBaseChainParams::MAIN);
    auto consensus = chainParams->GetConsensus();
    consensus.LWMAHeight = std::numeric_limits<int>::max();
    int64_t nLastRetargetTime = 1279008237; // Block #66528
    CBlockIndex pindexLast;
    pindexLast.nHeight = 68543;
    pindexLast.nTime = 1279297671;  // Block #68543
    pindexLast.nBits = 0x1c05a3f4;
    unsigned int expected_nbits = 0x1c0168fdU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, consensus), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(consensus, pindexLast.nHeight+1, pindexLast.nBits, expected_nbits));
    // Test that reducing nbits further would not be a PermittedDifficultyTransition.
    unsigned int invalid_nbits = expected_nbits-1;
    BOOST_CHECK(!PermittedDifficultyTransition(consensus, pindexLast.nHeight+1, pindexLast.nBits, invalid_nbits));
}

/* Test the constraint on the upper bound for actual time taken */
BOOST_AUTO_TEST_CASE(get_next_work_upper_limit_actual)
{
    const auto chainParams = CreateChainParams(*m_node.args, CBaseChainParams::MAIN);
    int64_t nLastRetargetTime = 1263163443; // NOTE: Not an actual block time
    CBlockIndex pindexLast;
    pindexLast.nHeight = 46367;
    pindexLast.nTime = 1269211443;  // Block #46367
    pindexLast.nBits = 0x1c387f6f;
    unsigned int expected_nbits = 0x1d00e1fdU;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequired(&pindexLast, nLastRetargetTime, chainParams->GetConsensus()), expected_nbits);
    BOOST_CHECK(PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, expected_nbits));
    // Test that increasing nbits further would not be a PermittedDifficultyTransition.
    unsigned int invalid_nbits = expected_nbits+1;
    BOOST_CHECK(!PermittedDifficultyTransition(chainParams->GetConsensus(), pindexLast.nHeight+1, pindexLast.nBits, invalid_nbits));
}

namespace {
constexpr uint32_t LWMA_TEST_BITS = 0x1d00ffff;

std::vector<CBlockIndex> BuildLWMAChain(int count, int64_t solvetime, uint32_t nbits = LWMA_TEST_BITS)
{
    std::vector<CBlockIndex> blocks(count);
    for (int i = 0; i < count; ++i) {
        blocks[i].nHeight = i;
        blocks[i].pprev = i > 0 ? &blocks[i - 1] : nullptr;
        blocks[i].nTime = i == 0 ? 1700000000 : blocks[i - 1].nTime + solvetime;
        blocks[i].nBits = nbits;
    }
    return blocks;
}
}

BOOST_AUTO_TEST_CASE(lwma_activation_boundary)
{
    auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    consensus.LWMAHeight = 62;
    auto blocks = BuildLWMAChain(62, 300);

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blocks[60], nullptr, consensus), LWMA_TEST_BITS);
    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blocks[61], nullptr, consensus), LWMA_TEST_BITS);
    BOOST_CHECK(!PermittedDifficultyTransition(consensus, 61, LWMA_TEST_BITS, LWMA_TEST_BITS + 1));
    BOOST_CHECK(PermittedDifficultyTransition(consensus, 62, LWMA_TEST_BITS, LWMA_TEST_BITS + 1));
}

BOOST_AUTO_TEST_CASE(lwma_stable_slow_and_fast_chains)
{
    auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    consensus.LWMAHeight = 0;
    auto stable = BuildLWMAChain(61, 300);
    auto slow = BuildLWMAChain(61, 600);
    auto fast = BuildLWMAChain(61, 150);

    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&stable.back(), consensus), 0x1d00ffffU);
    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&slow.back(), consensus), 0x1d01fffeU);
    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&fast.back(), consensus), 0x1c7fff80U);
}

BOOST_AUTO_TEST_CASE(lwma_weighted_window_vector)
{
    auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    auto blocks = BuildLWMAChain(61, 300);
    for (int i = 1; i < 61; ++i) {
        blocks[i].nTime = blocks[i - 1].nTime + ((i - 1) * 37) % 1801;
    }

    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&blocks.back(), consensus), 0x1d02ddc9U);
}

BOOST_AUTO_TEST_CASE(lwma_timestamp_clamps)
{
    auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    auto instant = BuildLWMAChain(61, 1);
    auto equal_times = BuildLWMAChain(61, 0);
    auto backwards = BuildLWMAChain(61, -60);
    auto long_gap = BuildLWMAChain(61, 1800);
    auto longer_gap = BuildLWMAChain(61, 7200);

    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&instant.back(), consensus), 0x1c00da73U);
    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&equal_times.back(), consensus), 0x1c00da73U);
    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&backwards.back(), consensus), 0x1c00da73U);
    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&long_gap.back(), consensus), CalculateNextWorkRequiredLWMA(&longer_gap.back(), consensus));
}

BOOST_AUTO_TEST_CASE(lwma_limits_and_insufficient_history)
{
    auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    auto short_chain = BuildLWMAChain(60, 600);
    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&short_chain.back(), consensus), LWMA_TEST_BITS);

    auto slow_chain = BuildLWMAChain(61, 600);
    consensus.fPowNoRetargeting = true;
    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&slow_chain.back(), consensus), LWMA_TEST_BITS);
    consensus.fPowNoRetargeting = false;

    consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    const uint32_t limit_bits = UintToArith256(consensus.powLimit).GetCompact();
    auto limit_chain = BuildLWMAChain(61, 1800, limit_bits);
    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&limit_chain.back(), consensus), limit_bits);

    consensus.powLimit = uint256S("1");
    const uint32_t minimum_bits = UintToArith256(consensus.powLimit).GetCompact();
    auto minimum_chain = BuildLWMAChain(61, 1, minimum_bits);
    BOOST_CHECK_EQUAL(CalculateNextWorkRequiredLWMA(&minimum_chain.back(), consensus), minimum_bits);
}

BOOST_AUTO_TEST_CASE(lwma_regtest_activation_option)
{
    ArgsManager args;
    args.ForceSetArg("-testactivationheight", "lwma@62");
    const auto chain_params = CreateChainParams(args, CBaseChainParams::REGTEST);

    BOOST_CHECK_EQUAL(chain_params->GetConsensus().LWMAHeight, 62);
    BOOST_CHECK(!chain_params->GetConsensus().fPowAllowMinDifficultyBlocks);
    BOOST_CHECK(!chain_params->GetConsensus().fPowNoRetargeting);
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_negative_target)
{
    const auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    nBits = UintToArith256(consensus.powLimit).GetCompact(true);
    hash.SetHex("0x1");
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_overflow_target)
{
    const auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits{~0x00800000U};
    hash.SetHex("0x1");
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_too_easy_target)
{
    const auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    arith_uint256 nBits_arith = UintToArith256(consensus.powLimit);
    nBits_arith *= 2;
    nBits = nBits_arith.GetCompact();
    hash.SetHex("0x1");
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_biger_hash_than_target)
{
    const auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    arith_uint256 hash_arith = UintToArith256(consensus.powLimit);
    nBits = hash_arith.GetCompact();
    hash_arith *= 2; // hash > nBits
    hash = ArithToUint256(hash_arith);
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(CheckProofOfWork_test_zero_target)
{
    const auto consensus = CreateChainParams(*m_node.args, CBaseChainParams::MAIN)->GetConsensus();
    uint256 hash;
    unsigned int nBits;
    arith_uint256 hash_arith{0};
    nBits = hash_arith.GetCompact();
    hash = ArithToUint256(hash_arith);
    BOOST_CHECK(!CheckProofOfWork(hash, nBits, consensus));
}

BOOST_AUTO_TEST_CASE(GetBlockProofEquivalentTime_test)
{
    const auto chainParams = CreateChainParams(*m_node.args, CBaseChainParams::MAIN);
    std::vector<CBlockIndex> blocks(10000);
    for (int i = 0; i < 10000; i++) {
        blocks[i].pprev = i ? &blocks[i - 1] : nullptr;
        blocks[i].nHeight = i;
        blocks[i].nTime = 1269211443 + i * chainParams->GetConsensus().nPowTargetSpacing;
        blocks[i].nBits = 0x207fffff; /* target 0x7fffff000... */
        blocks[i].nChainWork = i ? blocks[i - 1].nChainWork + GetBlockProof(blocks[i - 1]) : arith_uint256(0);
    }

    for (int j = 0; j < 1000; j++) {
        CBlockIndex *p1 = &blocks[InsecureRandRange(10000)];
        CBlockIndex *p2 = &blocks[InsecureRandRange(10000)];
        CBlockIndex *p3 = &blocks[InsecureRandRange(10000)];

        int64_t tdiff = GetBlockProofEquivalentTime(*p1, *p2, *p3, chainParams->GetConsensus());
        BOOST_CHECK_EQUAL(tdiff, p1->GetBlockTime() - p2->GetBlockTime());
    }
}

void sanity_check_chainparams(const ArgsManager& args, std::string chainName)
{
    const auto chainParams = CreateChainParams(args, chainName);
    const auto consensus = chainParams->GetConsensus();

    // hash genesis is correct
    BOOST_CHECK_EQUAL(consensus.hashGenesisBlock, chainParams->GenesisBlock().GetHash());

    // target timespan is an even multiple of spacing
    BOOST_CHECK_EQUAL(consensus.nPowTargetTimespan % consensus.nPowTargetSpacing, 0);

    // genesis nBits is positive, doesn't overflow and is lower than powLimit
    arith_uint256 pow_compact;
    bool neg, over;
    pow_compact.SetCompact(chainParams->GenesisBlock().nBits, &neg, &over);
    BOOST_CHECK(!neg && pow_compact != 0);
    BOOST_CHECK(!over);
    BOOST_CHECK(UintToArith256(consensus.powLimit) >= pow_compact);

    // check max target * 4*nPowTargetTimespan doesn't overflow -- see pow.cpp:CalculateNextWorkRequired()
    if (!consensus.fPowNoRetargeting) {
        arith_uint256 targ_max("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
        targ_max /= consensus.nPowTargetTimespan*4;
        BOOST_CHECK(UintToArith256(consensus.powLimit) < targ_max);
    }
}

BOOST_AUTO_TEST_CASE(ChainParams_MAIN_sanity)
{
    sanity_check_chainparams(*m_node.args, CBaseChainParams::MAIN);
}

BOOST_AUTO_TEST_CASE(ChainParams_REGTEST_sanity)
{
    sanity_check_chainparams(*m_node.args, CBaseChainParams::REGTEST);
}

BOOST_AUTO_TEST_CASE(ChainParams_TESTNET_sanity)
{
    sanity_check_chainparams(*m_node.args, CBaseChainParams::TESTNET);
}

BOOST_AUTO_TEST_CASE(ChainParams_SIGNET_sanity)
{
    sanity_check_chainparams(*m_node.args, CBaseChainParams::SIGNET);
}

BOOST_AUTO_TEST_SUITE_END()
