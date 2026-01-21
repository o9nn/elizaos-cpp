#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-WORLD_SRC_LIB_TOKEN-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-WORLD_SRC_LIB_TOKEN-UTILS_H
#include "core.h"
#include "@solana/web3.js.h"
#include "@solana/spl-token.h"
#include "axios.h"
using axios = _default;
#include "@/types.h"
#include "alchemy-sdk.h"
#include "fs.h"
using fs = _default;
#include "path.h"
using path = _default;

class SolSnifferResponse;

extern object config;
extern std::any alchemy;
extern std::any SCORES_CACHE_PATH;
Record<std::string, double> loadCachedScores();

void saveScoresToCache(Record<std::string, double> scores);

std::shared_ptr<Promise<array<std::shared_ptr<TokenBalance>>>> getTokenBalances(std::shared_ptr<Connection> connection, std::shared_ptr<PublicKey> walletPubkey);

std::shared_ptr<Promise<std::shared_ptr<DexScreenerResponse>>> fetchDexScreenerData(array<string> tokenAddresses);

std::any fetchTokenAnalysis(std::string address);

std::shared_ptr<Promise<array<std::shared_ptr<TokenHolding>>>> calculateHoldings(std::shared_ptr<Connection> connection, array<std::shared_ptr<TokenBalance>> balances, std::shared_ptr<DexScreenerResponse> marketData, std::string walletAddress);

std::any getTokenData(std::shared_ptr<Connection> connection);

class SolSnifferResponse : public object, public std::enable_shared_from_this<SolSnifferResponse> {
public:
    using std::enable_shared_from_this<SolSnifferResponse>::shared_from_this;
    array<object> data;
};

std::shared_ptr<Promise<Record<std::string, double>>> fetchTokenScores(array<string> tokenAddresses);

#endif
