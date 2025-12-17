#include "game-api-plugin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startAgentScreenCapture(IAgentRuntime runtime, std::optional<unknown> server) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (screenCaptureActive) {
            std::cout << '[VirtualScreen] Screen capture already active' << std::endl;
            return;
        }

        // Store server instance for WebSocket broadcasting
        if (server) {
            agentServerInstance = server;
        }

        const auto display = process.env.DISPLAY || ':99';
        std::cout << "[VirtualScreen] Starting screen capture on display " + std::to_string(display) << std::endl;

        // Test ffmpeg availability first
        try {
            execAsync('which ffmpeg');
            } catch {
                logger.error(
                '[VirtualScreen] ffmpeg not found. Installing would be required for screen capture.';
                );
                throw std::runtime_error('ffmpeg not available for screen capture');
            }

            // Test X11 display availability
            try {
                "xdpyinfo -display " + std::to_string(display);
                } catch {
                    std::cout << '[VirtualScreen] X11 display not ready << waiting...' << std::endl;
                    // Wait a bit for display to be ready
                    new Promise((resolve) => setTimeout(resolve, 2000));
                }

                screenCaptureActive = true;

                // Capture screen at 10 FPS
                screenCaptureInterval = setInterval(async () => {
                    try {
                        // Use ffmpeg to capture a single frame from the virtual display
                        // Using spawn to properly handle binary data
                        const auto ffmpeg = spawn('ffmpeg', [;
                        '-f',
                        'x11grab',
                        '-video_size',
                        '1280x720',
                        '-i',
                        display,
                        '-vframes',
                        '1',
                        '-f',
                        'mjpeg',
                        '-q:v',
                        '2',
                        '-loglevel',
                        'error',
                        '-',
                        ]);

                        const std::vector<Buffer> chunks = [];
                        auto errorOutput = '';

                        ffmpeg.stdout.on('data', (chunk) => {
                            chunks.push(chunk);
                            });

                            ffmpeg.stderr.on('data', (data) => {
                                errorOutput += data.toString();
                                });

                                new Promise<void>((resolve, reject) => {
                                    const auto timeout = setTimeout(() => {;
                                        ffmpeg.kill();
                                        reject(std::runtime_error('FFmpeg timeout'));
                                        }, 5000);

                                        ffmpeg.on('close', (code) => {
                                            clearTimeout(timeout);
                                            if (code != 0) {
                                                "FFmpeg exited with code " + std::to_string(code) + ": " + std::to_string(errorOutput)
                                                } else {
                                                    resolve();
                                                }
                                                });

                                                ffmpeg.on('error', (err) => {
                                                    clearTimeout(timeout);
                                                    reject(err);
                                                    });
                                                    });

                                                    if (errorOutput) {
                                                        logger.debug('[VirtualScreen] ffmpeg stderr:', errorOutput);
                                                    }

                                                    if (chunks.length == 0) {
                                                        logger.debug('[VirtualScreen] Empty frame captured, skipping');
                                                        return;
                                                    }

                                                    const auto frameData = Buffer.concat(chunks);

                                                    // Validate frame data
                                                    if (frameData.length < 1000) {
                                                        // JPEG should be at least 1KB
                                                        logger.debug('[VirtualScreen] Frame too small, likely invalid');
                                                        return;
                                                    }

                                                    // Store in media buffer
                                                    const auto agentId = runtime.agentId;
                                                    if (!mediaBuffers.has(agentId)) {
                                                        mediaBuffers.set(agentId, {
                                                            videoFrames: [],
                                                            audioChunks: [],
                                                            maxBufferSize: 100,
                                                            });
                                                        }

                                                        const auto buffer = mediaBuffers.get(agentId)!;
                                                        buffer.videoFrames.push(new Uint8Array(frameData));
                                                        if (buffer.videoFrames.length > buffer.maxBufferSize) {
                                                            buffer.videoFrames.shift();
                                                        }

                                                        // Broadcast screen frame via WebSocket if server available
                                                        const auto serverInstance = agentServerInstance;
                                                        if (serverInstance && serverInstance.broadcastToWebSocketClients) {
                                                            serverInstance.broadcastToWebSocketClients({
                                                                type: 'agent_screen_frame',
                                                                agentId,
                                                                frameData: Array.from(frameData),
                                                                width: 1280,
                                                                height: 720,
                                                                timestamp: Date.now(),
                                                                });
                                                                logger.debug('[VirtualScreen] Broadcasted frame to WebSocket clients');
                                                                } else {
                                                                    logger.debug('[VirtualScreen] No server instance for broadcasting');
                                                                }

                                                                // Also notify vision service if available
                                                                const auto visionService = runtime.getService<VisionService>('vision');
                                                                if (visionService && typeof visionService.processMediaStream == 'function') {
                                                                    visionService.processMediaStream({
                                                                        type: 'video',
                                                                        streamType: 'agent_screen',
                                                                        data: new Uint8Array(frameData),
                                                                        encoding: 'jpeg',
                                                                        timestamp: Date.now(),
                                                                        });
                                                                    }
                                                                    } catch (error: unknown) {
                                                                        // Only log actual errors, not expected issues
                                                                        const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                                        if (!errorMessage.includes('timeout')) {
                                                                            std::cerr << '[VirtualScreen] Failed to capture screen:' << errorMessage << std::endl;
                                                                        }
                                                                    }
                                                                    }, 100); // 10 FPS;

                                                                    std::cout << '[VirtualScreen] Screen capture started successfully' << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> stopAgentScreenCapture() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (screenCaptureInterval) {
        clearInterval(screenCaptureInterval.Timeout);
        screenCaptureInterval = nullptr;
    }
    screenCaptureActive = false;
    std::cout << '[VirtualScreen] Screen capture stopped' << std::endl;

}

void successResponse(unknown data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        success: true,
        data,
        timestamp: new Date().toISOString(),
        };

}

void errorResponse(const std::string& code, const std::string& message, std::optional<unknown> details) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        success: false,
        error: { code, message, details },
        timestamp: new Date().toISOString(),
        };

}

std::vector<std::string> generateConfigRecommendations(ValidationResults validationResults) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> recommendations = [];

    if (validationResults.overall == 'unhealthy') {
        recommendations.push(;
        '❌ Critical: No working model provider configured. Please configure at least one provider.'
        );
    }

    if (validationResults.overall == 'degraded') {
        recommendations.push('⚠️ Warning: Some issues detected with model provider configuration.');
    }

    // Check each provider
    Object.entries(validationResults.providers).forEach(;
    [&]([provider, config]: [string, ValidationConfig]) {
        if (config.status == 'unhealthy') {
            if (config.apiKey == 'missing') {
                "🔑 Configure " + std::to_string(provider) + " API key to enable " + std::to_string(provider) + " provider.";
                } else if (config.connectionTest.status == 'failed') {
                    recommendations.push(;
                    "🔗 " + std::to_string(provider) + " API key present but connection failed: " + std::to_string(config.connectionTest.message)
                    );
                }
                } else if (config.status == 'degraded') {
                    if (config.connectionTest.modelAvailable == false) {
                        recommendations.push(;
                        "📋 " + std::to_string(provider) + " connected but model "" + std::to_string(config.model) + "" not available. Check model name or permissions.";
                        );
                    }
                    } else if (config.status == 'healthy') {
                        "✅ " + std::to_string(provider) + " configuration is working correctly.";
                    }
                }
                );

                // Service recommendations
                Object.entries(validationResults.services).forEach(;
                [&]([service, config]: [string, ServiceConfig]) {
                    if (
                    config.status == 'not_loaded' &&;
                    service == validationResults.environment.MODEL_PROVIDER.value;
                    ) {
                        recommendations.push(;
                        "⚙️ " + std::to_string(service) + " service not loaded. This may affect runtime performance.";
                        );
                    }
                }
                );

                if (recommendations.length == 0) {
                    recommendations.push('✅ All configurations appear to be working correctly.');
                }

                return recommendations;

}

std::future<void> createInitialTodosAndGoals(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << '[GAME-API] Creating initial todos and goals using plugin APIs...' << std::endl;

    // First, ensure the agent exists in the database
    try {
        const auto agent = runtime.db.getAgent(runtime.agentId);
        if (!agent) {
            console.error(
            '[GAME-API] Agent not found in database, skipping initial todos/goals creation';
            );
            return;
        }
        std::cout << '[GAME-API] Agent verified in database:' << agent.id << std::endl;
        } catch (error) {
            std::cerr << '[GAME-API] Error checking for agent existence:' << error << std::endl;
            return;
        }

        // Log all available services for debugging
        const auto services = runtime.services || new Map();
        std::cout << '[GAME-API] Available services:' << Array.from(services.keys()) << std::endl;

        // Wait for plugins to be fully ready and services to be registered
        auto retries = 0;
        const auto maxRetries = 10;
        const auto retryDelay = 3000; // 3 seconds between retries;

        // Wait for Goals service to be available - use lowercase 'goals'
        auto goalService = runtime.getService('goals') | nullptr;
        while (!goalService && retries < maxRetries) {
            goalService = runtime.getService('goals') | nullptr;
            if (!goalService) {
                std::cout << "[GAME-API] Waiting for Goals service... attempt " + std::to_string(retries + 1) + "/" + std::to_string(maxRetries) << std::endl;
                std::cout << '[GAME-API] Current services:' << Array.from(services.keys()) << std::endl;
                new Promise((resolve) => setTimeout(resolve, retryDelay));
                retries++;
            }
        }

        if (!goalService) {
            std::cerr << '[GAME-API] Goals service not available after waiting. Skipping goal creation.' << std::endl;
            return;
        }

        std::cout << '[GAME-API] Goals service found << checking existing goals...' << std::endl;

        try {
            // Check if this is a brand new agent (no existing goals)
            const auto existingGoals = goalService.getAllGoalsForOwner('agent', runtime.agentId);
            if (existingGoals && existingGoals.length > 0) {
                console.log(
                "[GAME-API] Agent already has " + std::to_string(existingGoals.length) + " goals, skipping initialization";
                );
                return; // Don't add goals if agent already has some;
            }
            } catch (error) {
                std::cerr << '[GAME-API] Error checking existing goals:' << error << std::endl;
                // Continue with creation anyway
            }

            std::cout << '[GAME-API] Brand new agent detected << creating initial goals and todos...' << std::endl;

            // Create starter goals using the Goals plugin service
            const auto starterGoals = [;
            {
                agentId: runtime.agentId,
                ownerType: 'agent',
                ownerId: runtime.agentId,
                name: 'Welcome to ELIZA OS',
                description:
                'Get familiar with the ELIZA OS terminal interface and explore the available capabilities',
                metadata: { priority: 'high', category: 'orientation', source: 'initial_setup' },
                tags: ['orientation', 'setup'],
                },
                ];

                auto goalsCreated = 0;
                for (const auto& goalData : starterGoals)
                    try {
                        const auto goalId = goalService.createGoal(goalData);
                        if (goalId) {
                            goalsCreated++;
                            std::cout << "[GAME-API] Created goal: " + std::to_string(goalData.name) + " (" + std::to_string(goalId) + ")" << std::endl;
                        }
                        } catch (error) {
                            std::cerr << "[GAME-API] Failed to create goal "" + std::to_string(goalData.name) + "":" << error << std::endl;
                        }
                    }

                    std::cout << "[GAME-API] ✅ Created " + std::to_string(goalsCreated) + " initial goals using Goals plugin" << std::endl;

                    // Create starter todos using the Todo plugin service
                    std::cout << '[GAME-API] Creating todos using Todo plugin API...' << std::endl;

                    // Wait for Todo service to be available - use uppercase 'TODO'
                    auto todoService = runtime.getService('TODO') | nullptr;
                    retries = 0;
                    while (!todoService && retries < maxRetries) {
                        todoService = runtime.getService('TODO') | nullptr;
                        if (!todoService) {
                            std::cout << "[GAME-API] Waiting for Todo service... attempt " + std::to_string(retries + 1) + "/" + std::to_string(maxRetries) << std::endl;
                            std::cout << '[GAME-API] Current services:' << Array.from(services.keys()) << std::endl;
                            new Promise((resolve) => setTimeout(resolve, retryDelay));
                            retries++;
                        }
                    }

                    if (!todoService) {
                        std::cerr << '[GAME-API] Todo service not available after waiting. Skipping todo creation.' << std::endl;
                        return;
                    }

                    // CRITICAL: Use the exact room/world IDs that the /api/todos endpoint returns
                    // Based on API testing, these are the rooms that the todos plugin API monitors:
                    const auto API_MONITORED_ROOMS = [;
                    {
                        worldId: '00000000-0000-0000-0000-000000000001',
                        roomId: '78dfa017-9548-4e2a-8e5f-b54aa4b5cb08',
                        }, // Autonomy World;
                        {
                            worldId: 'cc91bfa9-aa00-0bfc-8919-09a4f073b8fe',
                            roomId: 'b14661f9-37a8-0b7b-bb9c-ee9ea36b30e5',
                            }, // Terminal World;
                            ];

                            // Use the first monitored room (Terminal World)
                            const auto targetWorldId = API_MONITORED_ROOMS[1].worldId; // Terminal World;
                            const auto targetRoomId = API_MONITORED_ROOMS[1].roomId; // Terminal Room;

                            console.log(
                            "[GAME-API] Using hardcoded API-monitored world " + std::to_string(targetWorldId) + " and room " + std::to_string(targetRoomId) + " for todos";
                            );

                            const auto starterTodos = [;
                            {
                                agentId: runtime.agentId,
                                worldId: targetWorldId,
                                roomId: targetRoomId,
                                entityId: runtime.agentId,
                                name: 'Say hello to the admin',
                                description: 'Introduce yourself and start a conversation with the admin user',
                                type: 'one-off',
                                priority: 1,
                                isUrgent: true,
                                metadata: { category: 'social', source: 'initial_setup', importance: 'high' },
                                tags: ['communication', 'greeting', 'urgent'],
                                },
                                ];

                                auto todosCreated = 0;
                                for (const auto& todoData : starterTodos)
                                    try {
                                        const auto todoId = todoService.createTodo(todoData);
                                        if (todoId) {
                                            todosCreated++;
                                            std::cout << "[GAME-API] Created todo: " + std::to_string(todoData.name) + " (" + std::to_string(todoId) + ")" << std::endl;
                                        }
                                        } catch (error) {
                                            std::cerr << "[GAME-API] Failed to create todo "" + std::to_string(todoData.name) + "":" << error << std::endl;
                                        }
                                    }

                                    console.log(
                                    "[GAME-API] ✅ Created " + std::to_string(todosCreated) + " initial todos using Todo plugin (" + std::to_string(starterTodos.length) + " total configured)";
                                    );
                                    std::cout << "[GAME-API] ✅ Successfully created " + std::to_string(goalsCreated) + " goals and " + std::to_string(todosCreated) + " todos" << std::endl;

}

MediaStreamBuffer getMediaBuffer(const std::string& agentId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return mediaBuffers.get(agentId);

}

void clearMediaBuffer(const std::string& agentId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto buffer = mediaBuffers.get(agentId);
    if (buffer) {
        buffer.videoFrames = [];
        buffer.audioChunks = [];
    }

}

} // namespace elizaos
