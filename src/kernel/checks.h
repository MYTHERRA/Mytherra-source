// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MYTHERRA_KERNEL_CHECKS_H
#define MYTHERRA_KERNEL_CHECKS_H

#include <optional>

struct bilingual_str;

namespace kernel {

struct Context;

/**
 *  Ensure a usable environment with all necessary library support.
 */
std::optional<bilingual_str> SanityChecks(const Context&);

}

#endif // MYTHERRA_KERNEL_CHECKS_H
