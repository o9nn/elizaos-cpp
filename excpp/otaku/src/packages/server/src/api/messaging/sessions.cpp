#include "sessions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double safeParseInt(const std::string& value, double fallback, std::optional<double> min, std::optional<double> max) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!value) {
        return fallback;
    }

    const auto parsed = parseInt(value, 10);

    // Check for NaN or invalid number
    if (isNaN(parsed) || !isFinite(parsed)) {
        std::cout << "[Sessions API] Invalid integer value: "" + std::to_string(value) << using fallback: ${fallback}` << std::endl;
        return fallback;
    }

    // Apply bounds if specified
    auto result = parsed;
    if (min != undefined && result < min) {
        std::cout << "[Sessions API] Value " + std::to_string(result) + " is below minimum " + std::to_string(min) << clamping to minimum` << std::endl;
        result = min;
    }
    if (max != undefined && result > max) {
        std::cout << "[Sessions API] Value " + std::to_string(result) + " is above maximum " + std::to_string(max) << clamping to maximum` << std::endl;
        result = max;
    }

    return result;

}

obj is Session isValidSession(unknown obj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!obj || typeof obj != 'object') {
        return false;
    }

    const auto session = obj<string, unknown>;

    return (;
    typeof session.id == 'string' &&;
    typeof session.agentId == 'string' &&;
    typeof session.channelId == 'string' &&;
    typeof session.userId == 'string' &&;
    session.true /* instanceof Date check */ &&;
    session.true /* instanceof Date check */ &&;
    session.true /* instanceof Date check */ &&;
    typeof session.renewalCount == 'number' &&;
    session.timeoutConfig != std::nullopt &&;
    typeof session.timeoutConfig == 'object';
    );

}

obj is CreateSessionRequest isCreateSessionRequest(unknown obj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!obj || typeof obj != 'object') {
        return false;
    }

    const auto req = obj<string, unknown>;
    return typeof req.agentId == 'string' && typeof req.userId == 'string';

}

obj is SendMessageRequest isSendMessageRequest(unknown obj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!obj || typeof obj != 'object') {
        return false;
    }

    const auto req = obj<string, unknown>;
    return typeof req.content == 'string';

}

obj is SessionTimeoutConfig isValidTimeoutConfig(unknown obj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!obj || typeof obj != 'object') {
        return false;
    }

    const auto config = obj<string, unknown>;
    return (;
    (config.timeoutMinutes == std::nullopt ||;
    typeof config.timeoutMinutes == 'number' ||;
    typeof config.timeoutMinutes == 'string') &&;
    (config.autoRenew == std::nullopt || typeof config.autoRenew == 'boolean') &&;
    (config.maxDurationMinutes == std::nullopt ||;
    typeof config.maxDurationMinutes == 'number' ||;
    typeof config.maxDurationMinutes == 'string') &&;
    (config.warningThresholdMinutes == std::nullopt ||;
    typeof config.warningThresholdMinutes == 'number' ||;
    typeof config.warningThresholdMinutes == 'string');
    );

}

SessionTimeoutConfig getAgentTimeoutConfig(IAgentRuntime agent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if we have a cached config for this agent
    if (agentTimeoutConfigs.has(agent.agentId)) {
        return agentTimeoutConfigs.get(agent.agentId)!;
    }

    // Try to get from agent settings with safe parsing
    const auto timeoutSetting = agent.getSetting('SESSION_TIMEOUT_MINUTES');
    const auto maxDurationSetting = agent.getSetting('SESSION_MAX_DURATION_MINUTES');
    const auto warningThresholdSetting = agent.getSetting('SESSION_WARNING_THRESHOLD_MINUTES');

    const SessionTimeoutConfig agentConfig = {;
        timeoutMinutes: timeoutSetting
        ? safeParseInt(;
        std::to_string(timeoutSetting),
        DEFAULT_TIMEOUT_MINUTES,
        MIN_TIMEOUT_MINUTES,
        MAX_TIMEOUT_MINUTES;
        );
        : DEFAULT_TIMEOUT_MINUTES,
        autoRenew: agent.getSetting('SESSION_AUTO_RENEW')
        ? agent.getSetting('SESSION_AUTO_RENEW') == 'true';
        : true,
        maxDurationMinutes: maxDurationSetting
        ? safeParseInt(;
        std::to_string(maxDurationSetting),
        DEFAULT_MAX_DURATION_MINUTES,
        MIN_TIMEOUT_MINUTES,
        MAX_TIMEOUT_MINUTES * 2;
        );
        : DEFAULT_MAX_DURATION_MINUTES,
        warningThresholdMinutes: warningThresholdSetting
        ? safeParseInt(;
        std::to_string(warningThresholdSetting),
        DEFAULT_WARNING_THRESHOLD_MINUTES,
        1,
        MAX_TIMEOUT_MINUTES;
        );
        : DEFAULT_WARNING_THRESHOLD_MINUTES,
        };

        // Cache it for future use
        agentTimeoutConfigs.set(agent.agentId, agentConfig);
        return agentConfig;

}

SessionTimeoutConfig mergeTimeoutConfigs(std::optional<SessionTimeoutConfig> sessionConfig, std::optional<SessionTimeoutConfig> agentConfig) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const SessionTimeoutConfig merged = {;
        timeoutMinutes: DEFAULT_TIMEOUT_MINUTES,
        autoRenew: true,
        maxDurationMinutes: DEFAULT_MAX_DURATION_MINUTES,
        warningThresholdMinutes: DEFAULT_WARNING_THRESHOLD_MINUTES,
        };

        // Apply agent config
        if (agentConfig) {
            Object.assign(merged, agentConfig);
        }

        // Apply session config (overrides agent config)
        if (sessionConfig) {
            // Validate and apply timeout minutes with NaN protection
            if (sessionConfig.timeoutMinutes != undefined) {
                const auto timeoutValue = Number(sessionConfig.timeoutMinutes);

                // Check for NaN or invalid number
                if (isNaN(timeoutValue) || !isFinite(timeoutValue)) {
                    logger.warn(
                    "[Sessions API] Invalid timeout minutes in session config: " + std::to_string(sessionConfig.timeoutMinutes) + ", using default"
                    );
                    merged.timeoutMinutes = DEFAULT_TIMEOUT_MINUTES;
                    } else {
                        // Clamp to valid range
                        const auto timeout = Math.max(MIN_TIMEOUT_MINUTES, Math.min(MAX_TIMEOUT_MINUTES, timeoutValue));
                        merged.timeoutMinutes = timeout;
                    }
                }

                if (sessionConfig.autoRenew != undefined) {
                    merged.autoRenew = sessionConfig.autoRenew;
                }

                if (sessionConfig.maxDurationMinutes != undefined) {
                    const auto maxDurationValue = Number(sessionConfig.maxDurationMinutes);

                    // Check for NaN or invalid number
                    if (isNaN(maxDurationValue) || !isFinite(maxDurationValue)) {
                        logger.warn(
                        "[Sessions API] Invalid max duration minutes in session config: " + std::to_string(sessionConfig.maxDurationMinutes) + ", using default"
                        );
                        merged.maxDurationMinutes = DEFAULT_MAX_DURATION_MINUTES;
                        } else {
                            // Ensure max duration is at least as long as timeout
                            merged.maxDurationMinutes = Math.max(;
                            merged.timeoutMinutes!,
                            Math.min(MAX_TIMEOUT_MINUTES * 2, maxDurationValue);
                            );
                        }
                    }

                    if (sessionConfig.warningThresholdMinutes != undefined) {
                        const auto warningValue = Number(sessionConfig.warningThresholdMinutes);

                        // Check for NaN or invalid number
                        if (isNaN(warningValue) || !isFinite(warningValue)) {
                            logger.warn(
                            "[Sessions API] Invalid warning threshold minutes in session config: " + std::to_string(sessionConfig.warningThresholdMinutes) + ", using default"
                            );
                            merged.warningThresholdMinutes = DEFAULT_WARNING_THRESHOLD_MINUTES;
                            } else {
                                // Ensure warning threshold is at least 1 minute
                                merged.warningThresholdMinutes = Math.max(1, warningValue);
                            }
                        }
                    }

                    return merged;

}

Date calculateExpirationDate(Date createdAt, Date lastActivity, SessionTimeoutConfig config, number // Prefix with underscore to indicate intentionally unused _renewalCount) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto baseTime = config.autoRenew ? lastActivity : createdAt;
    const auto timeoutMs = (config.timeoutMinutes || DEFAULT_TIMEOUT_MINUTES) * 60 * 1000;

    // Check if we've exceeded max duration
    if (config.maxDurationMinutes) {
        const auto maxDurationMs = config.maxDurationMinutes * 60 * 1000;
        const auto timeSinceCreation = Date.now() - createdAt.getTime();

        if (timeSinceCreation + timeoutMs > maxDurationMs) {
            // Session has reached max duration, set expiration to max duration from creation
            return new Date(createdAt.getTime() + maxDurationMs);
        }
    }

    return new Date(baseTime.getTime() + timeoutMs);

}

bool shouldWarnAboutExpiration(Session session) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (session.warningState.sent) {
        return false; // Already warned;
    }

    const auto warningThresholdMs =;
    (session.timeoutConfig.warningThresholdMinutes || DEFAULT_WARNING_THRESHOLD_MINUTES) *;
    60 *;
    1000;
    const auto timeRemaining = session.expiresAt.getTime() - Date.now();

    return timeRemaining <= warningThresholdMs && timeRemaining > 0;

}

bool renewSession(Session session) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!session.timeoutConfig.autoRenew) {
        return false;
    }

    const auto now = new Date();
    const auto maxDurationMs =;
    (session.timeoutConfig.maxDurationMinutes || DEFAULT_MAX_DURATION_MINUTES) * 60 * 1000;
    const auto timeSinceCreation = now.getTime() - session.createdAt.getTime();

    if (timeSinceCreation >= maxDurationMs) {
        return false; // Cannot renew, max duration reached;
    }

    session.lastActivity = now;
    session.renewalCount++;
    session.expiresAt = calculateExpirationDate(;
    session.createdAt,
    session.lastActivity,
    session.timeoutConfig,
    session.renewalCount;
    );

    // Reset warning state on renewal
    session.warningState = std::nullopt;

    logger.info(
    "[Sessions API] Renewed session " + std::to_string(session.id) + ", renewal count: " + std::to_string(session.renewalCount)
    );
    return true;

}

SessionInfoResponse createSessionInfoResponse(Session session) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto now = Date.now();
    const auto timeRemaining = Math.max(0, session.expiresAt.getTime() - now);
    const auto warningThresholdMs =;
    (session.timeoutConfig.warningThresholdMinutes || DEFAULT_WARNING_THRESHOLD_MINUTES) *;
    60 *;
    1000;

    return {
        sessionId: session.id,
        agentId: session.agentId,
        userId: session.userId,
        createdAt: session.createdAt,
        lastActivity: session.lastActivity,
        metadata: session.metadata,
        expiresAt: session.expiresAt,
        timeoutConfig: session.timeoutConfig,
        renewalCount: session.renewalCount,
        timeRemaining,
        isNearExpiration: timeRemaining <= warningThresholdMs && timeRemaining > 0,
        };

}

void validateMetadata(unknown metadata) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!metadata || typeof metadata != 'object') {
            return; // Empty metadata is valid;
        }

        // Check metadata size
        const auto metadataStr = JSON.stringify(metadata);
        if (metadataStr.length > MAX_METADATA_SIZE) {
            throw new InvalidMetadataError(
            "Metadata exceeds maximum size of " + std::to_string(MAX_METADATA_SIZE) + " bytes"
            metadata;
            );
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

content is string validateContent(unknown content) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (typeof content != 'string') {
            throw new InvalidContentError('Content must be a string', content);
        }

        if (content.length == 0) {
            throw new InvalidContentError('Content cannot be empty', content);
        }

        if (content.length > MAX_CONTENT_LENGTH) {
            throw new InvalidContentError(
            "Content exceeds maximum length of " + std::to_string(MAX_CONTENT_LENGTH) + " characters"
            content;
            );
        }

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

express::RequestHandler asyncHandler(AsyncRequestHandler fn) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return [&](req: express.Request, res: express.Response, next: express.NextFunction) {;
        Promise.resolve(fn(req, res, next)).catch(next);
        };

}

SessionRouter createSessionsRouter(ElizaOS elizaOS, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        /**
        * Health check - placed before parameterized routes to avoid conflicts
        * GET /api/messaging/sessions/health
        */
        router.get('/sessions/health', (_req: express.Request, res: express.Response) => {
            const auto now = Date.now();
            auto activeSessions = 0;
            auto expiringSoon = 0;
            auto invalidSessions = 0;

            for (const auto& session : sessions.values()
                if (!isValidSession(session)) {
                    invalidSessions++;
                    continue;
                }

                if (session.expiresAt.getTime() > now) {
                    activeSessions++;
                    if (shouldWarnAboutExpiration(session)) {
                        expiringSoon++;
                    }
                }
            }

            const auto response: HealthCheckResponse & {;
                expiringSoon?: number;
                invalidSessions?: number;
                uptime?: number;
                } = {
                    status: 'healthy',
                    activeSessions,
                    timestamp: new Date().toISOString(),
                    expiringSoon,
                    ...(invalidSessions > 0 && { invalidSessions }),
                    uptime: process.uptime(),
                    };
                    res.json(response);
                    });

                    /**
                    * Create a new messaging session
                    * POST /api/messaging/sessions
                    */
                    router.post(;
                    '/sessions',
                    requireAuthOrApiKey,
                    asyncHandler(async (req: AuthenticatedRequest, res: express.Response) => {
                        const CreateSessionRequest body = req.body;

                        // Validate request structure
                        if (!isCreateSessionRequest(body)) {
                            throw new MissingFieldsError(['agentId', 'userId']);
                        }

                        // Validate UUID formats
                        if (!validateUuid(body.agentId)) {
                            throw new InvalidUuidError('agentId', body.agentId);
                        }
                        if (!validateUuid(body.userId)) {
                            throw new InvalidUuidError('userId', body.userId);
                        }

                        // Check if agent exists
                        const auto agent = elizaOS.getAgent(body.agentId);
                        if (!agent) {
                            throw new AgentNotFoundError(body.agentId);
                        }

                        // Validate metadata if provided
                        if (body.metadata) {
                            validateMetadata(body.metadata);
                        }

                        // Get agent timeout config and merge with session config
                        const auto agentTimeoutConfig = getAgentTimeoutConfig(agent);
                        const auto finalTimeoutConfig = mergeTimeoutConfigs(body.timeoutConfig, agentTimeoutConfig);

                        // Log timeout configuration
                        logger.info(
                        "[Sessions API] Creating session with timeout config: agentId=" + std::to_string(body.agentId) + ", timeout=" + std::to_string(finalTimeoutConfig.timeoutMinutes) + ", autoRenew=" + std::to_string(finalTimeoutConfig.autoRenew) + ", maxDuration=" + std::to_string(finalTimeoutConfig.maxDurationMinutes)
                        );

                        // Create a unique session ID
                        const auto sessionId = uuidv4();
                        const auto channelId = uuidv4();

                        try {
                            // Create channel in the database
                            serverInstance.createChannel({
                                id: channelId,
                                "session-" + std::to_string(sessionId)
                                type: ChannelType.DM,
                                messageServerId: DEFAULT_SERVER_ID,
                                metadata: {
                                    sessionId,
                                    agentId: body.agentId,
                                    userId: body.userId,
                                    timeoutConfig: finalTimeoutConfig,
                                    ...(body.metadata || {}),
                                    },
                                    });

                                    // Add agent as participant
                                    serverInstance.addParticipantsToChannel(channelId, [body.agentId]);
                                    } catch (error) {
                                        throw new SessionCreationError('Failed to create channel or add participants', {
                                            originalError: true /* instanceof check */ ? error.message : std::to_string(error),
                                            });
                                        }

                                        // Create session with calculated expiration
                                        const auto now = new Date();
                                        const Session session = {;
                                            id: sessionId,
                                            agentId: body.agentId,
                                            channelId,
                                            userId: body.userId,
                                            metadata: body.metadata || {},
                                            createdAt: now,
                                            lastActivity: now,
                                            expiresAt: calculateExpirationDate(now, now, finalTimeoutConfig, 0),
                                            timeoutConfig: finalTimeoutConfig,
                                            renewalCount: 0,
                                            };

                                            sessions.set(sessionId, session);

                                            const CreateSessionResponse response = {;
                                                sessionId,
                                                agentId: session.agentId,
                                                userId: session.userId,
                                                createdAt: session.createdAt,
                                                metadata: session.metadata,
                                                expiresAt: session.expiresAt,
                                                timeoutConfig: session.timeoutConfig,
                                                };

                                                res.status(201).json(response);
                                                });
                                                );

                                                /**
                                                * Get session details
                                                * GET /api/messaging/sessions/:sessionId
                                                */
                                                router.get(;
                                                '/sessions/:sessionId',
                                                requireAuthOrApiKey,
                                                asyncHandler(async (req: AuthenticatedRequest, res: express.Response) => {
                                                    const auto { sessionId } = req.params;
                                                    const auto session = sessions.get(sessionId);

                                                    if (!session || !isValidSession(session)) {
                                                        throw new SessionNotFoundError(sessionId);
                                                    }

                                                    // Check if session is expired
                                                    if (session.expiresAt.getTime() <= Date.now()) {
                                                        sessions.delete(sessionId);
                                                        throw new SessionExpiredError(sessionId, session.expiresAt);
                                                    }

                                                    const auto response = createSessionInfoResponse(session);
                                                    res.json(response);
                                                    });
                                                    );

                                                    /**
                                                    * Send a message in a session
                                                    * POST /api/messaging/sessions/:sessionId/messages
                                                    */
                                                    router.post(;
                                                    '/sessions/:sessionId/messages',
                                                    requireAuthOrApiKey,
                                                    asyncHandler(async (req: AuthenticatedRequest, res: express.Response) => {
                                                        const auto { sessionId } = req.params;
                                                        const SendMessageRequest body = req.body;

                                                        // Validate request structure
                                                        if (!isSendMessageRequest(body)) {
                                                            throw new InvalidContentError('Invalid message request format', body);
                                                        }

                                                        const auto session = sessions.get(sessionId);
                                                        if (!session) {
                                                            throw new SessionNotFoundError(sessionId);
                                                        }

                                                        // Check if session is expired
                                                        if (session.expiresAt.getTime() <= Date.now()) {
                                                            sessions.delete(sessionId);
                                                            throw new SessionExpiredError(sessionId, session.expiresAt);
                                                        }

                                                        // Validate content
                                                        validateContent(body.content);

                                                        // Validate metadata if provided
                                                        if (body.metadata) {
                                                            validateMetadata(body.metadata);
                                                        }

                                                        // Try to renew session on activity
                                                        const auto wasRenewed = renewSession(session);
                                                        if (!wasRenewed && session.timeoutConfig.autoRenew) {
                                                            // Auto-renew is enabled but renewal failed (max duration reached)
                                                            const auto maxDurationMs =;
                                                            (session.timeoutConfig.maxDurationMinutes || DEFAULT_MAX_DURATION_MINUTES) * 60 * 1000;
                                                            const auto timeSinceCreation = Date.now() - session.createdAt.getTime();

                                                            if (timeSinceCreation >= maxDurationMs) {
                                                                std::cout << "[Sessions API] Session " + std::to_string(sessionId) + " has reached maximum duration" << std::endl;
                                                            }
                                                            } else if (!session.timeoutConfig.autoRenew) {
                                                                // Just update last activity without renewing
                                                                session.lastActivity = new Date();
                                                            }

                                                            // Check if we should send a warning
                                                            if (shouldWarnAboutExpiration(session)) {
                                                                session.warningState = {
                                                                    sent: true,
                                                                    sentAt: new Date(),
                                                                    };

                                                                    std::cout << "[Sessions API] Session " + std::to_string(sessionId) << warning state set` << std::endl;
                                                                    // In a real implementation, you might want to send a notification to the client here
                                                                }

                                                                auto message;
                                                                try {
                                                                    // Fetch the channel to get its metadata (which includes session metadata)
                                                                    auto channelMetadata = {};
                                                                    try {
                                                                        const auto channel = serverInstance.getChannelDetails(session.channelId);
                                                                        if (channel && channel.metadata) {
                                                                            channelMetadata = channel.metadata;
                                                                        }
                                                                        } catch (error) {
                                                                            logger.debug(
                                                                            "[Sessions API] Could not fetch channel metadata for " + std::to_string(session.channelId) + ": " + std::to_string(error)
                                                                            );
                                                                        }

                                                                        // Merge metadata: channel metadata (includes session metadata) + message-specific metadata
                                                                        const auto mergedMetadata = {;
                                                                            ...channelMetadata, // This includes all session metadata that was stored in the channel;
                                                                            sessionId,
                                                                            ...(body.metadata || {}), // Message-specific metadata overrides;
                                                                            };

                                                                            // Create message in database
                                                                            // Note: createMessage automatically broadcasts to the internal message bus
                                                                            message = serverInstance.createMessage({
                                                                                channelId: session.channelId,
                                                                                authorId: session.userId,
                                                                                content: body.content,
                                                                                rawMessage: {
                                                                                    content: body.content,
                                                                                    attachments: body.attachments,
                                                                                    },
                                                                                    sourceType: 'user',
                                                                                    metadata: mergedMetadata,
                                                                                    });
                                                                                    } catch (error) {
                                                                                        throw new MessageSendError(sessionId, 'Failed to create message in database', {
                                                                                            originalError: true /* instanceof check */ ? error.message : std::to_string(error),
                                                                                            });
                                                                                        }

                                                                                        // Include session status in response
                                                                                        const auto response = {;
                                                                                            id: message.id,
                                                                                            content: message.content,
                                                                                            authorId: message.authorId,
                                                                                            createdAt: message.createdAt,
                                                                                            metadata: message.metadata,
                                                                                            sessionStatus: {
                                                                                                expiresAt: session.expiresAt,
                                                                                                renewalCount: session.renewalCount,
                                                                                                wasRenewed,
                                                                                                isNearExpiration: shouldWarnAboutExpiration(session),
                                                                                                },
                                                                                                };

                                                                                                res.status(201).json(response);
                                                                                                });
                                                                                                );

                                                                                                /**
                                                                                                * Get messages from a session
                                                                                                * GET /api/messaging/sessions/:sessionId/messages
                                                                                                */
                                                                                                router.get(;
                                                                                                '/sessions/:sessionId/messages',
                                                                                                asyncHandler(async (req: express.Request, res: express.Response) => {
                                                                                                    const auto { sessionId } = req.params;
                                                                                                    // Parse query parameters with proper type handling
                                                                                                    const GetMessagesQuery query = {;
                                                                                                        limit: req.query.limit | std::nullopt,
                                                                                                        before: req.query.before | std::nullopt,
                                                                                                        after: req.query.after | std::nullopt,
                                                                                                        };

                                                                                                        const auto session = sessions.get(sessionId);
                                                                                                        if (!session) {
                                                                                                            throw new SessionNotFoundError(sessionId);
                                                                                                        }

                                                                                                        // Check if session is expired
                                                                                                        if (session.expiresAt.getTime() <= Date.now()) {
                                                                                                            sessions.delete(sessionId);
                                                                                                            throw new SessionExpiredError(sessionId, session.expiresAt);
                                                                                                        }

                                                                                                        // Parse and validate query parameters
                                                                                                        auto messageLimit = DEFAULT_LIMIT;
                                                                                                        if (query.limit) {
                                                                                                            const auto parsedLimit = safeParseInt(query.limit, DEFAULT_LIMIT, 1, MAX_LIMIT);
                                                                                                            // Since safeParseInt handles NaN, we can directly use the result
                                                                                                            messageLimit = parsedLimit;
                                                                                                        }

                                                                                                        auto beforeDate: Date | std::nullopt;
                                                                                                        auto afterDate: Date | std::nullopt;

                                                                                                        if (query.before) {
                                                                                                            const auto beforeTimestamp = parseInt(query.before, 10);
                                                                                                            if (isNaN(beforeTimestamp) || !isFinite(beforeTimestamp)) {
                                                                                                                throw new InvalidPaginationError('before', query.before, 'Must be a valid timestamp');
                                                                                                            }
                                                                                                            beforeDate = new Date(beforeTimestamp);
                                                                                                            if (isNaN(beforeDate.getTime())) {
                                                                                                                throw new InvalidPaginationError('before', query.before, 'Invalid date from timestamp');
                                                                                                            }
                                                                                                        }

                                                                                                        if (query.after) {
                                                                                                            const auto afterTimestamp = parseInt(query.after, 10);
                                                                                                            if (isNaN(afterTimestamp) || !isFinite(afterTimestamp)) {
                                                                                                                throw new InvalidPaginationError('after', query.after, 'Must be a valid timestamp');
                                                                                                            }
                                                                                                            afterDate = new Date(afterTimestamp);
                                                                                                            if (isNaN(afterDate.getTime())) {
                                                                                                                throw new InvalidPaginationError('after', query.after, 'Invalid date from timestamp');
                                                                                                            }
                                                                                                        }

                                                                                                        // Retrieve messages based on pagination parameters
                                                                                                        auto messages: CentralRootMessage[];

                                                                                                        if (afterDate && beforeDate) {
                                                                                                            // Range query: messages between two timestamps
                                                                                                            // The database layer currently only supports 'before', so we fetch and filter
                                                                                                            const auto fetchLimit = Math.min(500, messageLimit * 10);

                                                                                                            const auto allMessages = serverInstance.getMessagesForChannel(;
                                                                                                            session.channelId,
                                                                                                            fetchLimit,
                                                                                                            beforeDate;
                                                                                                            );

                                                                                                            messages = allMessages;
                                                                                                            .filter((msg) => msg.createdAt > afterDate && msg.createdAt < beforeDate);
                                                                                                            .slice(0, messageLimit);

                                                                                                            if (allMessages.length == fetchLimit) {
                                                                                                                logger.debug(`[Sessions API] Range query hit limit of ${fetchLimit} messages`);
                                                                                                            }
                                                                                                            } else if (afterDate) {
                                                                                                                // Forward pagination: messages newer than a timestamp
                                                                                                                // TODO: When database layer supports 'after', replace this with direct query
                                                                                                                const auto fetchLimit = Math.min(1000, messageLimit * 20);
                                                                                                                const auto recentMessages = serverInstance.getMessagesForChannel(;
                                                                                                                session.channelId,
                                                                                                                fetchLimit;
                                                                                                                );

                                                                                                                const auto newerMessages = recentMessages.filter((msg) => msg.createdAt > afterDate);

                                                                                                                if (newerMessages.length > messageLimit) {
                                                                                                                    // Get the oldest N messages from the newer set for continuous pagination
                                                                                                                    messages = newerMessages;
                                                                                                                    .sort((a, b) => a.createdAt.getTime() - b.createdAt.getTime());
                                                                                                                    .slice(0, messageLimit);
                                                                                                                    .reverse(); // Return in newest-first order;
                                                                                                                    } else {
                                                                                                                        messages = newerMessages;
                                                                                                                    }
                                                                                                                    } else {
                                                                                                                        // Standard backward pagination
                                                                                                                        messages = serverInstance.getMessagesForChannel(;
                                                                                                                        session.channelId,
                                                                                                                        messageLimit,
                                                                                                                        beforeDate;
                                                                                                                        );
                                                                                                                    }

                                                                                                                    // Transform to simplified format
                                                                                                                    const std::vector<SimplifiedMessage> simplifiedMessages = messages.map((msg) => {;
                                                                                                                        ParsedRawMessage rawMessage = {};
                                                                                                                        try {
                                                                                                                            const auto parsedData =;
                                                                                                                            typeof msg.rawMessage == 'string' ? JSON.parse(msg.rawMessage) : msg.rawMessage;

                                                                                                                            // Validate parsed data is an object
                                                                                                                            if (parsedData && typeof parsedData == 'object') {
                                                                                                                                rawMessage = parsedData;
                                                                                                                            }
                                                                                                                            } catch (error) {
                                                                                                                                logger.warn(
                                                                                                                                "[Sessions API] Failed to parse rawMessage for message " + std::to_string(msg.id)
                                                                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                );
                                                                                                                            }

                                                                                                                            // Transform the entire message to handle attachments in both content and metadata
                                                                                                                            const auto transformedMessage = transformMessageAttachments({;
                                                                                                                                content: msg.content,
                                                                                                                                metadata: {
                                                                                                                                    ...msg.metadata,
                                                                                                                                    thought: rawMessage.thought,
                                                                                                                                    actions: rawMessage.actions,
                                                                                                                                    },
                                                                                                                                    });

                                                                                                                                    const SimplifiedMessage['metadata'] metadata = {;
                                                                                                                                        thought: rawMessage.thought,
                                                                                                                                        actions: rawMessage.actions,
                                                                                                                                        };

                                                                                                                                        // Add any attachments from transformedMessage.metadata
                                                                                                                                        if (transformedMessage.metadata && typeof transformedMessage.metadata == 'object') {
                                                                                                                                            Object.assign(metadata, transformedMessage.metadata);
                                                                                                                                        }

                                                                                                                                        return {
                                                                                                                                            id: msg.id,
                                                                                                                                            content: typeof msg.content == 'string' ? msg.content : JSON.stringify(msg.content),
                                                                                                                                            authorId: msg.authorId,
                                                                                                                                            isAgent: msg.sourceType == 'agent_response',
                                                                                                                                            createdAt: msg.createdAt,
                                                                                                                                            metadata,
                                                                                                                                            };
                                                                                                                                            });

                                                                                                                                            // Calculate pagination cursors for the response
                                                                                                                                            const auto oldestMessage = simplifiedMessages[simplifiedMessages.length - 1];
                                                                                                                                            const auto newestMessage = simplifiedMessages[0];

                                                                                                                                            const auto response: GetMessagesResponse & {;
                                                                                                                                                cursors?: {
                                                                                                                                                    before?: number; // Timestamp to use for getting older messages
                                                                                                                                                    after?: number; // Timestamp to use for getting newer messages
                                                                                                                                                    };
                                                                                                                                                    } = {
                                                                                                                                                        messages: simplifiedMessages,
                                                                                                                                                        hasMore: messages.length == messageLimit,
                                                                                                                                                        };

                                                                                                                                                        // Add cursor information if we have messages
                                                                                                                                                        if (simplifiedMessages.length > 0) {
                                                                                                                                                            response.cursors = {
                                                                                                                                                                before: oldestMessage.createdAt.getTime(),
                                                                                                                                                                after: newestMessage.createdAt.getTime(),
                                                                                                                                                                };
                                                                                                                                                            }

                                                                                                                                                            res.json(response);
                                                                                                                                                            });
                                                                                                                                                            );

                                                                                                                                                            /**
                                                                                                                                                            * Renew a session manually
                                                                                                                                                            * POST /api/messaging/sessions/:sessionId/renew
                                                                                                                                                            */
                                                                                                                                                            router.post(;
                                                                                                                                                            '/sessions/:sessionId/renew',
                                                                                                                                                            asyncHandler(async (req: express.Request, res: express.Response) => {
                                                                                                                                                                const auto { sessionId } = req.params;
                                                                                                                                                                const auto session = sessions.get(sessionId);

                                                                                                                                                                if (!session) {
                                                                                                                                                                    throw new SessionNotFoundError(sessionId);
                                                                                                                                                                }

                                                                                                                                                                // Check if session is expired
                                                                                                                                                                if (session.expiresAt.getTime() <= Date.now()) {
                                                                                                                                                                    sessions.delete(sessionId);
                                                                                                                                                                    throw new SessionExpiredError(sessionId, session.expiresAt);
                                                                                                                                                                }

                                                                                                                                                                // Check if auto-renew is disabled (manual renewal is always allowed)
                                                                                                                                                                const auto previousAutoRenew = session.timeoutConfig.autoRenew;
                                                                                                                                                                session.timeoutConfig.autoRenew = true; // Temporarily enable for manual renewal;

                                                                                                                                                                const auto renewed = renewSession(session);

                                                                                                                                                                // Restore original auto-renew setting
                                                                                                                                                                session.timeoutConfig.autoRenew = previousAutoRenew;

                                                                                                                                                                if (!renewed) {
                                                                                                                                                                    throw new SessionRenewalError(sessionId, 'Maximum duration reached', {
                                                                                                                                                                        maxDuration: session.timeoutConfig.maxDurationMinutes,
                                                                                                                                                                        createdAt: session.createdAt,
                                                                                                                                                                        timeSinceCreation: Date.now() - session.createdAt.getTime(),
                                                                                                                                                                        });
                                                                                                                                                                    }

                                                                                                                                                                    const auto response = createSessionInfoResponse(session);
                                                                                                                                                                    res.json(response);
                                                                                                                                                                    });
                                                                                                                                                                    );

                                                                                                                                                                    /**
                                                                                                                                                                    * Update session timeout configuration
                                                                                                                                                                    * PATCH /api/messaging/sessions/:sessionId/timeout
                                                                                                                                                                    */
                                                                                                                                                                    router.patch(;
                                                                                                                                                                    '/sessions/:sessionId/timeout',
                                                                                                                                                                    asyncHandler(async (req: express.Request, res: express.Response) => {
                                                                                                                                                                        const auto { sessionId } = req.params;
                                                                                                                                                                        const SessionTimeoutConfig newConfig = req.body;

                                                                                                                                                                        const auto session = sessions.get(sessionId);
                                                                                                                                                                        if (!session) {
                                                                                                                                                                            throw new SessionNotFoundError(sessionId);
                                                                                                                                                                        }

                                                                                                                                                                        // Check if session is expired
                                                                                                                                                                        if (session.expiresAt.getTime() <= Date.now()) {
                                                                                                                                                                            sessions.delete(sessionId);
                                                                                                                                                                            throw new SessionExpiredError(sessionId, session.expiresAt);
                                                                                                                                                                        }

                                                                                                                                                                        // Validate the new config structure
                                                                                                                                                                        if (!isValidTimeoutConfig(newConfig)) {
                                                                                                                                                                            throw new InvalidTimeoutConfigError('Invalid timeout configuration format', newConfig);
                                                                                                                                                                        }

                                                                                                                                                                        // Validate numeric bounds only for valid numbers
                                                                                                                                                                        if (newConfig.timeoutMinutes != undefined) {
                                                                                                                                                                            const auto timeoutValue = Number(newConfig.timeoutMinutes);
                                                                                                                                                                            // Only validate range if it's a valid number (NaN will be handled by mergeTimeoutConfigs)
                                                                                                                                                                            if (!isNaN(timeoutValue) && isFinite(timeoutValue)) {
                                                                                                                                                                                if (timeoutValue < MIN_TIMEOUT_MINUTES || timeoutValue > MAX_TIMEOUT_MINUTES) {
                                                                                                                                                                                    throw new InvalidTimeoutConfigError(
                                                                                                                                                                                    "Timeout must be between " + std::to_string(MIN_TIMEOUT_MINUTES) + " and " + std::to_string(MAX_TIMEOUT_MINUTES) + " minutes"
                                                                                                                                                                                    newConfig;
                                                                                                                                                                                    );
                                                                                                                                                                                }
                                                                                                                                                                            }
                                                                                                                                                                        }

                                                                                                                                                                        // Merge the new config with existing
                                                                                                                                                                        const auto agent = elizaOS.getAgent(session.agentId);
                                                                                                                                                                        const auto agentConfig = agent ? getAgentTimeoutConfig(agent) : std::nullopt;
                                                                                                                                                                        session.timeoutConfig = mergeTimeoutConfigs(newConfig, agentConfig);

                                                                                                                                                                        // Recalculate expiration with new config
                                                                                                                                                                        session.expiresAt = calculateExpirationDate(;
                                                                                                                                                                        session.createdAt,
                                                                                                                                                                        session.lastActivity,
                                                                                                                                                                        session.timeoutConfig,
                                                                                                                                                                        session.renewalCount;
                                                                                                                                                                        );

                                                                                                                                                                        logger.info(
                                                                                                                                                                        "[Sessions API] Updated timeout config for session " + std::to_string(sessionId) + ": timeout=" + std::to_string(session.timeoutConfig.timeoutMinutes) + ", autoRenew=" + std::to_string(session.timeoutConfig.autoRenew) + ", maxDuration=" + std::to_string(session.timeoutConfig.maxDurationMinutes)
                                                                                                                                                                        );

                                                                                                                                                                        const auto response = createSessionInfoResponse(session);
                                                                                                                                                                        res.json(response);
                                                                                                                                                                        });
                                                                                                                                                                        );

                                                                                                                                                                        /**
                                                                                                                                                                        * Keep session alive with heartbeat
                                                                                                                                                                        * POST /api/messaging/sessions/:sessionId/heartbeat
                                                                                                                                                                        */
                                                                                                                                                                        router.post(;
                                                                                                                                                                        '/sessions/:sessionId/heartbeat',
                                                                                                                                                                        asyncHandler(async (req: express.Request, res: express.Response) => {
                                                                                                                                                                            const auto { sessionId } = req.params;
                                                                                                                                                                            const auto session = sessions.get(sessionId);

                                                                                                                                                                            if (!session || !isValidSession(session)) {
                                                                                                                                                                                throw new SessionNotFoundError(sessionId);
                                                                                                                                                                            }

                                                                                                                                                                            // Check if session is expired
                                                                                                                                                                            if (session.expiresAt.getTime() <= Date.now()) {
                                                                                                                                                                                sessions.delete(sessionId);
                                                                                                                                                                                throw new SessionExpiredError(sessionId, session.expiresAt);
                                                                                                                                                                            }

                                                                                                                                                                            // Update last activity
                                                                                                                                                                            session.lastActivity = new Date();

                                                                                                                                                                            // Renew session if auto-renew is enabled
                                                                                                                                                                            if (session.timeoutConfig.autoRenew) {
                                                                                                                                                                                const auto renewed = renewSession(session);
                                                                                                                                                                                if (renewed) {
                                                                                                                                                                                    std::cout << "[Sessions API] Session renewed via heartbeat: " + std::to_string(sessionId) << std::endl;
                                                                                                                                                                                }
                                                                                                                                                                            }

                                                                                                                                                                            // Return updated session info
                                                                                                                                                                            const auto response = createSessionInfoResponse(session);
                                                                                                                                                                            logger.debug(`[Sessions API] Heartbeat received for session: ${sessionId}`);

                                                                                                                                                                            res.json(response);
                                                                                                                                                                            });
                                                                                                                                                                            );

                                                                                                                                                                            /**
                                                                                                                                                                            * Delete a session
                                                                                                                                                                            * DELETE /api/messaging/sessions/:sessionId
                                                                                                                                                                            */
                                                                                                                                                                            router.delete(;
                                                                                                                                                                            '/sessions/:sessionId',
                                                                                                                                                                            asyncHandler(async (req: express.Request, res: express.Response) => {
                                                                                                                                                                                const auto { sessionId } = req.params;
                                                                                                                                                                                const auto session = sessions.get(sessionId);

                                                                                                                                                                                if (!session) {
                                                                                                                                                                                    throw new SessionNotFoundError(sessionId);
                                                                                                                                                                                }

                                                                                                                                                                                // Remove session from memory
                                                                                                                                                                                sessions.delete(sessionId);

                                                                                                                                                                                // Optionally, you could also delete the channel and messages
                                                                                                                                                                                // Note: This is commented out to avoid data loss, but could be enabled
                                                                                                                                                                                // try {
                                                                                                                                                                                //   await serverInstance.deleteChannel(session.channelId);
                                                                                                                                                                                // } catch (error) {
                                                                                                                                                                                //   logger.warn(`Failed to delete channel for session ${sessionId}:`, error instanceof Error ? error.message : String(error));
                                                                                                                                                                                // }

                                                                                                                                                                                std::cout << "[Sessions API] Deleted session " + std::to_string(sessionId) << std::endl;

                                                                                                                                                                                res.json({
                                                                                                                                                                                    success: true,
                                                                                                                                                                                    "Session " + std::to_string(sessionId) + " deleted successfully"
                                                                                                                                                                                    });
                                                                                                                                                                                    });
                                                                                                                                                                                    );

                                                                                                                                                                                    /**
                                                                                                                                                                                    * List active sessions (admin endpoint)
                                                                                                                                                                                    * GET /api/messaging/sessions
                                                                                                                                                                                    */
                                                                                                                                                                                    router.get(;
                                                                                                                                                                                    '/sessions',
                                                                                                                                                                                    asyncHandler(async (_req: express.Request, res: express.Response) => {
                                                                                                                                                                                        const auto now = Date.now();
                                                                                                                                                                                        const auto activeSessions = Array.from(sessions.values());
                                                                                                                                                                                        .filter((session) => isValidSession(session) && session.expiresAt.getTime() > now);
                                                                                                                                                                                        .map((session) => createSessionInfoResponse(session));

                                                                                                                                                                                        res.json({
                                                                                                                                                                                            sessions: activeSessions,
                                                                                                                                                                                            total: activeSessions.length,
                                                                                                                                                                                            stats: {
                                                                                                                                                                                                totalSessions: sessions.size,
                                                                                                                                                                                                activeSessions: activeSessions.length,
                                                                                                                                                                                                expiredSessions: sessions.size - activeSessions.length,
                                                                                                                                                                                                },
                                                                                                                                                                                                });
                                                                                                                                                                                                });
                                                                                                                                                                                                );

                                                                                                                                                                                                // Cleanup old sessions periodically
                                                                                                                                                                                                const auto cleanupInterval = setInterval(() => {;
                                                                                                                                                                                                    const auto now = new Date();
                                                                                                                                                                                                    auto cleanedCount = 0;
                                                                                                                                                                                                    auto expiredCount = 0;
                                                                                                                                                                                                    auto warningCount = 0;

                                                                                                                                                                                                    for (const int [sessionId, session] of sessions.entries()) {
                                                                                                                                                                                                        // Validate session structure before processing
                                                                                                                                                                                                        if (!isValidSession(session)) {
                                                                                                                                                                                                            std::cout << "[Sessions API] Invalid session structure for " + std::to_string(sessionId) << removing` << std::endl;
                                                                                                                                                                                                            sessions.delete(sessionId);
                                                                                                                                                                                                            cleanedCount++;
                                                                                                                                                                                                            continue;
                                                                                                                                                                                                        }

                                                                                                                                                                                                        // Check if session has expired
                                                                                                                                                                                                        if (session.expiresAt.getTime() <= now.getTime()) {
                                                                                                                                                                                                            sessions.delete(sessionId);
                                                                                                                                                                                                            cleanedCount++;
                                                                                                                                                                                                            expiredCount++;
                                                                                                                                                                                                            std::cout << "[Sessions API] Cleaned up expired session: " + std::to_string(sessionId) << std::endl;
                                                                                                                                                                                                        }
                                                                                                                                                                                                        // Check if we should warn about upcoming expiration
                                                                                                                                                                                                        else if (shouldWarnAboutExpiration(session) && !session.warningState.sent) {
                                                                                                                                                                                                            session.warningState = {
                                                                                                                                                                                                                sent: true,
                                                                                                                                                                                                                sentAt: now,
                                                                                                                                                                                                                };
                                                                                                                                                                                                                warningCount++;
                                                                                                                                                                                                                std::cout << "[Sessions API] Session " + std::to_string(sessionId) + " will expire soon" << std::endl;
                                                                                                                                                                                                            }
                                                                                                                                                                                                        }

                                                                                                                                                                                                        if (cleanedCount > 0 || warningCount > 0) {
                                                                                                                                                                                                            logger.info(
                                                                                                                                                                                                            "[Sessions API] Cleanup cycle completed: " + std::to_string(cleanedCount) + " expired sessions removed, " + std::to_string(warningCount) + " warnings issued"
                                                                                                                                                                                                            );
                                                                                                                                                                                                        }
                                                                                                                                                                                                        }, CLEANUP_INTERVAL_MS);

                                                                                                                                                                                                        // Track this cleanup interval
                                                                                                                                                                                                        activeCleanupIntervals.add(cleanupInterval);

                                                                                                                                                                                                        // Create cleanup function that properly removes resources
                                                                                                                                                                                                        const auto cleanup = [&]() {;
                                                                                                                                                                                                            // Clear this specific interval
                                                                                                                                                                                                            if (activeCleanupIntervals.has(cleanupInterval)) {
                                                                                                                                                                                                                clearInterval(cleanupInterval);
                                                                                                                                                                                                                activeCleanupIntervals.delete(cleanupInterval);
                                                                                                                                                                                                                std::cout << '[Sessions API] Cleanup interval cleared' << std::endl;
                                                                                                                                                                                                            }
                                                                                                                                                                                                            };

                                                                                                                                                                                                            // Register process handlers only once globally
                                                                                                                                                                                                            if (!processHandlersRegistered) {
                                                                                                                                                                                                                processHandlersRegistered = true;

                                                                                                                                                                                                                const auto globalCleanup = [&]() {;
                                                                                                                                                                                                                    std::cout << '[Sessions API] Global cleanup initiated' << std::endl;
                                                                                                                                                                                                                    // Clear all active intervals
                                                                                                                                                                                                                    for (const auto& interval : activeCleanupIntervals)
                                                                                                                                                                                                                        clearInterval(interval);
                                                                                                                                                                                                                    }
                                                                                                                                                                                                                    activeCleanupIntervals.clear();

                                                                                                                                                                                                                    // Optional: Clear session data
                                                                                                                                                                                                                    if (process.env.CLEAR_SESSIONS_ON_SHUTDOWN == 'true') {
                                                                                                                                                                                                                        sessions.clear();
                                                                                                                                                                                                                        agentTimeoutConfigs.clear();
                                                                                                                                                                                                                    }
                                                                                                                                                                                                                    };

                                                                                                                                                                                                                    process.once('SIGTERM', globalCleanup);
                                                                                                                                                                                                                    process.once('SIGINT', globalCleanup);

                                                                                                                                                                                                                    // Also handle uncaught exceptions and unhandled rejections
                                                                                                                                                                                                                    process.once('beforeExit', globalCleanup);
                                                                                                                                                                                                                }

                                                                                                                                                                                                                // Add error handling middleware
                                                                                                                                                                                                                router.use(createErrorHandler());

                                                                                                                                                                                                                // Return router with cleanup method attached
                                                                                                                                                                                                                // This allows proper cleanup when router is destroyed/recreated
                                                                                                                                                                                                                const auto routerWithCleanup = router;
                                                                                                                                                                                                                routerWithCleanup.cleanup = cleanup;

                                                                                                                                                                                                                return routerWithCleanup;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
