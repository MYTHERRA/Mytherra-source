// Copyright (c) 2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MYTHERRA_IPC_PROCESS_H
#define MYTHERRA_IPC_PROCESS_H

#include <util/fs.h>

#include <memory>
#include <string>

namespace ipc {
class Protocol;

//! IPC process interface for spawning mytherra processes and serving requests
//! in processes that have been spawned.
//!
//! There will be different implementations of this interface depending on the
//! platform (e.g. unix, windows).
class Process
{
public:
    virtual ~Process() = default;

    //! Spawn process and return socket file descriptor for communicating with
    //! it.
    virtual int spawn(const std::string& new_exe_name, const fs::path& argv0_path, int& pid) = 0;

    //! Wait for spawned process to exit and return its exit code.
    virtual int waitSpawned(int pid) = 0;

    //! Parse command line and determine if current process is a spawned child
    //! process. If so, return true and a file descriptor for communicating
    //! with the parent process.
    virtual bool checkSpawned(int argc, char* argv[], int& fd) = 0;
};

//! Constructor for Process interface. Implementation will vary depending on
//! the platform (unix or windows).
std::unique_ptr<Process> MakeProcess();
} // namespace ipc

#endif // MYTHERRA_IPC_PROCESS_H
