#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".utils.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CoinGeckoTokenMetadata {
    std::string id;
    std::string symbol;
    std::string name;
    std::optional<std::optional<std::string>> asset_platform_id;
    std::optional<std::optional<std::string>> contract_address;
    std::optional<unknown> market_data;
    std::optional<unknown> links;
    std::optional<unknown> image;
};

struct MarketRow {
    std::string id;
    std::optional<std::optional<double>> market_cap;
    std::optional<std::optional<double>> total_volume;
    std::optional<std::optional<double>> market_cap_rank;
};

struct TokenMetadataCandidate {
    std::string id;
    std::string coinId;
    double confidence;
    std::optional<double> marketCap;
    std::optional<double> totalVolume;
    std::optional<double> marketCapRank;
};

struct TokenMetadataResolution {
    std::string id;
    bool success;
    std::optional<std::string> resolvedCoinId;
    std::optional<std::string> error;
    std::vector<TokenMetadataCandidate> candidates;
};

/**
 * Map of native token symbols to their CoinGecko IDs
 * These tokens can be used directly by symbol in price chart queries
 */

class CoinGeckoService {
public:
    CoinGeckoService(IAgentRuntime runtime);
    std::future<CoinGeckoService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<std::vector<TokenMetadataResolution>> getTokenMetadata(const std::variant<std::string, std::vector<std::string>>& ids);
    std::variant<std::future<Record<string, unknown>, null>> fetchByContractAddress(const std::string& baseUrl, const std::string& address, const std::vector<std::string>& platforms);
    std::future<TokenMetadataResolution> handleContractLookup(const std::string& baseUrl, const std::string& address, const std::vector<std::string>& platforms, const std::string& networkLabel);
    std::future<void> loadCoinsIndex();
    std::future<std::vector<TokenMetadataCandidate>> resolveCandidates(const std::string& input);
    std::future<std::vector<TokenMetadataCandidate>> rankByMarkets(const std::vector<std::string>& ids);
    std::future<std::vector<MarketRow>> fetchMarketRows(const std::vector<std::string>& ids);
    std::future<std::vector<TokenMetadataCandidate>> enrichCandidateMetadata(const std::string& baseUrl, const std::vector<TokenMetadataCandidate>& candidates, number = 3 limit);
    std::variant<Promise<CoinGeckoTokenMetadata, null>> fetchCoinDetail(const std::string& baseUrl, const std::string& coinId);
    std::string extractCoinId(Record<string metadata, auto unknown>, const std::string& fallback);
    std::future<std::any> getTrendingTokens(string = "base" network, number = 10 limit);
    std::future<std::any> getNFTCollectionStats(const std::string& collectionIdentifier);
    std::future<std::any> getTrendingSearch();
    std::future<std::any> getTokenPriceChart(const std::string& tokenIdentifier, string = '24h' timeframe, string = 'base' chain);
    std::string formatDateForTimeframe(double timestamp, const std::string& timeframe);
    Promise< getHistoricalPrice(const std::string& tokenIdentifier, const std::string& date, string = 'base' chain);
    void if(auto isContractAddress);
    void if(auto !res.ok);
    void if(auto !tokenName);
    void if(auto !tokenSymbol);
    void catch(auto err);
    Promise<Array< getCategoriesList();
    void if(auto !res.ok);
    void catch(auto err);
    Promise<Array< getCategoriesWithMarketData(const std::variant<'market_cap_desc', 'market_cap_asc', 'name_desc', 'name_asc', 'market_cap_change_24h_desc', 'market_cap_change_24h_asc' = 'market_cap_desc'>& order);
    void if(auto order);
    void if(auto !res.ok);
    void catch(auto err);

private:
    std::string proApiKey_;
};

bool isEvmAddress(const std::string& s);

bool isSolanaAddress(const std::string& s);


} // namespace elizaos
