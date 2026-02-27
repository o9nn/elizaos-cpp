#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-birdeye/src/constants.h"

double DEFAULT_MAX_RETRIES = 3;
string BIRDEYE_SERVICE_NAME = std:("birdeye");
object DEFAULT_SUPPORTED_SYMBOLS = object{
    object::pair{std:("SOL"), std:("So11111111111111111111111111111111111111112")}, 
    object::pair{std:("BTC"), std:("qfnqNqs3nCAHjnyCgLRDbBtq4p2MtHZxw8YjSyYhPoL")}, 
    object::pair{std:("ETH"), std:("7vfCXTUXx5WJV5JADk17DUJ4ksgau7utNKj4b963voxs")}, 
    object::pair{std:("Example"), std:("2weMjPLLybRMMva1fM3U31goWWrCpF59CHWNhnCJ9Vyh")}
};
string API_BASE_URL = std:("https://public-api.birdeye.so");
double RETRY_DELAY_MS = 2000;
object BIRDEYE_ENDPOINTS = object{
    object::pair{std:("defi"), object{
        object::pair{std:("networks"), std:("/defi/networks")}, 
        object::pair{std:("price"), std:("/defi/price")}, 
        object::pair{std:("price_multi"), std:("/defi/multi_price")}, 
        object::pair{std:("price_multi_POST"), std:("/defi/multi_price")}, 
        object::pair{std:("history_price"), std:("/defi/history_price")}, 
        object::pair{std:("historical_price_unix"), std:("/defi/historical_price_unix")}, 
        object::pair{std:("trades_token"), std:("/defi/txs/token")}, 
        object::pair{std:("trades_pair"), std:("/defi/txs/pair")}, 
        object::pair{std:("trades_token_seek"), std:("/defi/txs/token/seek_by_time")}, 
        object::pair{std:("trades_pair_seek"), std:("/defi/txs/pair/seek_by_time")}, 
        object::pair{std:("ohlcv"), std:("/defi/ohlcv")}, 
        object::pair{std:("ohlcv_pair"), std:("/defi/ohlcv/pair")}, 
        object::pair{std:("ohlcv_base_quote"), std:("/defi/ohlcv/base_quote")}, 
        object::pair{std:("price_volume"), std:("/defi/price_volume/single")}, 
        object::pair{std:("price_volume_multi"), std:("/defi/price_volume/multi")}, 
        object::pair{std:("price_volume_multi_POST"), std:("/defi/price_volume/multi")}
    }}, 
    object::pair{std:("token"), object{
        object::pair{std:("list_all"), std:("/defi/tokenlist")}, 
        object::pair{std:("security"), std:("/defi/token_security")}, 
        object::pair{std:("overview"), std:("/defi/token_overview")}, 
        object::pair{std:("creation_info"), std:("/defi/token_creation_info")}, 
        object::pair{std:("trending"), std:("/defi/token_trending")}, 
        object::pair{std:("list_all_v2_POST"), std:("/defi/v2/tokens/all")}, 
        object::pair{std:("new_listing"), std:("/defi/v2/tokens/new_listing")}, 
        object::pair{std:("top_traders"), std:("/defi/v2/tokens/top_traders")}, 
        object::pair{std:("all_markets"), std:("/defi/v2/markets")}, 
        object::pair{std:("metadata_single"), std:("/defi/v3/token/meta-data/single")}, 
        object::pair{std:("metadata_multi"), std:("/defi/v3/token/meta-data/multiple")}, 
        object::pair{std:("market_data"), std:("/defi/v3/token/market-data")}, 
        object::pair{std:("trade_data_single"), std:("/defi/v3/token/trade-data/single")}, 
        object::pair{std:("trade_data_multi"), std:("/defi/v3/token/trade-data/multiple")}, 
        object::pair{std:("holders"), std:("/defi/v3/token/holder")}, 
        object::pair{std:("mint_burn"), std:("/defi/v3/token/mint-burn-txs")}
    }}, 
    object::pair{std:("wallet"), object{
        object::pair{std:("networks"), std:("/v1/wallet/list_supported_chain")}, 
        object::pair{std:("portfolio"), std:("/v1/wallet/token_list")}, 
        object::pair{std:("portfolio_multichain"), std:("/v1/wallet/multichain_token_list")}, 
        object::pair{std:("token_balance"), std:("/v1/wallet/token_balance")}, 
        object::pair{std:("transaction_history"), std:("/v1/wallet/tx_list")}, 
        object::pair{std:("transaction_history_multichain"), std:("/v1/wallet/multichain_tx_list")}, 
        object::pair{std:("transaction_simulation_POST"), std:("/v1/wallet/simulate")}
    }}, 
    object::pair{std:("trader"), object{
        object::pair{std:("gainers_losers"), std:("/trader/gainers-losers")}, 
        object::pair{std:("trades_seek"), std:("/trader/txs/seek_by_time")}
    }}, 
    object::pair{std:("pair"), object{
        object::pair{std:("overview_multi"), std:("/defi/v3/pair/overview/multiple")}, 
        object::pair{std:("overview_single"), std:("/defi/v3/pair/overview/single")}
    }}, 
    object::pair{std:("search"), object{
        object::pair{std:("token_market"), std:("/defi/v3/search")}
    }}
};

void Main(void)
{
}

MAIN
