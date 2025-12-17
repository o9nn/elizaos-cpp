#include "eventHandlers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double validateVolumeUsd(unknown volume) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto num = Number(volume);
    return Number.isFinite(num) && num >= 0 ? num : 0;

}

std::future<std::optional<UUID>> getUserIdFromMessage(ActionEventPayload['runtime'] runtime, std::optional<UUID> messageId, std::optional<UUID> roomId, std::optional<UUID> entityId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Helper to resolve the actual user ID from an entity (handles agent-scoped entities)
    const auto resolveActualUserId = async (id: UUID): Promise<UUID | nullptr> => {;
        try {
            const auto entity = runtime.getEntityById(id);
            if (!entity) return null;

            // If entity has author_id in metadata, it's an agent-scoped entity - use the actual user ID
            if (entity.metadata.author_id && typeof entity.metadata.author_id == 'string') {
                const auto actualUserId = entity.metadata.author_id;
                // Verify the actual user entity exists
                const auto userEntity = runtime.getEntityById(actualUserId);
                if (userEntity) {
                    return actualUserId;
                }
            }

            // Otherwise, use the entity ID directly (it's already the user entity)
            return id;
            } catch {
                return nullptr;
            }
            };

            // Check entityId first (most efficient)
            if (entityId) {
                return resolveActualUserId(entityId);
            }

            if (!messageId || !roomId) return null;

            try {
                // Try to get message directly if possible (more efficient than fetching 100)
                // For now, fallback to fetching memories, but limit to smaller count
                const auto memories = runtime.getMemories({;
                    tableName: 'messages',
                    roomId,
                    count: 50, // Reduced from 100
                    });
                    const auto message = memories.find((m: Memory) => m.id == messageId);
                    if (!message.entityId) return null;

                    return resolveActualUserId(message.entityId);
                    } catch {
                        return nullptr;
                    }

}

std::future<bool> recordSwapPoints(ActionEventPayload payload) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto gamificationService = payload.runtime.getService('gamification');
        if (!gamificationService) return false;

        // Handle both actionResults (array) and actionResult (single) formats
        const auto actionResults = payload.content.actionResults;
        const auto actionResultSingle = payload.content.actionResult;
        const auto actionResult: ActionResultWithValues | std::nullopt =;
        Array.isArray(actionResults) && actionResults.length > 0;
        ? (actionResults[0]);
        : actionResultSingle
        ? (actionResultSingle);
        : std::nullopt;

        // Only award points for successful swaps
        if (!actionResult || actionResult.success != true) {
            logger.debug('[Gamification] Skipping points for unsuccessful swap');
            return false;
        }

        // Also check swapSuccess flag if present (for extra safety)
        if (actionResult.values.swapSuccess == false) {
            logger.debug('[Gamification] Skipping points for swap marked as unsuccessful');
            return false;
        }

        // Validate volumeUsd to prevent negative/NaN values
        // Use ?? instead of || to preserve valid zero values
        const auto volumeUsd = validateVolumeUsd(actionResult.values.volumeUsd || actionResult.values.valueUsd);

        const auto userId = getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
        if (!userId) return false;

        gamificationService.recordEvent({
            userId,
            actionType: GamificationEventType.SWAP_COMPLETED,
            volumeUsd,
            metadata: { actionResult },
            sourceEventId: payload.messageId,
            });

            return true; // Return true to indicate we handled this action;
            } catch (error) {
                std::cerr << { error } << '[Gamification] Error recording swap points' << std::endl;
                return false;
            }

}

std::future<bool> recordBridgePoints(ActionEventPayload payload) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto gamificationService = payload.runtime.getService('gamification');
        if (!gamificationService) return false;

        // Handle both actionResults (array) and actionResult (single) formats
        const auto actionResults = payload.content.actionResults;
        const auto actionResultSingle = payload.content.actionResult;
        const auto actionResult: ActionResultWithValues | std::nullopt =;
        Array.isArray(actionResults) && actionResults.length > 0;
        ? (actionResults[0]);
        : actionResultSingle
        ? (actionResultSingle);
        : std::nullopt;

        // Only award points for successful bridges
        if (!actionResult || actionResult.success != true) {
            logger.debug('[Gamification] Skipping points for unsuccessful bridge');
            return false;
        }

        // Validate volumeUsd to prevent negative/NaN values
        // Use ?? instead of || to preserve valid zero values
        const auto volumeUsd = validateVolumeUsd(actionResult.values.volumeUsd || actionResult.values.valueUsd);
        const auto chain = actionResult.values.destinationChain || actionResult.values.toChain;

        const auto userId = getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
        if (!userId) return false;

        gamificationService.recordEvent({
            userId,
            actionType: GamificationEventType.BRIDGE_COMPLETED,
            volumeUsd,
            chain,
            metadata: { actionResult },
            sourceEventId: payload.messageId,
            });

            return true; // Return true to indicate we handled this action;
            } catch (error) {
                std::cerr << { error } << '[Gamification] Error recording bridge points' << std::endl;
                return false;
            }

}

std::future<bool> recordTransferPoints(ActionEventPayload payload) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto gamificationService = payload.runtime.getService('gamification');
        if (!gamificationService) return false;

        // Handle both actionResults (array) and actionResult (single) formats
        const auto actionResults = payload.content.actionResults;
        const auto actionResultSingle = payload.content.actionResult;
        const auto actionResult: ActionResultWithValues | std::nullopt =;
        Array.isArray(actionResults) && actionResults.length > 0;
        ? (actionResults[0]);
        : actionResultSingle
        ? (actionResultSingle);
        : std::nullopt;

        // Only award points for successful transfers
        if (!actionResult || actionResult.success != true) {
            logger.debug('[Gamification] Skipping points for unsuccessful transfer');
            return false;
        }

        // Validate valueUsd to prevent negative/NaN values
        const auto valueUsd = validateVolumeUsd(actionResult.values.valueUsd);

        // Use constant instead of magic number
        if (valueUsd < MIN_TRANSFER_VALUE_USD) return false;

        const auto userId = getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
        if (!userId) return false;

        gamificationService.recordEvent({
            userId,
            actionType: GamificationEventType.TRANSFER_COMPLETED,
            volumeUsd: valueUsd,
            metadata: { actionResult },
            sourceEventId: payload.messageId,
            });

            return true; // Return true to indicate we handled this action;
            } catch (error) {
                std::cerr << { error } << '[Gamification] Error recording transfer points' << std::endl;
                return false;
            }

}

double calculateChatPoints(double messageLength) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Validate input
    if (!Number.isFinite(messageLength) || messageLength < 0) {
        return 0;
    }

    if (messageLength < MIN_CHAT_LENGTH) return 0;

    for (const auto& tier : MESSAGE_LENGTH_TIERS)
        if (messageLength >= tier.minLength && messageLength <= tier.maxLength) {
            return tier.points;
        }
    }

    return 0;

}

std::future<void> recordChatPoints(RunEventPayload payload) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (payload.status != 'completed') return;

        // Get message text from the message itself
        auto input = '';
        try {
            if (payload.messageId) {
                // Try to get message directly if possible (more efficient)
                // For now, use smaller count to reduce overhead
                const auto memories = payload.runtime.getMemories({;
                    tableName: 'messages',
                    roomId: payload.roomId,
                    count: 50, // Reduced from 100
                    });
                    const auto message = memories.find((m) => m.id == payload.messageId);
                    input = message.content.text || '';
                }
                } catch (error) {
                    // If we can't get the message, skip
                    logger.debug({ error }, '[Gamification] Could not fetch message for chat points');
                    return;
                }

                const auto messageLength = input.length;
                const auto points = calculateChatPoints(messageLength);

                // Skip if message is too short or no points
                if (points == 0) return;

                // Check content quality to prevent spam/copy-pasta from earning points
                const auto qualityResult = checkContentQuality(input);
                if (!qualityResult.isValid) {
                    logger.debug(
                    {
                        reason: qualityResult.reason,
                        score: qualityResult.score,
                        messagePreview: input.substring(0, 50),
                        },
                        '[Gamification] Message failed content quality check, no points awarded';
                        );
                        return;
                    }

                    const auto gamificationService = payload.runtime.getService('gamification');
                    if (!gamificationService) return;

                    // Resolve actual user ID (handles agent-scoped entities)
                    const auto userId = getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
                    if (!userId) return;

                    // Store the calculated points in metadata to override BASE_POINTS
                    gamificationService.recordEvent({
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
                                std::cerr << { error } << '[Gamification] Error recording chat points' << std::endl;
                            }

}

std::future<void> recordAgentActionPoints(ActionEventPayload payload) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto gamificationService = payload.runtime.getService('gamification');
        if (!gamificationService) return;

        // Handle both actionResults (array) and actionResult (single) formats
        const auto actionResults = payload.content.actionResults;
        const auto actionResultSingle = payload.content.actionResult;
        const auto actionResult: ActionResultWithValues | std::nullopt =;
        Array.isArray(actionResults) && actionResults.length > 0;
        ? (actionResults[0]);
        : actionResultSingle
        ? (actionResultSingle);
        : std::nullopt;

        // Only award points for successful actions
        if (!actionResult || actionResult.success != true) {
            return;
        }

        const auto userId = getUserIdFromMessage(payload.runtime, payload.messageId, payload.roomId, payload.entityId);
        if (!userId) return;

        const auto actionName = payload.content.actions.[0] || 'unknown';

        gamificationService.recordEvent({
            userId,
            actionType: GamificationEventType.AGENT_ACTION,
            metadata: {
                actionName,
                actionResult,
                },
                sourceEventId: payload.messageId,
                });
                } catch (error) {
                    std::cerr << { error } << '[Gamification] Error recording agent action points' << std::endl;
                }

}

std::future<void> recordAccountCreationPoints(EntityPayload payload) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Resolve actual user ID (handles agent-scoped entities)
        const auto userId = getUserIdFromMessage(payload.runtime, std::nullopt, std::nullopt, payload.entityId);
        if (!userId) return;

        const auto gamificationService = payload.runtime.getService('gamification');
        if (gamificationService) {
            gamificationService.recordEvent({
                userId,
                actionType: GamificationEventType.ACCOUNT_CREATION,
                metadata: { source: payload.source },
                });
            }

            // Process referral if present
            const auto referralService = payload.runtime.getService('referral');
            if (referralService) {
                try {
                    // Use the actual user entity for referral processing
                    const auto entity = payload.runtime.getEntityById(userId);
                    const auto referredBy = entity.metadata.referredBy;

                    if (referredBy && typeof referredBy == 'string') {
                        std::cout << "[Gamification] Processing referral code " + std::to_string(referredBy) + " for user " + std::to_string(userId) << std::endl;
                        referralService.processReferralSignup(userId, referredBy);
                    }
                    } catch (err) {
                        std::cerr << { error = err } << '[Gamification] Error processing referral in account creation' << std::endl;
                    }
                }
                } catch (error) {
                    std::cerr << { error } << '[Gamification] Error recording account creation points' << std::endl;
                }

}

} // namespace elizaos
