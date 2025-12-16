#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


using BirdeyeApiParams = std::variant<, DefiPriceParams, DefiMultiPriceParams, DefiHistoryPriceParams, HistoricalPriceUnixParams, OHLCVParams, PriceVolumeParams, MultiPriceVolumeParams, PairTradesParams, OHLCVPairParams, PairOverviewMultiParams, PairOverviewSingleParams, TokenMarketSearchParams, TokenTradesParams, TokenSecurityParams, TokenOverviewParams, TokenCreationInfoParams, TokenListV2Params, TokenMetadataMultiParams, TokenTradeDataMultiParams, GainersLosersParams, TraderTransactionsSeekParams, WalletPortfolioParams, WalletTokenBalanceParams, WalletTransactionHistoryParams, BaseQuoteParams, TokenHoldersParams, MintBurnParams, TopTradersParams, AllMarketsParams, NewListingParams, TokenMetadataSingleParams, TokenMarketDataParams, TokenTradeDataSingleParams, WalletPortfolioMultichainParams, WalletTransactionHistoryMultichainParams, WalletSimulationParams, std::unordered_map<std::string, never>>;

using BirdeyeApiResponse = BirdeyeApiResponseWrapper<any>;

using TimeInterval = std::variant<, "1m", "3m", "5m", "15m", "30m", "1H", "2H", "4H", "6H", "8H", "12H", "1D", "3D", "1W", "1M", "30m", "1h", "2h", "4h", "6h", "8h", "12h", "24h">;

struct TokenTradeData {
    std::string address;
    double holder;
    double market;
    double last_trade_unix_time;
    std::string last_trade_human_time;
    double price;
    double history_30m_price;
    double price_change_30m_percent;
    double history_1h_price;
    double price_change_1h_percent;
    double history_2h_price;
    double price_change_2h_percent;
    double history_4h_price;
    double price_change_4h_percent;
    double history_6h_price;
    double price_change_6h_percent;
    double history_8h_price;
    double price_change_8h_percent;
    double history_12h_price;
    double price_change_12h_percent;
    double history_24h_price;
    double price_change_24h_percent;
    double unique_wallet_30m;
    double unique_wallet_history_30m;
    number | null unique_wallet_30m_change_percent;
    double unique_wallet_1h;
    double unique_wallet_history_1h;
    number | null unique_wallet_1h_change_percent;
    double unique_wallet_2h;
    double unique_wallet_history_2h;
    number | null unique_wallet_2h_change_percent;
    double unique_wallet_4h;
    double unique_wallet_history_4h;
    number | null unique_wallet_4h_change_percent;
    double unique_wallet_8h;
    double unique_wallet_history_8h;
    number | null unique_wallet_8h_change_percent;
    double unique_wallet_24h;
    double unique_wallet_history_24h;
    number | null unique_wallet_24h_change_percent;
    double trade_30m;
    double trade_history_30m;
    double trade_30m_change_percent;
    double sell_30m;
    double sell_history_30m;
    double sell_30m_change_percent;
    double buy_30m;
    double buy_history_30m;
    double buy_30m_change_percent;
    double volume_30m;
    double volume_30m_usd;
    double volume_history_30m;
    double volume_history_30m_usd;
    double volume_30m_change_percent;
    double volume_buy_30m;
    double volume_buy_30m_usd;
    double volume_buy_history_30m;
    double volume_buy_history_30m_usd;
    double volume_buy_30m_change_percent;
    double volume_sell_30m;
    double volume_sell_30m_usd;
    double volume_sell_history_30m;
    double volume_sell_history_30m_usd;
    double volume_sell_30m_change_percent;
    double trade_1h;
    double trade_history_1h;
    double trade_1h_change_percent;
    double sell_1h;
    double sell_history_1h;
    double sell_1h_change_percent;
    double buy_1h;
    double buy_history_1h;
    double buy_1h_change_percent;
    double volume_1h;
    double volume_1h_usd;
    double volume_history_1h;
    double volume_history_1h_usd;
    double volume_1h_change_percent;
    double volume_buy_1h;
    double volume_buy_1h_usd;
    double volume_buy_history_1h;
    double volume_buy_history_1h_usd;
    double volume_buy_1h_change_percent;
    double volume_sell_1h;
    double volume_sell_1h_usd;
    double volume_sell_history_1h;
    double volume_sell_history_1h_usd;
    double volume_sell_1h_change_percent;
    double trade_2h;
    double trade_history_2h;
    double trade_2h_change_percent;
    double sell_2h;
    double sell_history_2h;
    double sell_2h_change_percent;
    double buy_2h;
    double buy_history_2h;
    double buy_2h_change_percent;
    double volume_2h;
    double volume_2h_usd;
    double volume_history_2h;
    double volume_history_2h_usd;
    double volume_2h_change_percent;
    double volume_buy_2h;
    double volume_buy_2h_usd;
    double volume_buy_history_2h;
    double volume_buy_history_2h_usd;
    double volume_buy_2h_change_percent;
    double volume_sell_2h;
    double volume_sell_2h_usd;
    double volume_sell_history_2h;
    double volume_sell_history_2h_usd;
    double volume_sell_2h_change_percent;
    double trade_4h;
    double trade_history_4h;
    double trade_4h_change_percent;
    double sell_4h;
    double sell_history_4h;
    double sell_4h_change_percent;
    double buy_4h;
    double buy_history_4h;
    double buy_4h_change_percent;
    double volume_4h;
    double volume_4h_usd;
    double volume_history_4h;
    double volume_history_4h_usd;
    double volume_4h_change_percent;
    double volume_buy_4h;
    double volume_buy_4h_usd;
    double volume_buy_history_4h;
    double volume_buy_history_4h_usd;
    double volume_buy_4h_change_percent;
    double volume_sell_4h;
    double volume_sell_4h_usd;
    double volume_sell_history_4h;
    double volume_sell_history_4h_usd;
    double volume_sell_4h_change_percent;
    double trade_8h;
    double trade_history_8h;
    double trade_8h_change_percent;
    double sell_8h;
    double sell_history_8h;
    double sell_8h_change_percent;
    double buy_8h;
    double buy_history_8h;
    double buy_8h_change_percent;
    double volume_8h;
    double volume_8h_usd;
    double volume_history_8h;
    double volume_history_8h_usd;
    double volume_8h_change_percent;
    double volume_buy_8h;
    double volume_buy_8h_usd;
    double volume_buy_history_8h;
    double volume_buy_history_8h_usd;
    double volume_buy_8h_change_percent;
    double volume_sell_8h;
    double volume_sell_8h_usd;
    double volume_sell_history_8h;
    double volume_sell_history_8h_usd;
    double volume_sell_8h_change_percent;
    double trade_24h;
    double trade_history_24h;
    double trade_24h_change_percent;
    double sell_24h;
    double sell_history_24h;
    double sell_24h_change_percent;
    double buy_24h;
    double buy_history_24h;
    double buy_24h_change_percent;
    double volume_24h;
    double volume_24h_usd;
    double volume_history_24h;
    double volume_history_24h_usd;
    double volume_24h_change_percent;
    double volume_buy_24h;
    double volume_buy_24h_usd;
    double volume_buy_history_24h;
    double volume_buy_history_24h_usd;
    double volume_buy_24h_change_percent;
    double volume_sell_24h;
    double volume_sell_24h_usd;
    double volume_sell_history_24h;
    double volume_sell_history_24h_usd;
    double volume_sell_24h_change_percent;
};


} // namespace elizaos
