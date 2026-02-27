#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/config/providers.h"

object PROVIDER_CONFIG = object{
    object::pair{std::string("BIRDEYE_API"), std::string("https://public-api.birdeye.so")}, 
    object::pair{std::string("TOKEN_SECURITY_ENDPOINT"), std::string("/defi/token_security?address=")}, 
    object::pair{std::string("TOKEN_METADATA_ENDPOINT"), std::string("/defi/v3/token/meta-data/single?address=")}, 
    object::pair{std::string("MARKET_SEARCH_ENDPOINT"), std::string("/defi/v3/token/trade-data/single?address=")}, 
    object::pair{std::string("TOKEN_PRICE_CHANGE_ENDPOINT"), std::string("/defi/v3/search?chain=solana&target=token&sort_by=price_change_24h_percent&sort_type=desc&verify_token=true&markets=Raydium&limit=20")}, 
    object::pair{std::string("TOKEN_VOLUME_24_CHANGE_ENDPOINT"), std::string("/defi/v3/search?chain=solana&target=token&sort_by=volume_24h_change_percent&sort_type=desc&verify_token=true&markets=Raydium&limit=20")}, 
    object::pair{std::string("TOKEN_BUY_24_CHANGE_ENDPOINT"), std::string("/defi/v3/search?chain=solana&target=token&sort_by=buy_24h_change_percent&sort_type=desc&verify_token=true&markets=Raydium&offset=0&limit=20")}, 
    object::pair{std::string("TOKEN_SECURITY_ENDPOINT_BASE"), std::string("/defi/token_security?address=")}, 
    object::pair{std::string("TOKEN_METADATA_ENDPOINT_BASE"), std::string("/defi/v3/token/meta-data/single?address=")}, 
    object::pair{std::string("MARKET_SEARCH_ENDPOINT_BASE"), std::string("/defi/v3/token/trade-data/single?address=")}, 
    object::pair{std::string("TOKEN_PRICE_CHANGE_ENDPOINT_BASE"), std::string("/defi/v3/search?chain=base&target=token&sort_by=price_change_24h_percent&sort_type=desc&offset=0&limit=20")}, 
    object::pair{std::string("TOKEN_VOLUME_24_ENDPOINT_BASE"), std::string("/defi/v3/search?chain=base&target=token&sort_by=volume_24h_usd&sort_type=desc&offset=2&limit=20")}, 
    object::pair{std::string("TOKEN_BUY_24_ENDPOINT_BASE"), std::string("/defi/v3/search?chain=base&target=token&sort_by=buy_24h&sort_type=desc&offset=2&limit=20")}, 
    object::pair{std::string("MAX_RETRIES"), 3}, 
    object::pair{std::string("RETRY_DELAY"), 2000}
};
object ZEROEX_CONFIG = object{
    object::pair{std::string("API_URL"), std::string("https://api.0x.org")}, 
    object::pair{std::string("API_KEY"), OR((process->env->ZEROEX_API_KEY), (string_empty))}, 
    object::pair{std::string("QUOTE_ENDPOINT"), std::string("/swap/permit2/quote")}, 
    object::pair{std::string("PRICE_ENDPOINT"), std::string("/swap/permit2/price")}, 
    object::pair{std::string("SUPPORTED_CHAINS"), object{
        object::pair{std::string("BASE"), 8453}
    }}, 
    object::pair{std::string("HEADERS"), object{
        object::pair{std::string("Content-Type"), std::string("application/json")}, 
        object::pair{std::string("0x-api-key"), OR((process->env->ZEROEX_API_KEY), (string_empty))}, 
        object::pair{std::string("0x-version"), std::string("v2")}
    }}
};

void Main(void)
{
}

MAIN
