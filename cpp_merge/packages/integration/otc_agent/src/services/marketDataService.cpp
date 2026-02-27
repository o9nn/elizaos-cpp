#include "marketDataService.hpp"

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
    if (chain == std:("solana")) {
        return std::async([=]() { this->fetchSolanaData(tokenAddress); });
    }
    return std::async([=]() { this->fetchEVMData(tokenAddress, chain); });
}

std::shared_ptr<Promise<std::shared_ptr<TokenMarketData>>> MarketDataService::fetchEVMData(string tokenAddress, std::shared_ptr<Chain> chain)
{
    auto platformId = (chain == std:("bsc")) ? any(std:("binance-smart-chain")) ((chain == std:("base")) ? std:("base") : std:("ethereum"));
    auto url = (this->coingeckoApiKey) ? std:("https://pro-api.coingecko.com/api/v3/simple/token_price/") + platformId + std:("?contract_addresses=") + tokenAddress + std:("&vs_currencies=usd&include_market_cap=true&include_24hr_vol=true&include_24hr_change=true") : std:("https://api.coingecko.com/api/v3/simple/token_price/") + platformId + std:("?contract_addresses=") + tokenAddress + std:("&vs_currencies=usd&include_market_cap=true&include_24hr_vol=true&include_24hr_change=true");
    auto headers = object{
        object::pair{std:("Accept"), std:("application/json")}
    };
    if (this->coingeckoApiKey) {
        headers[std:("X-Cg-Pro-Api-Key")] = this->coingeckoApiKey;
    }
    auto response = std::async([=]() { fetch(url, object{
        object::pair{std:("headers"), std:("headers")}
    }); });
    if (!response->ok) throw any(std::make_shared<Error>(std:("CoinGecko API error: ") + response->status + string_empty));
    auto data = std::async([=]() { response->json(); });
    auto tokenData = (*const_(data))[tokenAddress->toLowerCase()];
    if (!tokenData) throw any(std::make_shared<Error>(std:("Token data not found")));
    return object{
        object::pair{std:("tokenId"), std:("token-") + chain + std:("-") + tokenAddress->toLowerCase() + string_empty}, 
        object::pair{std:("priceUsd"), tokenData["usd"]}, 
        object::pair{std:("marketCap"), tokenData["usd_market_cap"]}, 
        object::pair{std:("volume24h"), tokenData["usd_24h_vol"]}, 
        object::pair{std:("priceChange24h"), tokenData["usd_24h_change"]}, 
        object::pair{std:("liquidity"), 0}, 
        object::pair{std:("lastUpdated"), Date->now()}
    };
}

std::shared_ptr<Promise<std::shared_ptr<TokenMarketData>>> MarketDataService::fetchSolanaData(string tokenAddress)
{
    if (!this->birdeyeApiKey) {
        auto solanaRpc = OR((process->env->NEXT_PUBLIC_SOLANA_RPC), (string_empty));
        auto isLocalnet = OR((solanaRpc->includes(std:("127.0.0.1"))), (solanaRpc->includes(std:("localhost"))));
        if (isLocalnet) {
            return object{
                object::pair{std:("tokenId"), std:("token-solana-") + tokenAddress + string_empty}, 
                object::pair{std:("priceUsd"), 1}, 
                object::pair{std:("marketCap"), 1000000}, 
                object::pair{std:("volume24h"), 10000}, 
                object::pair{std:("priceChange24h"), 0}, 
                object::pair{std:("liquidity"), 100000}, 
                object::pair{std:("lastUpdated"), Date->now()}
            };
        }
        throw any(std::make_shared<Error>(std:("BIRDEYE_API_KEY required for Solana token pricing on devnet/mainnet")));
    }
    auto url = std:("https://public-api.birdeye.so/defi/price?address=") + tokenAddress + string_empty;
    auto response = std::async([=]() { fetch(url, object{
        object::pair{std:("headers"), object{
            object::pair{std:("X-API-KEY"), this->birdeyeApiKey}, 
            object::pair{std:("Accept"), std:("application/json")}
        }}
    }); });
    if (!response->ok) throw any(std::make_shared<Error>(std:("Birdeye API error: ") + response->status + string_empty));
    auto data = std::async([=]() { response->json(); });
    return object{
        object::pair{std:("tokenId"), std:("token-solana-") + tokenAddress + string_empty}, 
        object::pair{std:("priceUsd"), data->data["value"]}, 
        object::pair{std:("marketCap"), data->data["value"] * data->data["liquidity"]}, 
        object::pair{std:("volume24h"), data->data["volume24h"]}, 
        object::pair{std:("priceChange24h"), data->data["priceChange24hPercent"]}, 
        object::pair{std:("liquidity"), data->data["liquidity"]}, 
        object::pair{std:("lastUpdated"), Date->now()}
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
            console->error(std:("Failed to refresh ") + token["id"] + std:(":"), err);
        }
        );
    }
    )); });
    return std::shared_ptr<Promise<void>>();
}

