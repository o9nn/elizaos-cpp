#include "elizaos/core.hpp"
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

class UserSessionStorageService extends Service {
  static serviceType = "UserSessionService";
  capabilityDescription = "User session and rate limiting service";

  constructor(runtime: IAgentRuntime) {
    super();
    this.runtime = runtime;
  }

    // Cleanup if needed

UserSessionStorageService getUserSessionStorageService(IAgentRuntime runtime);

} // namespace elizaos
