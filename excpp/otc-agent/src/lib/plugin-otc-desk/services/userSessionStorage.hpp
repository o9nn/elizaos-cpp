#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// User session storage service using Eliza's cache system

struct UserSessionMemory {
    std::string id;
    std::string entityId;
    std::string walletAddress;
    double quotesCreated;
    double lastQuoteAt;
    double dailyQuoteCount;
    double dailyResetAt;
    double totalDeals;
    double totalVolumeUsd;
    double totalSavedUsd;
    double createdAt;
    double updatedAt;
};

class UserSessionStorageService {
public:
    UserSessionStorageService(IAgentRuntime runtime);
    std::future<void> initialize();
    std::future<void> stop();
    static std::future<UserSessionStorageService> start(IAgentRuntime runtime);
    std::future<UserSessionMemory> getOrCreateSession(const std::string& entityId, const std::string& walletAddress);
    std::future<void> checkRateLimit(const std::string& entityId, const std::string& walletAddress);
    std::future<void> incrementQuoteCount(const std::string& entityId, const std::string& walletAddress);
    std::future<void> updateDealStats(const std::string& entityId, const std::string& walletAddress, double volumeUsd, double savedUsd);
};

UserSessionStorageService getUserSessionStorageService(IAgentRuntime runtime);

} // namespace elizaos
