#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CoinGeckoTokenMetadata {
    std::string id;
    std::string symbol;
    std::string name;
    std::optional<std::string> asset_platform_id;
    std::optional<std::string> contract_address;
    std::optional<std::any> market_data;
    std::optional<std::any> links;
    std::optional<std::any> image;
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
    static std::future<CoinGeckoService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<std::vector<TokenMetadataResolution>> getTokenMetadata(const std::variant<std::string, std::vector<std::string>>& ids);
    std::future<TokenMetadataResolution> handleContractLookup(const std::string& baseUrl, const std::string& address, const std::vector<std::string>& platforms, const std::string& networkLabel);
    std::future<void> loadCoinsIndex();
    std::future<std::vector<TokenMetadataCandidate>> resolveCandidates(const std::string& input);
    std::future<std::vector<TokenMetadataCandidate>> rankByMarkets(const std::vector<std::string>& ids);
    std::future<std::vector<MarketRow>> fetchMarketRows(const std::vector<std::string>& ids);
    std::future<std::vector<TokenMetadataCandidate>> enrichCandidateMetadata(const std::string& baseUrl, const std::vector<TokenMetadataCandidate>& candidates, double limit = 3);
    std::variant<Promise<CoinGeckoTokenMetadata, null>> fetchCoinDetail(const std::string& baseUrl, const std::string& coinId);
    std::string extractCoinId(const std::unordered_map<std::string, std::any>& metadata, const std::string& fallback);
    std::future<std::any> getNFTCollectionStats(const std::string& collectionIdentifier);
    std::future<std::any> getTrendingSearch();
    std::string formatDateForTimeframe(double timestamp, const std::string& timeframe);
    Promise<Array< getCategoriesList();

private:
    std::string proApiKey_;
};

bool isEvmAddress(const std::string& s);

bool isSolanaAddress(const std::string& s);


} // namespace elizaos
