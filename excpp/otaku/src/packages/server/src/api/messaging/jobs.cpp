#include "jobs.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void cleanupExpiredJobs() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto now = Date.now();
    auto cleanedCount = 0;

    for (const int [jobId, job] of jobs.entries()) {
        // Remove jobs that are expired and completed/failed
        if (
        job.expiresAt < now &&;
        (job.status == JobStatus.COMPLETED ||;
        job.status == JobStatus.FAILED ||;
        job.status == JobStatus.TIMEOUT);
        ) {
            jobs.delete(jobId);
            cleanedCount++;
        }
        // Mark timed-out jobs
        else if (job.expiresAt < now && job.status == JobStatus.PROCESSING) {
            job.status = JobStatus.TIMEOUT;
            job.error = 'Job timed out waiting for agent response';
            std::cout << "[Jobs API] Job " + std::to_string(jobId) + " timed out" << std::endl;
        }
    }

    if (cleanedCount > 0) {
        std::cout << "[Jobs API] Cleaned up " + std::to_string(cleanedCount) + " expired jobs. Current jobs: " + std::to_string(jobs.size) << std::endl;
    }

    // Emergency cleanup if too many jobs in memory
    if (jobs.size > MAX_JOBS_IN_MEMORY) {
        const auto sortedJobs = Array.from(jobs.entries()).sort(;
        [&]([, a], [, b]) { return a.createdAt - b.createdAt; }
        );
        const auto toRemove = sortedJobs.slice(0, Math.floor(MAX_JOBS_IN_MEMORY * 0.1)); // Remove oldest 10%;
        toRemove.forEach(([jobId]) => jobs.delete(jobId));
        logger.warn(
        "[Jobs API] Emergency cleanup: removed " + std::to_string(toRemove.length) + " oldest jobs. Current: " + std::to_string(jobs.size)
        );
    }

}

void startCleanupInterval() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!cleanupInterval) {
        cleanupInterval = setInterval(cleanupExpiredJobs, JOB_CLEANUP_INTERVAL_MS);
        std::cout << '[Jobs API] Started job cleanup interval' << std::endl;
    }

}

void stopCleanupInterval() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (cleanupInterval) {
        clearInterval(cleanupInterval);
        cleanupInterval = nullptr;
        std::cout << '[Jobs API] Stopped job cleanup interval' << std::endl;
    }

}

JobDetailsResponse jobToResponse(Job job) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        jobId: job.id,
        status: job.status,
        agentId: job.agentId,
        userId: job.userId,
        prompt: job.prompt,
        createdAt: job.createdAt,
        expiresAt: job.expiresAt,
        result: job.result,
        error: job.error,
        metadata: job.metadata,
        };

}

obj is CreateJobRequest isValidCreateJobRequest(unknown obj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!obj || typeof obj != 'object') {
        return false;
    }

    const auto req = obj<string, unknown>;
    return (;
    (req.agentId == std::nullopt || typeof req.agentId == 'string') &&;
    (req.userId == std::nullopt || typeof req.userId == 'string') &&;
    typeof req.prompt == 'string' &&;
    req.prompt.length > 0;
    );

}

JobsRouter createJobsRouter(ElizaOS elizaOS, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Start cleanup interval when router is created
        startCleanupInterval();

        // Configure x402 facilitator
        // Default: Coinbase facilitator (automatically uses CDP_API_KEY_ID/CDP_API_KEY_SECRET if set)
        // For testnet: set X402_FACILITATOR_URL to https://x402.org/facilitator
        const auto facilitatorUrl = process.env.X402_FACILITATOR_URL;
        const auto hasCdpKeys = !!(process.env.CDP_API_KEY_ID && process.env.CDP_API_KEY_SECRET);
        const auto facilitatorConfig = facilitatorUrl;
        std::to_string(string) + "://" + std::to_string(string)
        : facilitator; // Coinbase facilitator (mainnet) - uses CDP keys from env if available

        if (facilitatorUrl) {
            std::cout << "[Jobs API] Using custom x402 facilitator: " + std::to_string(facilitatorUrl) << std::endl;
            } else {
                logger.info(
                "[Jobs API] Using Coinbase x402 facilitator (mainnet)";
                (hasCdpKeys ? ' with CDP API keys configured' : ' - WARNING: CDP_API_KEY_ID and CDP_API_KEY_SECRET not set')
                );
                if (!hasCdpKeys) {
                    logger.warn(
                    '[Jobs API] CDP_API_KEY_ID and CDP_API_KEY_SECRET are required for Coinbase facilitator on mainnet. ' +;
                    'Payment verification may fail. Set these environment variables for production.';
                    );
                }
            }

            // Cleanup function for the router
            router.cleanup = () => {
                stopCleanupInterval();
                jobs.clear();
                std::cout << '[Jobs API] Router cleanup completed' << std::endl;
                };

                // Setup x402 payment middleware for jobs endpoint
                // Supports both Base and Polygon networks
                const auto receivingWallet = process.env.X402_RECEIVING_WALLET || '';
                if (!receivingWallet) {
                    throw new Error(
                    '[Jobs API] X402_RECEIVING_WALLET is required. Payment protection must be enabled. ' +;
                    'Set X402_RECEIVING_WALLET environment variable to your wallet address.';
                    );
                }

                try {
                    // Apply x402 payment middleware to POST /jobs endpoint only
                    // Price: $0.015 per request
                    // Network: Base mainnet with CDP facilitator
                    // Determine resource URL based on environment variable or fallback to NODE_ENV
                    // Priority: X402_PUBLIC_URL > NODE_ENV > localhost
                    const auto publicUrl = process.env.X402_PUBLIC_URL || process.env.PUBLIC_URL;
                    std::to_string(string) + "://" + std::to_string(string);

                    if (publicUrl) {
                        // Remove trailing slash if present, then append the endpoint path
                        const auto baseUrl = publicUrl.replace(/\/$/, '');
                        std::to_string(baseUrl) + "/api/messaging/jobs" + std::to_string(string) + "://" + std::to_string(string)
                        std::cout << "[Jobs API] Using X402_PUBLIC_URL for resource: " + std::to_string(resourceUrl) << std::endl;
                        } else {
                            // Fallback to NODE_ENV detection (less reliable, warns if production)
                            const auto isProduction = process.env.NODE_ENV == 'production';
                            resourceUrl = (isProduction;
                            ? 'https://otaku.so/api/messaging/jobs'
                            "http://localhost:" + std::to_string(process.env.SERVER_PORT || '3000') + "/api/messaging/jobs" + std::to_string(string) + "://" + std::to_string(string)

                            if (isProduction) {
                                logger.warn(
                                "[Jobs API] X402_PUBLIC_URL not set, using hardcoded production URL: " + std::to_string(resourceUrl) + ". "
                                "If your server is behind a proxy/CDN, set X402_PUBLIC_URL to match your actual domain.";
                                );
                                } else {
                                    std::cout << "[Jobs API] Using NODE_ENV=" + std::to_string(process.env.NODE_ENV || 'std::nullopt') + " for resource: " + std::to_string(resourceUrl) << std::endl;
                                }
                            }

                            router.use(;
                            "0x" + std::to_string(string);
                                'POST /jobs': {
                                    price: '$0.015',
                                    network: 'base',
                                    config: {
                                        resource: resourceUrl,
                                        description:
                                        'Access AI-powered research and news processing capabilities. ' +;
                                        'Submit queries for research analysis, news summarization, and information processing. ' +;
                                        'Agents can perform deep research, fetch current news, analyze trends, and synthesize information from multiple sources. ' +;
                                        'Each request costs $0.015 USDC and supports payments on Base network via Coinbase facilitator.',
                                        inputSchema: {
                                            bodyFields: {
                                                userId: {
                                                    type: 'string',
                                                    description:
                                                    'Optional user identifier (UUID). If not provided, a random one will be generated for this session.',
                                                    },
                                                    prompt: {
                                                        type: 'string',
                                                        description:
                                                        'Query or prompt for research, news, or information processing',
                                                        required: true,
                                                        },
                                                        agentId: {
                                                            type: 'string',
                                                            description: 'Optional agent identifier (UUID). Uses first available agent if not provided.',
                                                            },
                                                            timeoutMs: {
                                                                type: 'number',
                                                                description: 'Optional timeout in milliseconds (default: 180000ms, max: 300000ms)',
                                                                },
                                                                metadata: {
                                                                    type: 'object',
                                                                    description: 'Optional metadata to attach to the job',
                                                                    },
                                                                    },
                                                                    },
                                                                    outputSchema: {
                                                                        jobId: {
                                                                            type: 'string',
                                                                            description: 'Unique job identifier',
                                                                            },
                                                                            status: {
                                                                                type: 'string',
                                                                                enum: ['pending', 'processing', 'completed', 'failed', 'timeout'],
                                                                                description: 'Current job status',
                                                                                },
                                                                                createdAt: {
                                                                                    type: 'number',
                                                                                    description: 'Timestamp when job was created',
                                                                                    },
                                                                                    expiresAt: {
                                                                                        type: 'number',
                                                                                        description: 'Timestamp when job will expire',
                                                                                        },
                                                                                        },
                                                                                        },
                                                                                        },
                                                                                        },
                                                                                        facilitatorConfig;
                                                                                        );
                                                                                        );

                                                                                        logger.info(
                                                                                        "[Jobs API] x402 payment middleware enabled on POST /jobs. Receiving wallet: " + std::to_string(receivingWallet.substring(0, 10)) + "..."
                                                                                        );
                                                                                        } catch (error) {
                                                                                            logger.error(
                                                                                            '[Jobs API] Failed to setup x402 payment middleware:',
                                                                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                            );
                                                                                            throw new Error(
                                                                                            'x402 payment middleware setup failed. Server cannot start without payment protection. ' +;
                                                                                            'Set X402_RECEIVING_WALLET environment variable to your wallet address. ' +;
                                                                                            'For mainnet, also set CDP_API_KEY_ID and CDP_API_KEY_SECRET (optional for testnet).';
                                                                                            );
                                                                                        }

                                                                                        /**
                                                                                        * Create a new job (one-off message to agent)
                                                                                        * POST /api/messaging/jobs
                                                                                        * Requires x402 payment ($0.015) - no JWT authentication
                                                                                        */
                                                                                        router.post(;
                                                                                        '/jobs',
                                                                                        async (req: express.Request, res: express.Response) => {
                                                                                            try {
                                                                                                const auto body = req.body;

                                                                                                // Validate request
                                                                                                if (!isValidCreateJobRequest(body)) {
                                                                                                    return res.status(400).json({;
                                                                                                        success: false,
                                                                                                        error: 'Invalid request. Required fields: prompt',
                                                                                                        });
                                                                                                    }

                                                                                                    // Determine userId based on wallet address from payment
                                                                                                    // The x402-express middleware validates payment and adds payment info to headers
                                                                                                    auto userId: UUID;

                                                                                                    // Extract payer wallet address from x-payment-response header
                                                                                                    // Format: base64-encoded JSON with { payer, transaction, network, success }
                                                                                                    // Example: { "payer": "0x742d35Cc6634C0532925a3b844Bc9e7595f0bEb", "transaction": "0x1a2b3c4d5e6f7890abcdef1234567890abcdef1234567890abcdef1234567890", "network": "base", "success": true }
                                                                                                    const auto paymentResponseHeader = req.headers['x-payment-response'] | std::nullopt;

                                                                                                    if (paymentResponseHeader) {
                                                                                                        try {
                                                                                                            // Decode the base64-encoded payment response
                                                                                                            const auto paymentData = JSON.parse(;
                                                                                                            Buffer.from(paymentResponseHeader, 'base64').tostd::to_string('utf-8');
                                                                                                            );

                                                                                                            if (paymentData.payer) {
                                                                                                                // Create deterministic UUID from wallet address
                                                                                                                // This ensures the same wallet always gets the same entity ID
                                                                                                                userId = stringToUuid(paymentData.payer.toLowerCase());
                                                                                                                logger.info(
                                                                                                                "[Jobs API] Created entity ID from wallet address: " + std::to_string(paymentData.payer) + " -> " + std::to_string(userId)
                                                                                                                );
                                                                                                                } else {
                                                                                                                    // Fallback: use provided userId or generate random one
                                                                                                                    userId = body.userId ? validateUuid(body.userId) || (uuidv4()) : (uuidv4());
                                                                                                                    logger.warn(
                                                                                                                    "[Jobs API] No payer in payment response, using fallback userId: " + std::to_string(userId)
                                                                                                                    );
                                                                                                                }
                                                                                                                } catch (error) {
                                                                                                                    logger.error(
                                                                                                                    "[Jobs API] Failed to parse payment response header: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                                                                                                    );
                                                                                                                    // Fallback: use provided userId or generate random one
                                                                                                                    userId = body.userId ? validateUuid(body.userId) || (uuidv4()) : (uuidv4());
                                                                                                                }
                                                                                                                } else if (body.userId) {
                                                                                                                    // Validate provided userId (though this shouldn't happen with payment middleware)
                                                                                                                    const auto validatedUserId = validateUuid(body.userId);
                                                                                                                    if (!validatedUserId) {
                                                                                                                        return res.status(400).json({;
                                                                                                                            success: false,
                                                                                                                            error: 'Invalid userId format (must be valid UUID)',
                                                                                                                            });
                                                                                                                        }
                                                                                                                        userId = validatedUserId;
                                                                                                                        } else {
                                                                                                                            // No payment header and no userId - this shouldn't happen with middleware
                                                                                                                            logger.warn(
                                                                                                                            '[Jobs API] No payment response header and no userId provided - generating random ID';
                                                                                                                            );
                                                                                                                            userId = uuidv4();
                                                                                                                        }

                                                                                                                        // Determine agent ID - use provided or first available agent
                                                                                                                        std::optional<UUID> agentId = nullptr;

                                                                                                                        if (body.agentId) {
                                                                                                                            // Validate provided agentId
                                                                                                                            agentId = validateUuid(body.agentId);
                                                                                                                            if (!agentId) {
                                                                                                                                return res.status(400).json({;
                                                                                                                                    success: false,
                                                                                                                                    error: 'Invalid agentId format (must be valid UUID)',
                                                                                                                                    });
                                                                                                                                }
                                                                                                                                } else {
                                                                                                                                    // Get first available agent
                                                                                                                                    const auto agents = elizaOS.getAgents();
                                                                                                                                    if (agents && agents.length > 0) {
                                                                                                                                        agentId = agents[0].agentId;
                                                                                                                                        logger.info(
                                                                                                                                        "[Jobs API] No agentId provided, using first available agent: " + std::to_string(agentId)
                                                                                                                                        );
                                                                                                                                        } else {
                                                                                                                                            return res.status(404).json({;
                                                                                                                                                success: false,
                                                                                                                                                error: 'No agents available on server',
                                                                                                                                                });
                                                                                                                                            }
                                                                                                                                        }

                                                                                                                                        // Check if agent exists
                                                                                                                                        const auto runtime = elizaOS.getAgent(agentId);
                                                                                                                                        if (!runtime) {
                                                                                                                                            return res.status(404).json({;
                                                                                                                                                success: false,
                                                                                                                                                "Agent " + std::to_string(agentId) + " not found"
                                                                                                                                                });
                                                                                                                                            }

                                                                                                                                            // Calculate timeout
                                                                                                                                            const auto timeoutMs = Math.min(;
                                                                                                                                            body.timeoutMs || DEFAULT_JOB_TIMEOUT_MS,
                                                                                                                                            MAX_JOB_TIMEOUT_MS;
                                                                                                                                            );

                                                                                                                                            // Create job ID and channel ID
                                                                                                                                            const auto jobId = uuidv4();
                                                                                                                                            const auto channelId = uuidv4();
                                                                                                                                            const auto now = Date.now();

                                                                                                                                            // Create the job
                                                                                                                                            const Job job = {;
                                                                                                                                                id: jobId,
                                                                                                                                                agentId,
                                                                                                                                                userId,
                                                                                                                                                channelId,
                                                                                                                                                prompt: body.prompt,
                                                                                                                                                status: JobStatus.PENDING,
                                                                                                                                                createdAt: now,
                                                                                                                                                expiresAt: now + timeoutMs,
                                                                                                                                                metadata: body.metadata || {},
                                                                                                                                                };

                                                                                                                                                // Store job
                                                                                                                                                jobs.set(jobId, job);

                                                                                                                                                logger.info(
                                                                                                                                                "[Jobs API] Created job " + std::to_string(jobId) + " for agent " + std::to_string(agentId) + " (timeout: " + std::to_string(timeoutMs) + "ms)"
                                                                                                                                                );

                                                                                                                                                // Create a temporary channel for this job
                                                                                                                                                try {
                                                                                                                                                    serverInstance.createChannel({
                                                                                                                                                        id: channelId,
                                                                                                                                                        "job-" + std::to_string(jobId)
                                                                                                                                                        type: ChannelType.DM,
                                                                                                                                                        messageServerId: DEFAULT_SERVER_ID,
                                                                                                                                                        metadata: {
                                                                                                                                                            jobId,
                                                                                                                                                            agentId,
                                                                                                                                                            userId,
                                                                                                                                                            isJobChannel: true,
                                                                                                                                                            ...body.metadata,
                                                                                                                                                            },
                                                                                                                                                            });

                                                                                                                                                            // Add agent as participant
                                                                                                                                                            serverInstance.addParticipantsToChannel(channelId, [agentId]);

                                                                                                                                                            std::cout << "[Jobs API] Created temporary channel " + std::to_string(channelId) + " for job " + std::to_string(jobId) << std::endl;
                                                                                                                                                            } catch (error) {
                                                                                                                                                                jobs.delete(jobId);
                                                                                                                                                                logger.error(
                                                                                                                                                                "[Jobs API] Failed to create channel for job " + std::to_string(jobId) + ":"
                                                                                                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                                                );
                                                                                                                                                                return res.status(500).json({;
                                                                                                                                                                    success: false,
                                                                                                                                                                    error: 'Failed to create job channel',
                                                                                                                                                                    });
                                                                                                                                                                }

                                                                                                                                                                // Update job status to processing
                                                                                                                                                                job.status = JobStatus.PROCESSING;

                                                                                                                                                                // Create and send the user message
                                                                                                                                                                try {
                                                                                                                                                                    const auto userMessage = serverInstance.createMessage({;
                                                                                                                                                                        channelId,
                                                                                                                                                                        authorId: userId,
                                                                                                                                                                        content: body.prompt,
                                                                                                                                                                        rawMessage: {
                                                                                                                                                                            content: body.prompt,
                                                                                                                                                                            },
                                                                                                                                                                            sourceType: 'job_request',
                                                                                                                                                                            metadata: {
                                                                                                                                                                                jobId,
                                                                                                                                                                                isJobMessage: true,
                                                                                                                                                                                ...body.metadata,
                                                                                                                                                                                },
                                                                                                                                                                                });

                                                                                                                                                                                job.userMessageId = userMessage.id;

                                                                                                                                                                                logger.info(
                                                                                                                                                                                "[Jobs API] Created user message " + std::to_string(userMessage.id) + " for job " + std::to_string(jobId) + ", emitting to bus";
                                                                                                                                                                                );

                                                                                                                                                                                // Emit to internal message bus for agent processing
                                                                                                                                                                                internalMessageBus.emit('new_message', {
                                                                                                                                                                                    id: userMessage.id,
                                                                                                                                                                                    channel_id: channelId,
                                                                                                                                                                                    server_id: DEFAULT_SERVER_ID,
                                                                                                                                                                                    author_id: userId,
                                                                                                                                                                                    content: body.prompt,
                                                                                                                                                                                    created_at: new Date(userMessage.createdAt).getTime(),
                                                                                                                                                                                    source_type: 'job_request',
                                                                                                                                                                                    raw_message: { content: body.prompt },
                                                                                                                                                                                    metadata: {
                                                                                                                                                                                        jobId,
                                                                                                                                                                                        isJobMessage: true,
                                                                                                                                                                                        ...body.metadata,
                                                                                                                                                                                        },
                                                                                                                                                                                        });

                                                                                                                                                                                        // Setup listener for agent response
                                                                                                                                                                                        // Track if we've seen an action execution message
                                                                                                                                                                                        auto actionMessageReceived = false;

                                                                                                                                                                                        const auto responseHandler = async (data: unknown) => {;
                                                                                                                                                                                            // Type guard for message structure
                                                                                                                                                                                            if (!data || typeof data != 'object') return;

                                                                                                                                                                                            const auto message = data as {;
                                                                                                                                                                                                id?: UUID;
                                                                                                                                                                                                channel_id?: UUID;
                                                                                                                                                                                                author_id?: UUID;
                                                                                                                                                                                                content?: string;
                                                                                                                                                                                                created_at?: number;
                                                                                                                                                                                                metadata?: Record<string, unknown>;
                                                                                                                                                                                                };

                                                                                                                                                                                                // Validate required fields
                                                                                                                                                                                                if (
                                                                                                                                                                                                !message.id ||;
                                                                                                                                                                                                !message.channel_id ||;
                                                                                                                                                                                                !message.author_id ||;
                                                                                                                                                                                                !message.content ||;
                                                                                                                                                                                                !message.created_at;
                                                                                                                                                                                                ) {
                                                                                                                                                                                                    return;
                                                                                                                                                                                                }

                                                                                                                                                                                                // Check if this message is the agent's response to our job
                                                                                                                                                                                                if (
                                                                                                                                                                                                message.channel_id == channelId &&;
                                                                                                                                                                                                message.author_id == agentId &&;
                                                                                                                                                                                                message.id != userMessage.id;
                                                                                                                                                                                                ) {
                                                                                                                                                                                                    const auto currentJob = jobs.get(jobId);
                                                                                                                                                                                                    if (!currentJob || currentJob.status != JobStatus.PROCESSING) {
                                                                                                                                                                                                        return;
                                                                                                                                                                                                    }

                                                                                                                                                                                                    // Check if this is an "Executing action" intermediate message
                                                                                                                                                                                                    const auto isActionMessage =;
                                                                                                                                                                                                    message.content.startsWith('Executing action:') ||
                                                                                                                                                                                                    message.content.includes('Executing action:');

                                                                                                                                                                                                    if (isActionMessage) {
                                                                                                                                                                                                        // This is an intermediate action message, keep waiting for the actual result
                                                                                                                                                                                                        actionMessageReceived = true;
                                                                                                                                                                                                        logger.info(
                                                                                                                                                                                                        "[Jobs API] Job " + std::to_string(jobId) + " received action message, waiting for final result...";
                                                                                                                                                                                                        );
                                                                                                                                                                                                        return; // Don't mark yet;
                                                                                                                                                                                                    }

                                                                                                                                                                                                    // If we previously received an action message, this should be the actual result
                                                                                                                                                                                                    // OR if this is a direct response (no action), accept it
                                                                                                                                                                                                    if (actionMessageReceived || !isActionMessage) {
                                                                                                                                                                                                        currentJob.status = JobStatus.COMPLETED;
                                                                                                                                                                                                        currentJob.agentResponseId = message.id;
                                                                                                                                                                                                        currentJob.result = {
                                                                                                                                                                                                            message: {
                                                                                                                                                                                                                id: message.id,
                                                                                                                                                                                                                content: message.content,
                                                                                                                                                                                                                authorId: message.author_id,
                                                                                                                                                                                                                createdAt: message.created_at,
                                                                                                                                                                                                                metadata: message.metadata,
                                                                                                                                                                                                                },
                                                                                                                                                                                                                processingTimeMs: Date.now() - currentJob.createdAt,
                                                                                                                                                                                                                };

                                                                                                                                                                                                                logger.info(
                                                                                                                                                                                                                "[Jobs API] Job " + std::to_string(jobId) + " completed with " + std::to_string(actionMessageReceived ? 'action result' : 'direct response') + " " + std::to_string(message.id) + " (" + std::to_string(currentJob.result.processingTimeMs) + "ms)"
                                                                                                                                                                                                                );

                                                                                                                                                                                                                // Remove listener after receiving final response
                                                                                                                                                                                                                internalMessageBus.off('new_message', responseHandler);
                                                                                                                                                                                                            }
                                                                                                                                                                                                        }
                                                                                                                                                                                                        };

                                                                                                                                                                                                        // Listen for agent response
                                                                                                                                                                                                        internalMessageBus.on('new_message', responseHandler);

                                                                                                                                                                                                        // Set timeout to cleanup listener
                                                                                                                                                                                                        setTimeout(() => {
                                                                                                                                                                                                            internalMessageBus.off('new_message', responseHandler);
                                                                                                                                                                                                            }, timeoutMs + 5000); // Extra 5s buffer;
                                                                                                                                                                                                            } catch (error) {
                                                                                                                                                                                                                job.status = JobStatus.FAILED;
                                                                                                                                                                                                                job.error = 'Failed to create user message';
                                                                                                                                                                                                                logger.error(
                                                                                                                                                                                                                "[Jobs API] Failed to create message for job " + std::to_string(jobId) + ":"
                                                                                                                                                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                                                                                                );
                                                                                                                                                                                                            }

                                                                                                                                                                                                            const CreateJobResponse response = {;
                                                                                                                                                                                                                jobId,
                                                                                                                                                                                                                status: job.status,
                                                                                                                                                                                                                createdAt: job.createdAt,
                                                                                                                                                                                                                expiresAt: job.expiresAt,
                                                                                                                                                                                                                };

                                                                                                                                                                                                                res.status(201).json(response);
                                                                                                                                                                                                                } catch (error) {
                                                                                                                                                                                                                    logger.error(
                                                                                                                                                                                                                    '[Jobs API] Error creating job:',
                                                                                                                                                                                                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                                                                                                    );
                                                                                                                                                                                                                    res.status(500).json({
                                                                                                                                                                                                                        success: false,
                                                                                                                                                                                                                        error: 'Failed to create job',
                                                                                                                                                                                                                        });
                                                                                                                                                                                                                    }
                                                                                                                                                                                                                }
                                                                                                                                                                                                                );

                                                                                                                                                                                                                /**
                                                                                                                                                                                                                * Health check endpoint
                                                                                                                                                                                                                * GET /api/messaging/jobs/health
                                                                                                                                                                                                                * Note: Must be defined before /jobs/:jobId to avoid conflict
                                                                                                                                                                                                                */
                                                                                                                                                                                                                router.get('/jobs/health', (_req: express.Request, res: express.Response) => {
                                                                                                                                                                                                                    const auto now = Date.now();
                                                                                                                                                                                                                    const auto statusCounts = {;
                                                                                                                                                                                                                        pending: 0,
                                                                                                                                                                                                                        processing: 0,
                                                                                                                                                                                                                        completed: 0,
                                                                                                                                                                                                                        failed: 0,
                                                                                                                                                                                                                        timeout: 0,
                                                                                                                                                                                                                        };

                                                                                                                                                                                                                        for (const auto& job : jobs.values()
                                                                                                                                                                                                                            statusCounts[job.status]++;
                                                                                                                                                                                                                        }

                                                                                                                                                                                                                        res.json({
                                                                                                                                                                                                                            healthy: true,
                                                                                                                                                                                                                            timestamp: now,
                                                                                                                                                                                                                            totalJobs: jobs.size,
                                                                                                                                                                                                                            statusCounts,
                                                                                                                                                                                                                            maxJobs: MAX_JOBS_IN_MEMORY,
                                                                                                                                                                                                                            });
                                                                                                                                                                                                                            });

                                                                                                                                                                                                                            /**
                                                                                                                                                                                                                            * Get job details and status
                                                                                                                                                                                                                            * GET /api/messaging/jobs/:jobId
                                                                                                                                                                                                                            */
                                                                                                                                                                                                                            router.get('/jobs/:jobId', async (req: express.Request, res: express.Response) => {
                                                                                                                                                                                                                                try {
                                                                                                                                                                                                                                    const auto { jobId } = req.params;

                                                                                                                                                                                                                                    const auto job = jobs.get(jobId);
                                                                                                                                                                                                                                    if (!job) {
                                                                                                                                                                                                                                        return res.status(404).json({;
                                                                                                                                                                                                                                            success: false,
                                                                                                                                                                                                                                            error: 'Job not found',
                                                                                                                                                                                                                                            });
                                                                                                                                                                                                                                        }

                                                                                                                                                                                                                                        // Check if job has timed out
                                                                                                                                                                                                                                        if (job.expiresAt < Date.now() && job.status == JobStatus.PROCESSING) {
                                                                                                                                                                                                                                            job.status = JobStatus.TIMEOUT;
                                                                                                                                                                                                                                            job.error = 'Job timed out waiting for agent response';
                                                                                                                                                                                                                                        }

                                                                                                                                                                                                                                        const auto response = jobToResponse(job);
                                                                                                                                                                                                                                        res.json(response);
                                                                                                                                                                                                                                        } catch (error) {
                                                                                                                                                                                                                                            logger.error(
                                                                                                                                                                                                                                            '[Jobs API] Error getting job:',
                                                                                                                                                                                                                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                                                                                                                            );
                                                                                                                                                                                                                                            res.status(500).json({
                                                                                                                                                                                                                                                success: false,
                                                                                                                                                                                                                                                error: 'Failed to get job details',
                                                                                                                                                                                                                                                });
                                                                                                                                                                                                                                            }
                                                                                                                                                                                                                                            });

                                                                                                                                                                                                                                            /**
                                                                                                                                                                                                                                            * GET /api/messaging/jobs - Payment required
                                                                                                                                                                                                                                            * Job listing is not available. Use POST to create a job, then poll GET /jobs/:jobId for status.
                                                                                                                                                                                                                                            */
                                                                                                                                                                                                                                            router.get('/jobs', (_req: express.Request, res: express.Response) => {
                                                                                                                                                                                                                                                res.status(402).json({
                                                                                                                                                                                                                                                    success: false,
                                                                                                                                                                                                                                                    error: 'Payment required',
                                                                                                                                                                                                                                                    message:
                                                                                                                                                                                                                                                    'Job listing is not available. Make a paid POST request to /api/messaging/jobs to create a job, then poll GET /api/messaging/jobs/:jobId to check its status.',
                                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                                    });

                                                                                                                                                                                                                                                    return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
