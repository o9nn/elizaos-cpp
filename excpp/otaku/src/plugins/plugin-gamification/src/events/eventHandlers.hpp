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
#include "constants.hpp"
#include "elizaos/core.hpp"
#include "services/GamificationService.hpp"
#include "services/ReferralService.hpp"
#include "utils/contentQuality.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Validate and sanitize volume USD value
 * Returns 0 for invalid/negative values
 */
double validateVolumeUsd(const std::any& volume);

std::future<bool> recordSwapPoints(ActionEventPayload payload);

std::future<bool> recordBridgePoints(ActionEventPayload payload);

std::future<bool> recordTransferPoints(ActionEventPayload payload);

/**
 * Calculate points based on message length tiers
 */
double calculateChatPoints(double messageLength);

std::future<void> recordChatPoints(RunEventPayload payload);

std::future<void> recordAgentActionPoints(ActionEventPayload payload);

std::future<void> recordAccountCreationPoints(EntityPayload payload);

      // Award specific action points

      // Only award generic 10 points if no specific handler processed this action


} // namespace elizaos
