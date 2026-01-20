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
#include "tokenRegistry.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Start listening for TokenRegistered events from RegistrationHelper
 */
std::future<void> startBaseListener();

/**
 * Handle a TokenRegistered event
 */
std::future<void> handleTokenRegistered(const std::any& client, const std::any& log);

/**
 * Backfill historical events (run once after deployment)
 */
std::future<void> backfillBaseEvents(std::optional<bigint> fromBlock);

} // namespace elizaos
