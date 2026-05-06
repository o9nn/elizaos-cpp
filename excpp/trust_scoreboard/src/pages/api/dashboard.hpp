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
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Create cache instance with 30 minute TTL

// Constants

// Additional Interfaces
struct TokenHolding {
    std::string name;
    double amount;
    double allocation;
    double price;
    double value;
};

struct TrustScoreResult {
    double score;
    std::optional<std::string> imagePath;
    std::string label;
};

struct Partner {
    std::string owner;
    std::string displayAddress;
    double amount;
    std::string createdAt;
    double trustScore;
};

struct DashboardResponse {
    std::vector<Partner> partners;
    std::vector<std::any> holdings;
    std::vector<std::any> prices;
    std::optional<std::vector<TokenHolding>> userHoldings;
};

// Add new ai16zer function for trust score calculation with tiers

// Add function to fetch token prices from Helius
std::future<void> getTokenPrices();

// Add function to get user holdings

// Main Data Fetching Functions

std::future<void> getDAOHoldings();

// Main API Handler with retry logic

// Add function to manually clear cache if needed
void clearDashboardCache();

} // namespace elizaos
