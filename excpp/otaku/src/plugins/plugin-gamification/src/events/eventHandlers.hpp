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

import type { PluginEvents, ActionEventPayload, RunEventPayload, EntityPayload, ActionResult, UUID, Memory } from '@elizaos/core';
;
;
;
;
;

interface ActionResultWithValues extends ActionResult {
  values?: {
    volumeUsd?: number;
    valueUsd?: number;
    destinationChain?: string;
    toChain?: string;
    swapSuccess?: boolean;
  };
}

/**
 * Validate and sanitize volume USD value
 * Returns 0 for invalid/negative values
 */


std::future<UUID | null> getUserIdFromMessage(ActionEventPayload['runtime'] runtime, std::optional<UUID> messageId, std::optional<UUID> roomId, std::optional<UUID> entityId);
      }
      
      // Otherwise, use the entity ID directly (it's already the user entity)
      return id;
    } catch {
      return null;
    }
  };

  // Check entityId first (most efficient)
  if (entityId) {
    return await resolveActualUserId(entityId);
  }
  
  if (!messageId || !roomId) return null;
  
  try {
    // Try to get message directly if possible (more efficient than fetching 100)
    // For now, fallback to fetching memories, but limit to smaller count
    const memories = await runtime.getMemories({
      tableName: 'messages',
      roomId,
      count: 50, // Reduced from 100
    });
    const message = memories.find((m: Memory) => m.id === messageId);
    if (!message?.entityId) return null;
    
    return await resolveActualUserId(message.entityId);
  } catch {
    return null;
  }
}

std::future<bool> recordSwapPoints(ActionEventPayload payload);

    // Also check swapSuccess flag if present (for extra safety)
    if (actionResult.values?.swapSuccess === false) {
      logger.debug('[Gamification] Skipping points for swap marked as unsuccessful');
      return false;
    }
    
    // Validate volumeUsd to prevent negative/NaN values
    // Use ?? instead of || to preserve valid zero values
    const volumeUsd = validateVolumeUsd(actionResult?.values?.volumeUsd ?? actionResult?.values?.valueUsd);

    const userId = await getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
    if (!userId) return false;

    await gamificationService.recordEvent({
      userId,
      actionType: GamificationEventType.SWAP_COMPLETED,
      volumeUsd,
      metadata: { actionResult },
      sourceEventId: payload.messageId,
    });
    
    return true; // Return true to indicate we handled this action
  } catch (error) {
    logger.error({ error }, '[Gamification] Error recording swap points');
    return false;
  }
}

std::future<bool> recordBridgePoints(ActionEventPayload payload);
    
    // Validate volumeUsd to prevent negative/NaN values
    // Use ?? instead of || to preserve valid zero values
    const volumeUsd = validateVolumeUsd(actionResult?.values?.volumeUsd ?? actionResult?.values?.valueUsd);
    const chain = actionResult?.values?.destinationChain ?? actionResult?.values?.toChain;

    const userId = await getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
    if (!userId) return false;

    await gamificationService.recordEvent({
      userId,
      actionType: GamificationEventType.BRIDGE_COMPLETED,
      volumeUsd,
      chain,
      metadata: { actionResult },
      sourceEventId: payload.messageId,
    });
    
    return true; // Return true to indicate we handled this action
  } catch (error) {
    logger.error({ error }, '[Gamification] Error recording bridge points');
    return false;
  }
}

std::future<bool> recordTransferPoints(ActionEventPayload payload);
    
    // Validate valueUsd to prevent negative/NaN values
    const valueUsd = validateVolumeUsd(actionResult?.values?.valueUsd);

    // Use constant instead of magic number
    if (valueUsd < MIN_TRANSFER_VALUE_USD) return false;

    const userId = await getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
    if (!userId) return false;

    await gamificationService.recordEvent({
      userId,
      actionType: GamificationEventType.TRANSFER_COMPLETED,
      volumeUsd: valueUsd,
      metadata: { actionResult },
      sourceEventId: payload.messageId,
    });
    
    return true; // Return true to indicate we handled this action
  } catch (error) {
    logger.error({ error }, '[Gamification] Error recording transfer points');
    return false;
  }
}

/**
 * Calculate points based on message length tiers
 */

  
  if (messageLength < MIN_CHAT_LENGTH) return 0;
  
  for (const tier of MESSAGE_LENGTH_TIERS) {
    if (messageLength >= tier.minLength && messageLength <= tier.maxLength) {
      return tier.points;
    }
  }
  
  return 0;
}

std::future<void> recordChatPoints(RunEventPayload payload););
        const message = memories.find((m) => m.id === payload.messageId);
        input = message?.content?.text || '';
      }
    } catch (error) {
      // If we can't get the message, skip
      logger.debug({ error }, '[Gamification] Could not fetch message for chat points');
      return;
    }

    const messageLength = input.length;
    const points = calculateChatPoints(messageLength);
    
    // Skip if message is too short or no points
    if (points === 0) return;

    // Check content quality to prevent spam/copy-pasta from earning points
    const qualityResult = checkContentQuality(input);
    if (!qualityResult.isValid) {
      logger.debug(
        { 
          reason: qualityResult.reason, 
          score: qualityResult.score,
          messagePreview: input.substring(0, 50),
        },
        '[Gamification] Message failed content quality check, no points awarded'
      );
      return;
    }

    const gamificationService = payload.runtime.getService('gamification') as GamificationService;
    if (!gamificationService) return;

    // Resolve actual user ID (handles agent-scoped entities)
    const userId = await getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
    if (!userId) return;

    // Store the calculated points in metadata to override BASE_POINTS
    await gamificationService.recordEvent({
      userId,
      actionType: GamificationEventType.MEANINGFUL_CHAT,
      metadata: { 
        inputLength: messageLength,
        tier: points,
        contentQualityScore: qualityResult.score,
      },
      sourceEventId: payload.messageId,
    });
  } catch (error) {
    logger.error({ error }, '[Gamification] Error recording chat points');
  }
}

std::future<void> recordAgentActionPoints(ActionEventPayload payload);

    const userId = await getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
    if (!userId) return;

    const actionName = payload.content?.actions?.[0] || 'unknown';

    await gamificationService.recordEvent({
      userId,
      actionType: GamificationEventType.AGENT_ACTION,
      metadata: { 
        actionName,
        actionResult,
      },
      sourceEventId: payload.messageId,
    });
  } catch (error) {
    logger.error({ error }, '[Gamification] Error recording agent action points');
  }
}

std::future<void> recordAccountCreationPoints(EntityPayload payload);,
      });
    }

    // Process referral if present
    const referralService = payload.runtime.getService('referral') as ReferralService;
    if (referralService) {
      try {
        // Use the actual user entity for referral processing
        const entity = await payload.runtime.getEntityById(userId);
        const referredBy = entity?.metadata?.referredBy;
        
        if (referredBy && typeof referredBy === 'string') {
          logger.info(`[Gamification] Processing referral code ${referredBy} for user ${userId}`);
          await referralService.processReferralSignup(userId, referredBy);
        }
      } catch (err) {
        logger.error({ error: err }, '[Gamification] Error processing referral in account creation');
      }
    }
  } catch (error) {
    logger.error({ error }, '[Gamification] Error recording account creation points');
  }
}

const gamificationEvents: PluginEvents = {
  [EventType.ACTION_COMPLETED]: [
    async (payload: ActionEventPayload) => {
      const actionName = payload.content?.actions?.[0];

      // Award specific action points
      let handled = false;
      if (actionName === 'USER_WALLET_SWAP') {
        handled = await recordSwapPoints(payload);
      } else if (actionName === 'EXECUTE_RELAY_BRIDGE' || actionName === 'RELAY_BRIDGE') {
        handled = await recordBridgePoints(payload);
      } else if (actionName === 'USER_WALLET_TOKEN_TRANSFER' || actionName === 'USER_WALLET_NFT_TRANSFER') {
        handled = await recordTransferPoints(payload);
      }

      // Only award generic 10 points if no specific handler processed this action
      if (!handled) {
        await recordAgentActionPoints(payload);
      }
    },
  ],

  [EventType.RUN_ENDED]: [
    async (payload: RunEventPayload) => {
      if (payload.status === 'completed') {
        await recordChatPoints(payload);
      }
    },
  ],

  [EventType.ENTITY_JOINED]: [
    async (payload: EntityPayload) => {
      await recordAccountCreationPoints(payload);
    },
  ],
};


} // namespace elizaos
