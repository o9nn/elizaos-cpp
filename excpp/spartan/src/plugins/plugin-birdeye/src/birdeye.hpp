#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "constants.hpp"
#include "elizaos/core.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// emulate settings from 0.x

/*
class BaseCachedProvider {
public:
    BaseCachedProvider(std::optional<double> ttl);

private:
    NodeCache cache_;
};

*/

// extends BaseCachedProvider
// really more like a service
class BirdeyeProvider {
public:
    BirdeyeProvider(auto runtime, std::optional<std::unordered_map<std::string, std::string>> symbolMap, std::optional<double> maxRetries);
    void fetchDefiSupportedNetworks();
    void fetchDefiPrice(DefiPriceParams params, std::any options = {});
    void fetchDefiPriceMultiple(DefiMultiPriceParams params, std::any options = {});
    void fetchDefiPriceMultiple_POST(DefiMultiPriceParamsPOST params, std::any options = {});
    void fetchDefiPriceHistorical(DefiHistoryPriceParams params, std::any options = {});
    void fetchDefiPriceHistoricalByUnixTime(HistoricalPriceUnixParams params, std::any options = {});
    void fetchDefiTradesToken(DefiTradesTokenParams params, std::any options = {});
    void fetchDefiTradesPair(DefiTradesTokenParams params, std::any options = {});
    void fetchDefiTradesTokenSeekByTime(DefiTradesTokenParams params, std::any options = {});
    void fetchDefiTradesPairSeekByTime(DefiTradesTokenParams params, std::any options = {});
    void fetchDefiOHLCV(OHLCVParams params, std::any options = {});
    void fetchDefiOHLCVPair(OHLCVPairParams params, std::any options = {});
    void fetchDefiOHLCVBaseQuote(BaseQuoteParams params, std::any options = {});
    void fetchDefiPriceVolume(PriceVolumeParams params, std::any options = {});
    void fetchDefiPriceVolumeMulti_POST(MultiPriceVolumeParams params, std::any options = {});
    void fetchTokenList(TokenListParams params, std::any options = {});
    void fetchTokenSecurityByAddress(TokenSecurityParams params, std::any options = {});
    void fetchTokenOverview(TokenOverviewParams params, std::any options = {});
    void fetchTokenCreationInfo(TokenCreationInfoParams params, std::any options = {});
    void fetchTokenTrending(std::optional<TokenTrendingParams> params, std::any options = {});
    void fetchTokenNewListing(std::optional<NewListingParams> params, std::any options = {});
    void fetchTokenTopTraders(TopTradersParams params, std::any options = {});
    void fetchTokenAllMarketsList(AllMarketsParams params, std::any options = {});
    void fetchTokenMetadataSingle(TokenMetadataSingleParams params, std::any options = {});
    void fetchTokenMetadataMulti(TokenMetadataMultiParams params, std::any options = {});
    void fetchTokenMarketData(TokenMarketDataParams params, std::any options = {});
    void fetchTokenTradeDataSingle(TokenTradeDataSingleParams params, std::any options = {});
    void fetchTokenTradeDataMultiple(TokenTradeDataMultiParams params, std::any options = {});
    void fetchTokenHolders(TokenHoldersParams params, std::any options = {});
    void fetchTokenMintBurn(MintBurnParams params, std::any options = {});
    void fetchWalletSupportedNetworks(std::any options = {});
    void fetchWalletPortfolio(WalletPortfolioParams params, std::any options = {});
    void fetchWalletPortfolioMultichain(WalletPortfolioMultichainParams params, std::any options = {});
    void fetchWalletTokenBalance(WalletTokenBalanceParams params, std::any options = {});
    void fetchWalletTransactionHistory(WalletTransactionHistoryParams params, std::any options = {});
    void fetchWalletTransactionHistoryMultichain(WalletTransactionHistoryMultichainParams params, std::any options = {});
    void fetchWalletTransactionSimulate_POST(WalletSimulationParams params, std::any options = {});
    void fetchTraderGainersLosers(GainersLosersParams params, std::any options = {});
    void fetchTraderTransactionsSeek(TraderTransactionsSeekParams params, std::any options = {});
    void fetchPairOverviewSingle(PairOverviewSingleParams params, std::any options = {});
    void fetchMultiPairOverview(PairOverviewMultiParams params, std::any options = {});
    void fetchPairOverviewMultiple(PairOverviewMultiParams params, std::any options = {});
    void fetchSearchTokenMarketData(TokenMarketSearchParams params, std::any options = {});


} // namespace elizaos
