#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/services/marketDataService.h"

MarketDataService::MarketDataService() {
    this->coingeckoApiKey = process->env->COINGECKO_API_KEY;
    this->birdeyeApiKey = process->env->BIRDEYE_API_KEY;
}

std::shared_ptr<Promise<double>> MarketDataService::fetchTokenPrice(string tokenAddress, std::shared_ptr<Chain> chain)
{
    auto marketData = std::async([=]() { this->fetchMarketData(tokenAddress, chain); });
    return marketData->priceUsd;
}

std::shared_ptr<Promise<std::shared_ptr<TokenMarketData>>> MarketDataService::fetchMarketData(string tokenAddress, std::shared_ptr<Chain> chain)
{
    if (chain == std::string("solana")) {
        return std::async([=]() { this->fetchSolanaData(tokenAddress); });
    }
    return std::async([=]() { this->fetchEVMData(tokenAddress, chain); });
}

std::shared_ptr<Promise<std::shared_ptr<TokenMarketData>>> MarketDataService::fetchEVMData(string tokenAddress, std::shared_ptr<Chain> chain)
{
    auto platformId = (chain == std::string("bsc")) ? any(std::string("binance-smart-chain")) : any((chain == std::string("base")) ? std::string("base") : std::string("ethereum"));
    auto url = (this->coingeckoApiKey) ? std::string("https://pro-api.coingecko.com/api/v3/simple/token_price/") + platformId + std::string("?contract_addresses=") + tokenAddress + std::string("&vs_currencies=usd&include_market_cap=true&include_24hr_vol=true&include_24hr_change=true") : std::string("https://api.coingecko.com/api/v3/simple/token_price/") + platformId + std::string("?contract_addresses=") + tokenAddress + std::string("&vs_currencies=usd&include_market_cap=true&include_24hr_vol=true&include_24hr_change=true");
    auto headers = object{
        object::pair{std::string("Accept"), std::string("application/json")}
    };
    if (this->coingeckoApiKey) {
        headers[std::string("X-Cg-Pro-Api-Key")] = this->coingeckoApiKey;
    }
    auto response = std::async([=]() { fetch(url, object{
        object::pair{std::string("headers"), std::string("headers")}
    }); });
    if (!response->ok) throw any(std::make_shared<Error>(std::string("CoinGecko API error: ") + response->status + string_empty));
    auto data = std::async([=]() { response->json(); });
    auto tokenData = (*const_(data))[tokenAddress->toLowerCase()];
    if (!tokenData) throw any(std::make_shared<Error>(std::string("Token data not found")));
    return object{
        object::pair{std::string("tokenId"), std::string("token-") + chain + std::string("-") + tokenAddress->toLowerCase() + string_empty}, 
        object::pair{std::string("priceUsd"), tokenData["usd"]}, 
        object::pair{std::string("marketCap"), tokenData["usd_market_cap"]}, 
        object::pair{std::string("volume24h"), tokenData["usd_24h_vol"]}, 
        object::pair{std::string("priceChange24h"), tokenData["usd_24h_change"]}, 
        object::pair{std::string("liquidity"), 0}, 
        object::pair{std::string("lastUpdated"), Date->now()}
    };
}

std::shared_ptr<Promise<std::shared_ptr<TokenMarketData>>> MarketDataService::fetchSolanaData(string tokenAddress)
{
    if (!this->birdeyeApiKey) {
        auto solanaRpc = OR((process->env->NEXT_PUBLIC_SOLANA_RPC), (string_empty));
        auto isLocalnet = OR((solanaRpc->includes(std::string("127.0.0.1"))), (solanaRpc->includes(std::string("localhost"))));
        if (isLocalnet) {
            return object{
                object::pair{std::string("tokenId"), std::string("token-solana-") + tokenAddress + string_empty}, 
                object::pair{std::string("priceUsd"), 1}, 
                object::pair{std::string("marketCap"), 1000000}, 
                object::pair{std::string("volume24h"), 10000}, 
                object::pair{std::string("priceChange24h"), 0}, 
                object::pair{std::string("liquidity"), 100000}, 
                object::pair{std::string("lastUpdated"), Date->now()}
            };
        }
        throw any(std::make_shared<Error>(std::string("BIRDEYE_API_KEY required for Solana token pricing on devnet/mainnet")));
    }
    auto url = std::string("https://public-api.birdeye.so/defi/price?address=") + tokenAddress + string_empty;
    auto response = std::async([=]() { fetch(url, object{
        object::pair{std::string("headers"), object{
            object::pair{std::string("X-API-KEY"), this->birdeyeApiKey}, 
            object::pair{std::string("Accept"), std::string("application/json")}
        }}
    }); });
    if (!response->ok) throw any(std::make_shared<Error>(std::string("Birdeye API error: ") + response->status + string_empty));
    auto data = std::async([=]() { response->json(); });
    return object{
        object::pair{std::string("tokenId"), std::string("token-solana-") + tokenAddress + string_empty}, 
        object::pair{std::string("priceUsd"), data->data["value"]}, 
        object::pair{std::string("marketCap"), data->data["value"] * data->data["liquidity"]}, 
        object::pair{std::string("volume24h"), data->data["volume24h"]}, 
        object::pair{std::string("priceChange24h"), data->data["priceChange24hPercent"]}, 
        object::pair{std::string("liquidity"), data->data["liquidity"]}, 
        object::pair{std::string("lastUpdated"), Date->now()}
    };
}

std::shared_ptr<Promise<void>> MarketDataService::refreshTokenData(string tokenId, string tokenAddress, std::shared_ptr<Chain> chain)
{
    auto marketData = std::async([=]() { this->fetchMarketData(tokenAddress, chain); });
    std::async([=]() { MarketDataDB::setMarketData(marketData); });
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> MarketDataService::refreshAllTokenData(array<object> tokens)
{
    std::async([=]() { Promise->all(tokens->map([=](auto token) mutable
    {
        return this->refreshTokenData(token["id"], token["contractAddress"], token["chain"])->_catch([=](auto err) mutable
        {
            console->error(std::string("Failed to refresh ") + token["id"] + std::string(":"), err);
        }
        );
    }
    )); });
    return std::shared_ptr<Promise<void>>();
}

