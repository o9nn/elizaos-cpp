#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_SERVICES_MARKETDATASERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_SERVICES_MARKETDATASERVICE_H
#include "core.h"
#include "./database.h"
#include "@/config/chains.h"

class CoinGeckoPrice;
class BirdeyeResponse;
class MarketDataService;

class CoinGeckoPrice : public object, public std::enable_shared_from_this<CoinGeckoPrice> {
public:
    using std::enable_shared_from_this<CoinGeckoPrice>::shared_from_this;
};

class BirdeyeResponse : public object, public std::enable_shared_from_this<BirdeyeResponse> {
public:
    using std::enable_shared_from_this<BirdeyeResponse>::shared_from_this;
    object data;
};

class MarketDataService : public object, public std::enable_shared_from_this<MarketDataService> {
public:
    using std::enable_shared_from_this<MarketDataService>::shared_from_this;
    string coingeckoApiKey;

    string birdeyeApiKey;

    MarketDataService();
    virtual std::shared_ptr<Promise<double>> fetchTokenPrice(string tokenAddress, std::shared_ptr<Chain> chain);
    virtual std::shared_ptr<Promise<std::shared_ptr<TokenMarketData>>> fetchMarketData(string tokenAddress, std::shared_ptr<Chain> chain);
    virtual std::shared_ptr<Promise<std::shared_ptr<TokenMarketData>>> fetchEVMData(string tokenAddress, std::shared_ptr<Chain> chain);
    virtual std::shared_ptr<Promise<std::shared_ptr<TokenMarketData>>> fetchSolanaData(string tokenAddress);
    virtual std::shared_ptr<Promise<void>> refreshTokenData(string tokenId, string tokenAddress, std::shared_ptr<Chain> chain);
    virtual std::shared_ptr<Promise<void>> refreshAllTokenData(array<object> tokens);
};

#endif
