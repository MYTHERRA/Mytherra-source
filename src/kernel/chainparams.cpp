// Copyright (c) 2012 Satoshi Nakamoto
// Copyright (c) 2009-2020 The Bitcoin Core developers
// Copyright (c) 2025 The Mytherra Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <arith_uint256.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <chainparamsbase.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000004d6394d96ceba22ed8f91440aea49c35a5d0054bee7a111d6b4714fc3fe, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1752832800, nBits=1e0ffff0, nNonce=1011800, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Panto Created Genesis Block 18 July 2025";
    const CScript genesisOutputScript = CScript() << ParseHex("0479be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = CBaseChainParams::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0; 
        consensus.BIP66Height = 0; 
        consensus.CSVHeight = 0; 
        consensus.SegwitHeight = 0; 
        consensus.MinBIP9WarningHeight = 0; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan =  60 * 60;
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 11; // 90% of 12
        consensus.nMinerConfirmationWindow = 12; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; 

        // Placeholder for versionbit 14
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN14].bit = 14;
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN14].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN14].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN14].min_activation_height = 0;

        // Placeholder for versionbit 23
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN23].bit = 23;
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN23].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN23].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN23].min_activation_height = 0;

        // Placeholder for versionbit 26
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN26].bit = 26;
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN26].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN26].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_UNKNOWN26].min_activation_height = 0;

        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000020ca58a376edd8d9218");
        consensus.defaultAssumeValid = uint256S("0x00000fb7201bbe5db3dbabe46a1028836f8143c8fcea62c59dc5379e2f5b8309"); 

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x26;
        pchMessageStart[1] = 0x12;
        pchMessageStart[2] = 0x70;
        pchMessageStart[3] = 0x64;
        nDefaultPort = 24156;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 5;
        m_assumed_chain_state_size = 5;

        genesis = CreateGenesisBlock(1752832800, 1002405, 0x1e0ffff0, 1, 60 * COIN);
        consensus.hashGenesisBlock = uint256S("0x00000fb7201bbe5db3dbabe46a1028836f8143c8fcea62c59dc5379e2f5b8309");
        // calculate main genesis block 
        if (false && (genesis.GetHash() != consensus.hashGenesisBlock)) {
            std::cout << std::string("Calculating main genesis block...\n");
            arith_uint256 hashTarget = arith_uint256().SetCompact(genesis.nBits);
            uint256 hash;
            genesis.nNonce = 0;
            while (UintToArith256(genesis.GetHash()) > hashTarget)
            {
                ++genesis.nNonce;
                if (genesis.nNonce == 0)
                {
                    ++genesis.nTime;
                }
                if (genesis.nNonce % 50000 == 0) {
                    std::cout << "Testing nonce: " << genesis.nNonce << "\n";
                }
            }
            std::cout << "Genesis block found!\n";
            std::cout << "nonce: " << genesis.nNonce << "\n";
            std::cout << "time: " << genesis.nTime << "\n";
            std::cout << "blockhash: " << genesis.GetHash().ToString().c_str() << "\n";
            std::cout << "merklehash: " << genesis.hashMerkleRoot.ToString().c_str() << "\n";
            consensus.hashGenesisBlock = genesis.GetHash();
        }
        assert(consensus.hashGenesisBlock == uint256S("0x00000fb7201bbe5db3dbabe46a1028836f8143c8fcea62c59dc5379e2f5b8309"));
        assert(genesis.hashMerkleRoot == uint256S("0x130e93034ccb00ed91b2afd158dbede725dbc91e534f4382baacb69bcff5f70a"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("217.154.114.75");
        vSeeds.emplace_back("192.168.188.67");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,1);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,0);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "myt";


        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {     0, uint256S("0x00000fb7201bbe5db3dbabe46a1028836f8143c8fcea62c59dc5379e2f5b8309")},
                {  2000, uint256S("0x00000000000000d5ee802ac451cf0c86f5ff2afefc7902fea2c2fe5d264a3488")},
                {  4000, uint256S("0x0000000000000009c4fc90ef0ed2aa3a866669b3c7f2ad3faa92de916b978eb7")},
                {  6000, uint256S("0x000000000000000b7cd6ac819bbd9ea528f98b651f4c49ab7b413612c82c49af")},
                {  8000, uint256S("0x0000000000000027909740e67f4acb5aaa8198db36855c88f486b50590b4b51e")},
                { 10000, uint256S("0x00000000000000269f0ce7776d82b07a2e91dfd963614bdbfae26f4a0b6b6d0d")},
                { 12000, uint256S("0x0000000000000000bafae1bfd0963fe156fabef5335396c501a45d43c7f30cb0")},
                { 14000, uint256S("0x0000000000000015906d7c3cad96a59e24dffc2c3f34b28099c0d5a5fe580f07")},
                { 16000, uint256S("0x000000000000000e5f9fe7ec07d0d48c2f9c4383d819b831e21f596b8fc16182")},
                { 18000, uint256S("0x000000000000005d2d5f13b907f6ca9aa5184711099ad1a1a7e15b91232abfe6")},
                { 20000, uint256S("0x000000000000000cf973d227186992b0225aaa5f2d4b1e9557737bd3ea8686e9")},
                { 22000, uint256S("0x000000000000000587f3d5a674c70eb4fc8d5888eea2305ea65a238b65ba7e16")},
            }
        };

        m_assumeutxo_data = MapAssumeutxo{
         // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 0000003e57cc2746a59b957a0c23005f20da747c1da747137f7627caf0a5a154
            .nTime    = 1752832800,
            .nTxCount = 0,
            .dTxRate  = 0,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = CBaseChainParams::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.BIP34Height = 21111;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 581885; 
        consensus.BIP66Height = 330776; 
        consensus.CSVHeight = 770112; 
        consensus.SegwitHeight = 834624; 
        consensus.MinBIP9WarningHeight = 836640; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan =  60 * 60;
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 12; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000100010");
        consensus.defaultAssumeValid = uint256S("0x00000b063edf8b5e994c87fd37d955535afdc307a1a8666e11982cfcc6bcceae"); 

        pchMessageStart[0] = 0x30;
        pchMessageStart[1] = 0xa4;
        pchMessageStart[2] = 0xc4;
        pchMessageStart[3] = 0x7e;
        nDefaultPort = 34156;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 42;
        m_assumed_chain_state_size = 3;

        genesis = CreateGenesisBlock(1752832800, 5176, 0x1e0ffff0, 1, 60 * COIN);
        consensus.hashGenesisBlock = uint256S("0x00006ee3cc00feeaf54b97f7c421c1159e8ad0d7d5ff8b39b1dd0e4cd1ceb21a");
        //assert(consensus.hashGenesisBlock == uint256S("0x00006ee3cc00feeaf54b97f7c421c1159e8ad0d7d5ff8b39b1dd0e4cd1ceb21a"));
        //assert(genesis.hashMerkleRoot == uint256S("0x66abee8edd7338df85c93d13a577b9acc0f3a90665db232862f1bf4eba7f5ba0"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,51);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,50);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,178);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tmyt";


        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        m_is_test_chain = true;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {0, uint256S("00006ee3cc00feeaf54b97f7c421c1159e8ad0d7d5ff8b39b1dd0e4cd1ceb21a")},
            }
        };

        m_assumeutxo_data = MapAssumeutxo{
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 00000b063edf8b5e994c87fd37d955535afdc307a1a8666e11982cfcc6bcceae
            .nTime    = 1751277010,
            .nTxCount = 0,
            .dTxRate  = 0,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vSeeds.clear();

        if (!options.challenge) {
            bin = ParseHex("512103ad5e0edad18cb1f0fc0d28a3d4f1f3e445640337489abb10404f2d1e086be430210359ef5021964fe22d6f8e05b2463c9540ce96883fe3b278760f048f5189f2e6c452ae");

            // Hardcoded nodes can be removed once there are more DNS seeds

            consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000100010");
            consensus.defaultAssumeValid = uint256S("0x000003482612625f5c9b53d736fb474f390ab3d3f631e25342d38dd2e3c4cb10"); 
            m_assumed_blockchain_size = 1;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                // Data from RPC: getchaintxstats 4096 000003482612625f5c9b53d736fb474f390ab3d3f631e25342d38dd2e3c4cb10
                .nTime    = 1751277015,
                .nTxCount = 0,
                .dTxRate  = 0,
            };
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogPrintf("Signet with challenge %s\n", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        strNetworkID = CBaseChainParams::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.nPowTargetTimespan =  60 * 60;
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 11; // 90% of 12
        consensus.nMinerConfirmationWindow = 12; // nPowTargetTimespan / nPowTargetSpacing
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00000377ae000000000000000000000000000000000000000000000000000000");
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Activation of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        memcpy(pchMessageStart, hash.begin(), 4);

        nDefaultPort = 324156;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1752832800, 8122285, 0x1e0377ae, 1, 60 * COIN);
        consensus.hashGenesisBlock = uint256S("0x00000f3f2d1d9b99f15845b26e4e78cd88d9b6ba8b6a0a5b5a2e1e1f0e0e1e0e");
        //assert(consensus.hashGenesisBlock == uint256S("0x00000f3f2d1d9b99f15845b26e4e78cd88d9b6ba8b6a0a5b5a2e1e1f0e0e1e0e"));
        //assert(genesis.hashMerkleRoot == uint256S("0x66abee8edd7338df85c93d13a577b9acc0f3a90665db232862f1bf4eba7f5ba0"));

        vFixedSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,51);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,50);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,178);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tmyt";

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = true;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        strNetworkID =  CBaseChainParams::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP34Height = 1; // Always active unless overridden
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1;  // Always active unless overridden
        consensus.BIP66Height = 1;  // Always active unless overridden
        consensus.CSVHeight = 1;    // Always active unless overridden
        consensus.SegwitHeight = 0; // Always active unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan =  60 * 60;
        consensus.nPowTargetSpacing = 5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0x95;
        pchMessageStart[1] = 0x1d;
        pchMessageStart[2] = 0x48;
        pchMessageStart[3] = 0xc4;
        nDefaultPort = 18444;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock(1752832800, 1, 0x207fffff, 1, 60 * COIN);
        consensus.hashGenesisBlock = uint256S("0x20b18b28095c89aa15915ff48a8bcc5f0e5e5c8ad8a10b03e2b4f5fe6f12ee2c");
        //assert(consensus.hashGenesisBlock == uint256S("0x20b18b28095c89aa15915ff48a8bcc5f0e5e5c8ad8a10b03e2b4f5fe6f12ee2c"));
        //assert(genesis.hashMerkleRoot == uint256S("0x66abee8edd7338df85c93d13a577b9acc0f3a90665db232862f1bf4eba7f5ba0"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");

        fDefaultConsistencyChecks = true;
        fRequireStandard = true;
        m_is_test_chain = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256S("20b18b28095c89aa15915ff48a8bcc5f0e5e5c8ad8a10b03e2b4f5fe6f12ee2c")},
            }
        };

        m_assumeutxo_data = MapAssumeutxo{};

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,51);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,50);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,178);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "rmyt";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}
