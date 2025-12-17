#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".database.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TokenMetadataRequest {
    std::string mint;
    double priority;
    double retryCount;
    Date requestedAt;
};

struct TokenMetadataResponse {
    std::string symbol;
    std::string name;
    std::optional<std::string> logoURI;
};

class TokenMetadataService {
public:
    TokenMetadataService();
    void addToQueue(const std::string& mint, double priority = 0);
    void addBatchToQueue(const std::vector<std::string>& mints, double priority = 0);
    bool isMetadataFresh(const std::string& lastUpdated);
    std::future<void> startProcessing();
    std::future<void> processTokenMetadata(TokenMetadataRequest request);
     getQueueStatus();
    void clearQueue();
    std::vector<std::string> getTokensNeedingMetadata();
    void refreshStaleMetadata();
    Promise< healthCheck();
    std::future<void> gracefulShutdown();
    void startBackgroundRefresh(double intervalMinutes = 60);
    void printBatchSummary();
     getBatchStats();

private:
    DatabaseService db_;
    std::string jupiterTokensApiUrl_;
};

// Create singleton instance

} // namespace elizaos
