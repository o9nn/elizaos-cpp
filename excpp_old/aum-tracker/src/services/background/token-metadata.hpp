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
    void addToQueue(const std::string& mint, number = 0 priority);
    void addBatchToQueue(const std::vector<std::string>& mints, number = 0 priority);
    bool isMetadataFresh(const std::string& lastUpdated);
    std::future<void> startProcessing();
    std::future<void> processTokenMetadata(TokenMetadataRequest request);
    void for(auto let attempt = 0; attempt < maxRetries; attempt++);
     getQueueStatus();
    void clearQueue();
    std::vector<std::string> getTokensNeedingMetadata();
    void refreshStaleMetadata();
    Promise< healthCheck();
    void if(auto !response.ok);
    void catch(auto error);
    std::future<void> gracefulShutdown();
    void startBackgroundRefresh(number = 60 intervalMinutes);
    void printBatchSummary();
     getBatchStats();

private:
    DatabaseService db_;
    std::string jupiterTokensApiUrl_;
};

// Create singleton instance

} // namespace elizaos
