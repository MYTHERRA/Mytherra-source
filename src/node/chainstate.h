// Copyright (c) 2021-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MYTHERRA_NODE_CHAINSTATE_H
#define MYTHERRA_NODE_CHAINSTATE_H

#include <util/translation.h>
#include <validation.h>

#include <cstdint>
#include <functional>
#include <tuple>

class CTxMemPool;

namespace node {

struct CacheSizes;

struct ChainstateLoadOptions {
    CTxMemPool* mempool{nullptr};
    bool block_tree_db_in_memory{false};
    bool coins_db_in_memory{false};
    bool reindex{false};
    bool reindex_chainstate{false};
    bool prune{false};
    //! Setting require_full_verification to true will require all checks at
    //! check_level (below) to succeed for loading to succeed. Setting it to
    //! false will skip checks if cache is not big enough to run them, so may be
    //! helpful for running with a small cache.
    bool require_full_verification{true};
    int64_t check_blocks{DEFAULT_CHECKBLOCKS};
    int64_t check_level{DEFAULT_CHECKLEVEL};
    std::function<bool()> check_interrupt;
    std::function<void()> coins_error_cb;
};

//! Chainstate load status. Simple applications can just check for the success
//! case, and treat other cases as errors. More complex applications may want to
//! try reindexing in the generic failure case, and pass an interrupt callback
//! and exit cleanly in the interrupted case.
enum class ChainstateLoadStatus {
    SUCCESS,
    FAILURE,
    FAILURE_INCOMPATIBLE_DB,
    FAILURE_INSUFFICIENT_DBCACHE,
    INTERRUPTED,
};

//! Chainstate load status code and optional error string.
using ChainstateLoadResult = std::tuple<ChainstateLoadStatus, bilingual_str>;

/** This sequence can have 4 types of outcomes:
 *
 *  1. Success
 *  2. Shutdown requested
 *    - nothing failed but a shutdown was triggered in the middle of the
 *      sequence
 *  3. Soft failure
 *    - a failure that might be recovered from with a reindex
 *  4. Hard failure
 *    - a failure that definitively cannot be recovered from with a reindex
 *
 *  LoadChainstate returns a (status code, error string) tuple.
 */
ChainstateLoadResult LoadChainstate(ChainstateManager& chainman, const CacheSizes& cache_sizes,
                                    const ChainstateLoadOptions& options);
ChainstateLoadResult VerifyLoadedChainstate(ChainstateManager& chainman, const ChainstateLoadOptions& options);
} // namespace node

#endif // MYTHERRA_NODE_CHAINSTATE_H
