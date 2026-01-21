#include ".constants.hpp"
#include ".services/GamificationService.hpp"
#include ".services/ReferralService.hpp"
#include ".utils/contentQuality.hpp"
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



/**
 * Validate and sanitize volume USD value
 * Returns 0 for invalid/negative values
 */
double validateVolumeUsd(unknown volume);

std::future<UUID | null> getUserIdFromMessage(ActionEventPayload['runtime'] runtime, std::optional<UUID> messageId, std::optional<UUID> roomId, std::optional<UUID> entityId);

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
        await recordAgentActionPoints(payload);

        await recordChatPoints(payload);

      await recordAccountCreationPoints(payload);


} // namespace elizaos
