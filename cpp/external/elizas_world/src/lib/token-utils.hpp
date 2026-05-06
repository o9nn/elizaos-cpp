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



// Initialize Alchemy - note we can't use SOL_MAINNET as it's not supported

// Helper to load cached scores
std::unordered_map<std::string, double> loadCachedScores();

// Helper to save scores to cache
void saveScoresToCache(Record<std::string scores, auto number>);

std::future<DexScreenerResponse> fetchDexScreenerData(const std::vector<std::string>& tokenAddresses);

std::future<void> fetchTokenAnalysis(const std::string& address);

// Modify calculateHoldings to make the first received date std::optional

// This std::function now serves as a wrapper around getTokenBalances for compatibility
std::future<void> getTokenData(Connection connection);

struct SolSnifferResponse {
    Array<{ data;
    std::string address;
    std::optional<{> tokenData;
    double score;
    { indicatorData;
    { count: number; details: std::string } high;
    { count: number; details: std::string } moderate;
    { count: number; details: std::string } low;
    { count: number; details: std::string } specific;
    std::string deployTime;
    { auditRisk;
    bool mintDisabled;
    bool freezeDisabled;
    bool lpBurned;
    bool top10Holders;
    std::optional<std::string> error;
};

std::future<std::unordered_map<std::string, double>> fetchTokenScores(const std::vector<std::string>& tokenAddresses);
} // namespace elizaos
