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
#include "agent-runtime.hpp"
#include "elizaos/core.hpp"
#include "entityId.hpp"
#include "services/priceFeed.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    // Only provide quote context if user is asking about quotes/terms/pricing

    // Use QuoteService for consistent quote lookup

std::future<QuoteMemory> getUserQuote(const std::string& walletAddress);

    // Token metadata (optional)

  // Generate consistent quote ID using the same method as QuoteService

  // Generate signature using same method as QuoteService

    // Token metadata

  // Add to indexes

  // Add to beneficiary index for faster lookups

std::future<void> deleteUserQuote(const std::string& walletAddress);

} // namespace elizaos
