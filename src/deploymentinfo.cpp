// Copyright (c) 2016-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <deploymentinfo.h>

#include <consensus/params.h>

#include <string_view>

const struct VBDeploymentInfo VersionBitsDeploymentInfo[Consensus::MAX_VERSION_BITS_DEPLOYMENTS] = {
    {
        /*.name =*/ "testdummy",
        /*.gbt_force =*/ true,
    },
    {
        /*.name =*/ "taproot",
        /*.gbt_force =*/ true,
    },
    {
        /*.name =*/ "reserved_bit13",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit14",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit15",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit16",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit17",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit18",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit19",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit20",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit21",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit22",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit23",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit24",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit25",
        /*.gbt_force =*/ false,
    },
    {
        /*.name =*/ "reserved_bit26",
        /*.gbt_force =*/ false,
    },
};

std::string DeploymentName(Consensus::BuriedDeployment dep)
{
    assert(ValidDeployment(dep));
    switch (dep) {
    case Consensus::DEPLOYMENT_HEIGHTINCB:
        return "bip34";
    case Consensus::DEPLOYMENT_CLTV:
        return "bip65";
    case Consensus::DEPLOYMENT_DERSIG:
        return "bip66";
    case Consensus::DEPLOYMENT_CSV:
        return "csv";
    case Consensus::DEPLOYMENT_SEGWIT:
        return "segwit";
    } // no default case, so the compiler can warn about missing cases
    return "";
}

std::optional<Consensus::BuriedDeployment> GetBuriedDeployment(const std::string_view name)
{
    if (name == "segwit") {
        return Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT;
    } else if (name == "bip34") {
        return Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB;
    } else if (name == "dersig") {
        return Consensus::BuriedDeployment::DEPLOYMENT_DERSIG;
    } else if (name == "cltv") {
        return Consensus::BuriedDeployment::DEPLOYMENT_CLTV;
    } else if (name == "csv") {
        return Consensus::BuriedDeployment::DEPLOYMENT_CSV;
    }
    return std::nullopt;
}
