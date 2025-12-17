#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Initialize Alchemy - note we can't use SOL_MAINNET as it's not supported

// Helper to load cached scores
std::unordered_map<std::string, double> loadCachedScores();

// Helper to save scores to cache
void saveScoresToCache(const std::unordered_map<std::string, double>& scores);

std::future<DexScreenerResponse> fetchDexScreenerData(const std::vector<std::string>& tokenAddresses);

std::future<void> fetchTokenAnalysis(const std::string& address);

// Modify calculateHoldings to make the first received date optional

// This function now serves as a wrapper around getTokenBalances for compatibility
std::future<void> getTokenData(Connection connection);

struct SolSnifferResponse {
    std::string address;
    double score;
    std::string deployTime;
    bool mintDisabled;
    bool freezeDisabled;
    bool lpBurned;
    bool top10Holders;
    std::optional<std::string> error;
};


} // namespace elizaos
