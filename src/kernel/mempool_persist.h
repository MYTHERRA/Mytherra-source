// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MYTHERRA_KERNEL_MEMPOOL_PERSIST_H
#define MYTHERRA_KERNEL_MEMPOOL_PERSIST_H

#include <util/fs.h>

class Chainstate;
class CTxMemPool;

namespace kernel {

/** Dump the mempool to disk. */
bool DumpMempool(const CTxMemPool& pool, const fs::path& dump_path,
                 fsbridge::FopenFn mockable_fopen_function = fsbridge::fopen,
                 bool skip_file_commit = false);

/** Load the mempool from disk. */
bool LoadMempool(CTxMemPool& pool, const fs::path& load_path,
                 Chainstate& active_chainstate,
                 fsbridge::FopenFn mockable_fopen_function = fsbridge::fopen);

} // namespace kernel


#endif // MYTHERRA_KERNEL_MEMPOOL_PERSIST_H
