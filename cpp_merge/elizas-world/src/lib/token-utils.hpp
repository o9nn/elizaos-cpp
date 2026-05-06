#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_WORLD_SRC_LIB_TOKEN_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_WORLD_SRC_LIB_TOKEN_UTILS_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "axios.hpp"
// Using alias removed (invalid transpilation)
// External dependency removed
#include "alchemy-sdk.h"
#include "fs.hpp"
// Using alias removed (invalid transpilation)
#include "path.hpp"
// Using alias removed (invalid transpilation)

class SolSnifferResponse;

extern object config;
extern any alchemy;
extern any SCORES_CACHE_PATH;
Record<string, double> loadCachedScores();

void saveScoresToCache(Record<string, double> scores);

std::shared_ptr<Promise<array<std::shared_ptr<TokenBalance>>>> getTokenBalances(std::shared_ptr<Connection> connection, std::shared_ptr<PublicKey> walletPubkey);

std::shared_ptr<Promise<std::shared_ptr<DexScreenerResponse>>> fetchDexScreenerData(array<string> tokenAddresses);

any fetchTokenAnalysis(string address);

std::shared_ptr<Promise<array<std::shared_ptr<TokenHolding>>>> calculateHoldings(std::shared_ptr<Connection> connection, array<std::shared_ptr<TokenBalance>> balances, std::shared_ptr<DexScreenerResponse> marketData, string walletAddress);

any getTokenData(std::shared_ptr<Connection> connection);

class SolSnifferResponse : public object, public std::enable_shared_from_this<SolSnifferResponse> {
public:
    using std::enable_shared_from_this<SolSnifferResponse>::shared_from_this;
    array<object> data;
};

std::shared_ptr<Promise<Record<string, double>>> fetchTokenScores(array<string> tokenAddresses);

#endif
