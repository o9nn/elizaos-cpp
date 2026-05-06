#include ".types.hpp"
#include ".utils/errors.hpp"
#include ".utils/transactions.hpp"
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



// Utility function to sanitize errors by converting BigInt values to strings
std::any sanitizeError(const std::any& error);

// Helper function to detect platform from URL
std::string detectPlatform(const std::string& url);

class ClankerService extends Service {
  static serviceType = "clanker";
  capabilityDescription = "";
  private transactionManager: CdpTransactionManager;

  constructor(runtime: IAgentRuntime) {
    super(runtime);
    this.transactionManager = CdpTransactionManager.getInstance();
  }

    // Test connections

      // Validate parameters

          // Transform string URLs to objects with platform and url

          // Sanitize error to remove BigInt before logging/throwing

          // Sanitize error to remove BigInt before logging/throwing
        
        // Fetch transaction receipt to get actual deployment cost
          // Keep deploymentCost as 0 if we can't fetch it

      // Sanitize error to remove BigInt before logging/throwing


} // namespace elizaos
