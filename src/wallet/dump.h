// Copyright (c) 2020-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MYTHERRA_WALLET_DUMP_H
#define MYTHERRA_WALLET_DUMP_H

#include <util/fs.h>

#include <string>
#include <vector>

struct bilingual_str;
class ArgsManager;

namespace wallet {
class CWallet;
bool DumpWallet(const ArgsManager& args, CWallet& wallet, bilingual_str& error);
bool CreateFromDump(const ArgsManager& args, const std::string& name, const fs::path& wallet_path, bilingual_str& error, std::vector<bilingual_str>& warnings);
} // namespace wallet

#endif // MYTHERRA_WALLET_DUMP_H
