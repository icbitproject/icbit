// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"
#include "arith_uint256.h"

#include <assert.h>
#include <stdio.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

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
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Shanghai stock index closed at 3180.16, Apr. 12th, 2018.";
    const CScript genesisOutputScript = CScript() << ParseHex("0492a4dd7a91613c59203721923c94154ac91949264cd8cbb898a9076b90e778ab1621d6ee8e10544b0a0a44747f4522abc2f232ebd12c113ebd57a60d5aecfffb") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 5256000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("0x00cdfc211f52d2d3c4615cbd0979d43f0d35144a0c1db9a4a86b1caf6e6a06a3");
        consensus.powLimit = uint256S("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 30; // 30 seconds
        consensus.nPowTargetSpacing = 30; // 30 seconds
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1523529400;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1523529499;

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xf9;
        pchMessageStart[1] = 0xbe;
        pchMessageStart[2] = 0xbb;
        pchMessageStart[3] = 0xdd;
        vAlertPubKey = ParseHex("0408e6f641e4cf520244966a61f30dc849312adc85af5225f9943b378069f251ca1573f69f7aa5d018fc3830bb6392cfe1fd85c4979bbeadc089fb595dd9d2ad3e");
        nDefaultPort = 9013;
        nMaxTipAge = 100 * 24 * 60 * 60;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1526990901, 115, 0x2000ffff, 1, 77 * COIN);
        while(!true){// search genesis
            static FILE * genesis_file = NULL; if (genesis_file == NULL) {genesis_file = fopen("genesis.info", "w");}
            arith_uint256 hash = UintToArith256(genesis.GetHash());
            arith_uint256 target;
            target.SetCompact(genesis.nBits);
            printf("nonce: %d\npow:%s\nmerkle:%s\n\n"
                                    , genesis.nNonce
                                    , hash.ToString().c_str()
                                    , genesis.hashMerkleRoot.ToString().c_str());
            if (hash < target){
                if(genesis_file != NULL){
                    fprintf(genesis_file, "nonce: %d\npow:%s\nmerkle:%s\n\n"
                        , genesis.nNonce
                        , hash.ToString().c_str()
                        , genesis.hashMerkleRoot.ToString().c_str());
                    fclose(genesis_file); genesis_file = NULL;
                    exit(0);
                }
            }
            genesis.nNonce++;
        }
        assert(genesis.GetHash() == uint256S("0x00cdfc211f52d2d3c4615cbd0979d43f0d35144a0c1db9a4a86b1caf6e6a06a3"));
        assert(genesis.hashMerkleRoot == uint256S("0xec59f3525515f6f1bac6b61157f0032e78d85a5c26e4e60a3025b6361f2740be"));
        consensus.hashGenesisBlock = genesis.GetHash();

        vSeeds.push_back(CDNSSeedData("n1.icbstats.info", "n1.icbstats.info"));
        vSeeds.push_back(CDNSSeedData("n2.icbstats.info", "n2.icbstats.info"));
        vSeeds.push_back(CDNSSeedData("n3.icbstats.info", "n3.icbstats.info"));
        vSeeds.push_back(CDNSSeedData("n4.icbstats.info", "n4.icbstats.info"));
        vSeeds.push_back(CDNSSeedData("n5.icbstats.info", "n5.icbstats.info"));
        vSeeds.push_back(CDNSSeedData("n6.icbstats.info", "n6.icbstats.info"));

        vSeeds.push_back(CDNSSeedData("n1.icbit.io", "n1.icbit.io"));
        vSeeds.push_back(CDNSSeedData("n2.icbit.io", "n2.icbit.io"));
        vSeeds.push_back(CDNSSeedData("n3.icbit.io", "n3.icbit.io"));
        vSeeds.push_back(CDNSSeedData("n4.icbit.io", "n4.icbit.io"));
        vSeeds.push_back(CDNSSeedData("n5.icbit.io", "n5.icbit.io"));
        vSeeds.push_back(CDNSSeedData("n6.icbit.io", "n6.icbit.io"));

        // Ref: https://en.bitcoin.it/wiki/List_of_address_prefixes
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0); //'1'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,85); //'b'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (        0, uint256S("0x00cdfc211f52d2d3c4615cbd0979d43f0d35144a0c1db9a4a86b1caf6e6a06a3"))
            (        1, uint256S("0x00a64a5510b104fd4c2a8da7dc520267a944e0b698844c05a1bc31f5458223e3"))
            (        2, uint256S("0x00a3578faaeebee5d58845d39c116c7a4e756d66bb049a97bc68d4b0130717b8"))
            (        3, uint256S("0x0010b879cf9a4687ade763b16956dcadab28138beedcfabadc47ea3dd84e0745"))
            (        4, uint256S("0x00333a07d2354c44a93ac1be0eb8ab565c77a76e8a7dbc065afe8275d4ac8743"))
            (        5, uint256S("0x00180450a29c81d07a602b7be90525f32020794ed622e6ed983d48c9e964f0ed"))
            ,
            1523529400, // * UNIX timestamp of last checkpoint block
            1,          // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            60000.0     // * estimated number of transactions per day after checkpoint
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("0x00cdfc211f52d2d3c4615cbd0979d43f0d35144a0c1db9a4a86b1caf6e6a06a3");
        consensus.powLimit = uint256S("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1456790400; // March 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1493596800; // May 1st, 2017

        pchMessageStart[0] = 0x0b;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x09;
        pchMessageStart[3] = 0x07;
        vAlertPubKey = ParseHex("0408e6f641e4cf520244966a61f30dc849312adc85af5225f9943b378069f251ca1573f69f7aa5d018fc3830bb6392cfe1fd85c4979bbeadc089fb595dd9d2ad3e");
        nDefaultPort = 18333;
        nMaxTipAge = 0x7fffffff;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1526990901, 115, 0x2000ffff, 1, 77 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00cdfc211f52d2d3c4615cbd0979d43f0d35144a0c1db9a4a86b1caf6e6a06a3"));
        assert(genesis.hashMerkleRoot == uint256S("0xec59f3525515f6f1bac6b61157f0032e78d85a5c26e4e60a3025b6361f2740be"));

        vFixedSeeds.clear();
        vSeeds.clear();
        //vSeeds.push_back(CDNSSeedData("n1.ICBitstatus.info", "n1.ICBitstatus.info"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x00cdfc211f52d2d3c4615cbd0979d43f0d35144a0c1db9a4a86b1caf6e6a06a3")),
            1337966069,
            1488,
            300
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 0; // BIP34 has not necessarily activated on regtest
        consensus.BIP34Hash = uint256S("0x00cdfc211f52d2d3c4615cbd0979d43f0d35144a0c1db9a4a86b1caf6e6a06a3");
        consensus.powLimit = uint256S("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nMaxTipAge = 24 * 60 * 60;
        nDefaultPort = 18444;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1526990901, 115, 0x2000ffff, 1, 77 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00cdfc211f52d2d3c4615cbd0979d43f0d35144a0c1db9a4a86b1caf6e6a06a3"));
        assert(genesis.hashMerkleRoot == uint256S("0xec59f3525515f6f1bac6b61157f0032e78d85a5c26e4e60a3025b6361f2740be"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x00cdfc211f52d2d3c4615cbd0979d43f0d35144a0c1db9a4a86b1caf6e6a06a3")),
            0,
            0,
            0
        };
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}
