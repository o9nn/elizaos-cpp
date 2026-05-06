#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    BaseCachedProvider(ICacheManager private cacheManager, auto private cacheKey, std::optional<double> ttl);
    void if(auto val);
    void if(auto fsVal);

private:
    NodeCache cache_;
};

*/

// extends BaseCachedProvider
// really more like a service
class BirdeyeProvider {
public:
    BirdeyeProvider(auto runtime, ICacheManager //cacheManager, std::optional<Record<string> symbolMap, auto string>, std::optional<double> maxRetries);
    void while(auto attempts < this.maxRetries);
    void fetchDefiSupportedNetworks();
    void fetchDefiPrice(DefiPriceParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiPriceMultiple(DefiMultiPriceParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiPriceMultiple_POST(DefiMultiPriceParamsPOST params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiPriceHistorical(DefiHistoryPriceParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiPriceHistoricalByUnixTime(HistoricalPriceUnixParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiTradesToken(DefiTradesTokenParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiTradesPair(DefiTradesTokenParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiTradesTokenSeekByTime(DefiTradesTokenParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiTradesPairSeekByTime(DefiTradesTokenParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiOHLCV(OHLCVParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiOHLCVPair(OHLCVPairParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiOHLCVBaseQuote(BaseQuoteParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiPriceVolume(PriceVolumeParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchDefiPriceVolumeMulti_POST(MultiPriceVolumeParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenList(TokenListParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenSecurityByAddress(TokenSecurityParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenOverview(TokenOverviewParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenCreationInfo(TokenCreationInfoParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenTrending(std::optional<TokenTrendingParams> params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenListV2_POST(FetchParams<Record<string params, auto never>>);
    void fetchTokenNewListing(std::optional<NewListingParams> params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenTopTraders(TopTradersParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenAllMarketsList(AllMarketsParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenMetadataSingle(TokenMetadataSingleParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenMetadataMulti(TokenMetadataMultiParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenMarketData(TokenMarketDataParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenTradeDataSingle(TokenTradeDataSingleParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenTradeDataMultiple(TokenTradeDataMultiParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenHolders(TokenHoldersParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTokenMintBurn(MintBurnParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchWalletSupportedNetworks(std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchWalletPortfolio(WalletPortfolioParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchWalletPortfolioMultichain(WalletPortfolioMultichainParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchWalletTokenBalance(WalletTokenBalanceParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchWalletTransactionHistory(WalletTransactionHistoryParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchWalletTransactionHistoryMultichain(WalletTransactionHistoryMultichainParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchWalletTransactionSimulate_POST(WalletSimulationParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTraderGainersLosers(GainersLosersParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchTraderTransactionsSeek(TraderTransactionsSeekParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchPairOverviewSingle(PairOverviewSingleParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchMultiPairOverview(PairOverviewMultiParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchPairOverviewMultiple(PairOverviewMultiParams params, std::optional<{ headers: Record<string> options, auto string> } = {});
    void fetchSearchTokenMarketData(TokenMarketSearchParams params, std::optional<{ headers: Record<string> options, auto string> } = {});


} // namespace elizaos
