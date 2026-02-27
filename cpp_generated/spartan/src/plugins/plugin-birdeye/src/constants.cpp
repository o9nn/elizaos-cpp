#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-birdeye/src/constants.h"

double DEFAULT_MAX_RETRIES = 3;
string BIRDEYE_SERVICE_NAME = std::string("birdeye");
object DEFAULT_SUPPORTED_SYMBOLS = object{
    object::pair{std::string("SOL"), std::string("So11111111111111111111111111111111111111112")}, 
    object::pair{std::string("BTC"), std::string("qfnqNqs3nCAHjnyCgLRDbBtq4p2MtHZxw8YjSyYhPoL")}, 
    object::pair{std::string("ETH"), std::string("7vfCXTUXx5WJV5JADk17DUJ4ksgau7utNKj4b963voxs")}, 
    object::pair{std::string("Example"), std::string("2weMjPLLybRMMva1fM3U31goWWrCpF59CHWNhnCJ9Vyh")}
};
string API_BASE_URL = std::string("https://public-api.birdeye.so");
double RETRY_DELAY_MS = 2000;
object BIRDEYE_ENDPOINTS = object{
    object::pair{std::string("defi"), object{
        object::pair{std::string("networks"), std::string("/defi/networks")}, 
        object::pair{std::string("price"), std::string("/defi/price")}, 
        object::pair{std::string("price_multi"), std::string("/defi/multi_price")}, 
        object::pair{std::string("price_multi_POST"), std::string("/defi/multi_price")}, 
        object::pair{std::string("history_price"), std::string("/defi/history_price")}, 
        object::pair{std::string("historical_price_unix"), std::string("/defi/historical_price_unix")}, 
        object::pair{std::string("trades_token"), std::string("/defi/txs/token")}, 
        object::pair{std::string("trades_pair"), std::string("/defi/txs/pair")}, 
        object::pair{std::string("trades_token_seek"), std::string("/defi/txs/token/seek_by_time")}, 
        object::pair{std::string("trades_pair_seek"), std::string("/defi/txs/pair/seek_by_time")}, 
        object::pair{std::string("ohlcv"), std::string("/defi/ohlcv")}, 
        object::pair{std::string("ohlcv_pair"), std::string("/defi/ohlcv/pair")}, 
        object::pair{std::string("ohlcv_base_quote"), std::string("/defi/ohlcv/base_quote")}, 
        object::pair{std::string("price_volume"), std::string("/defi/price_volume/single")}, 
        object::pair{std::string("price_volume_multi"), std::string("/defi/price_volume/multi")}, 
        object::pair{std::string("price_volume_multi_POST"), std::string("/defi/price_volume/multi")}
    }}, 
    object::pair{std::string("token"), object{
        object::pair{std::string("list_all"), std::string("/defi/tokenlist")}, 
        object::pair{std::string("security"), std::string("/defi/token_security")}, 
        object::pair{std::string("overview"), std::string("/defi/token_overview")}, 
        object::pair{std::string("creation_info"), std::string("/defi/token_creation_info")}, 
        object::pair{std::string("trending"), std::string("/defi/token_trending")}, 
        object::pair{std::string("list_all_v2_POST"), std::string("/defi/v2/tokens/all")}, 
        object::pair{std::string("new_listing"), std::string("/defi/v2/tokens/new_listing")}, 
        object::pair{std::string("top_traders"), std::string("/defi/v2/tokens/top_traders")}, 
        object::pair{std::string("all_markets"), std::string("/defi/v2/markets")}, 
        object::pair{std::string("metadata_single"), std::string("/defi/v3/token/meta-data/single")}, 
        object::pair{std::string("metadata_multi"), std::string("/defi/v3/token/meta-data/multiple")}, 
        object::pair{std::string("market_data"), std::string("/defi/v3/token/market-data")}, 
        object::pair{std::string("trade_data_single"), std::string("/defi/v3/token/trade-data/single")}, 
        object::pair{std::string("trade_data_multi"), std::string("/defi/v3/token/trade-data/multiple")}, 
        object::pair{std::string("holders"), std::string("/defi/v3/token/holder")}, 
        object::pair{std::string("mint_burn"), std::string("/defi/v3/token/mint-burn-txs")}
    }}, 
    object::pair{std::string("wallet"), object{
        object::pair{std::string("networks"), std::string("/v1/wallet/list_supported_chain")}, 
        object::pair{std::string("portfolio"), std::string("/v1/wallet/token_list")}, 
        object::pair{std::string("portfolio_multichain"), std::string("/v1/wallet/multichain_token_list")}, 
        object::pair{std::string("token_balance"), std::string("/v1/wallet/token_balance")}, 
        object::pair{std::string("transaction_history"), std::string("/v1/wallet/tx_list")}, 
        object::pair{std::string("transaction_history_multichain"), std::string("/v1/wallet/multichain_tx_list")}, 
        object::pair{std::string("transaction_simulation_POST"), std::string("/v1/wallet/simulate")}
    }}, 
    object::pair{std::string("trader"), object{
        object::pair{std::string("gainers_losers"), std::string("/trader/gainers-losers")}, 
        object::pair{std::string("trades_seek"), std::string("/trader/txs/seek_by_time")}
    }}, 
    object::pair{std::string("pair"), object{
        object::pair{std::string("overview_multi"), std::string("/defi/v3/pair/overview/multiple")}, 
        object::pair{std::string("overview_single"), std::string("/defi/v3/pair/overview/single")}
    }}, 
    object::pair{std::string("search"), object{
        object::pair{std::string("token_market"), std::string("/defi/v3/search")}
    }}
};

void Main(void)
{
}

MAIN
