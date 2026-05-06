#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void loadEnvIfExists(const std::string& envPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (fs.existsSync(envPath)) {
        dotenv.config({ path: envPath });
    }

}

std::vector<Plugin> createInitialPlugins() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return [;
    sqlPlugin, // Always needed for database;
    bootstrapPlugin, // Always needed for basic runtime;
    gameAPIPlugin, // Always needed for game API;
    inferencePlugin, // Always needed for inference;
    autonomyPlugin, // Start with autonomy enabled;
    knowledgePlugin, // Always needed for memory;
    PersonalityPlugin, // Always needed for character;
    experiencePlugin, // Always needed for learning;
    shellPlugin, // Shell is available from level 0;
    // Goals and Todo will be added via progression at level 2
    ].filter(Boolean);

}

std::future<IAgentRuntime> startAgent(Character character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "[AGENT START] Starting agent:" << character.name << std::endl;

    // Generate the proper agent ID from the character name
    const auto agentId = stringToUuid(character.name);
    std::cout << "[AGENT START] Generated agent ID:" << agentId << "for character:" << character.name << std::endl;

    std::cout << "[AGENT START] Using embedding provider:" << embeddingProvider << std::endl;
    std::cout << "[AGENT START] Using model provider:" << modelProvider << std::endl;

    console.log(
    "[AGENT START] Initial plugins:",
    initialPlugins.map((p) => p.name || "unnamed").join(", ");
    );

    // Ensure character has proper structure with UUID string
    const auto cleanCharacter = {;
        ...character,
        id: agentId, // Ensure ID is always a string UUID
        };

        // Remove any nested objects that might have been accidentally included
        if (typeof cleanCharacter.id != 'string') {
            std::cout << '[AGENT START] Character ID was not a string << fixing...' << std::endl;
            cleanCharacter.id = agentId;
        }

        // Create the runtime using ElizaAgentRuntime with initial plugins only
        const auto runtime = new ElizaAgentRuntime({;
            agentId,
            character: cleanCharacter,
            plugins: initialPlugins,
            });

            // Define plugin mappings for progressive unlocking
            const std::unordered_map<std::string, std::vector<Plugin>> pluginMappings = {;
                // Level 1 capabilities
                browser: [allAvailablePlugins.stagehand], // Browser capability uses Stagehand for web automation
                stagehand: [allAvailablePlugins.stagehand], // Alias for backward compatibility

                // Level 2 capabilities
                goals: [allAvailablePlugins.goals],
                todo: [allAvailablePlugins.todo],

                // Level 3 capabilities
                vision: [allAvailablePlugins.vision], // Vision plugin handles both camera and screen capture

                // Level 4 capabilities
                sam: [allAvailablePlugins.sam], // SAM plugin for TTS/audio output

                // Level 5 capabilities
                advanced: [], // Placeholder for future advanced capabilities

                // Level 6 capabilities
                autocoder: [allAvailablePlugins.autocoder],
                code_generation: [allAvailablePlugins.autocoder],
                project_management: [allAvailablePlugins.autocoder],
                "plugin-manager": [allAvailablePlugins.pluginManager],

                // Note: 'shell' is already in initial plugins, no need to register progressively
                // Note: 'naming' is handled by ProgressionTracker, not a plugin capability
                };

                console.log(
                "[AGENT START] AgentRuntime created with initial capabilities and progressive plugin support";
                );

                // Initialize runtime - this will set up database connection AND create the agent via ensureAgentExists
                runtime.initialize();
                console.log(
                "[AGENT START] Runtime initialized - agent creation handled by runtime.ensureAgentExists()";
                );

                // Register progression services
                runtime.registerService(CapabilityProgressionService);
                runtime.registerService(ProgressivePluginService);

                // Get the registered services
                const auto progressionService =;
                runtime.getService<CapabilityProgressionService>("CAPABILITY_PROGRESSION");
                const auto progressivePluginService =;
                runtime.getService<ProgressivePluginService>("PROGRESSIVE_PLUGIN");

                if (progressivePluginService) {
                    progressivePluginService.setAvailablePlugins(pluginMappings);
                }

                // Initialize the progression tracker (event listener only)
                if (progressionService) {
                    const auto _progressionTracker = new ProgressionTracker(runtime, progressionService);
                }

                std::cout << "[AGENT START] Capability progression system initialized" << std::endl;

                return runtime;

}

std::future<void> startServer() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Check for existing database URL in environment variables first
        const auto envDatabaseUrl = process.env.POSTGRES_URL || process.env.DATABASE_URL;

        // Use localhost for local development, eliza-postgres for container environments
        const auto isContainer =;
        process.env.CONTAINER == "true" ||;
        process.env.AGENT_CONTAINER == "true" ||;
        process.env.DOCKER_CONTAINER == "true" ||;
        fs.existsSync("/.dockerenv");
        const auto postgresHost = isContainer ? "eliza-postgres:5432" : "localhost:5432";
        const auto fallbackDatabaseUrl = "postgresql://eliza:eliza_secure_pass@" + postgresHost + "/eliza";

        // Use environment variable if set, otherwise use fallback
        const auto databaseUrl = envDatabaseUrl || fallbackDatabaseUrl;
        const auto _dataDir = path.resolve(process.cwd(), "data");

        // Create and initialize server
        const auto server = new AgentServer();

        // Make server instance globally available for MessageBusService
        (global<string, unknown>).elizaAgentServer = server;

        // Assign the startAgent method to make it compatible with the lifecycle API
        (server<string, unknown>).startAgent = async (character: Character) => {
            std::cout << "[SERVER] Starting agent via API call:" << character.name << std::endl;
            const auto runtime = startAgent(character);
            server.registerAgent(runtime);
            return runtime;
            };

            std::cout << "[BACKEND] Using PostgreSQL database " + databaseUrl << std::endl;

            // In containers, retry initialization to wait for PostgreSQL
            const auto maxRetries = 30; // 30 seconds total;
            auto retries = 0;
            auto initialized = false;

            while (!initialized && retries < maxRetries) {
                try {
                    server.initialize(databaseUrl);
                    initialized = true;
                    } catch (error) {
                        retries++;
                        if (retries < maxRetries) {
                            std::cout << "[BACKEND] Waiting for PostgreSQL... (" + retries + "/" + maxRetries + ")" << std::endl;
                            new Promise((resolve) => setTimeout(resolve, 1000));
                            } else {
                                throw new Error(
                                "Failed to connect to PostgreSQL after " + maxRetries + " attempts: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                );
                            }
                        }
                    }

                    // Add file upload middleware for knowledge document uploads
                    const auto fileUpload = import("express-fileupload");
                    server.app.use(;
                    fileUpload.default({
                        limits: { fileSize: 10 * 1024 * 1024 }, // 10MB max file size
                        useTempFiles: true,
                        tempFileDir: "/tmp/",
                        createParentPath: true,
                        });
                        );
                        std::cout << "[BACKEND] ✅ All plugin migrations completed" << std::endl;

                        // Create and register the default agent BEFORE starting the server
                        // This ensures the agent exists when WebSocket messages arrive
                        const auto runtime = startAgent(terminalCharacter);
                        server.registerAgent(runtime);
                        console.log(
                        "[BACKEND] ✅ Default agent started and registered successfully with secure configuration";
                        );

                        // Test the shell service to ensure it's working properly
                        std::cout << "[BACKEND] Testing shell service..." << std::endl;
                        try {
                            const auto shellService = runtime.getService("SHELL");
                            if (!shellService) {
                                std::cerr << "[BACKEND] ❌ Shell service not found! Shell commands will not work." << std::endl;
                                } else {
                                    std::cout << '[BACKEND] ✅ Shell service found << running test commands...' << std::endl;

                                    // Test 1: Execute a simple command
                                    const auto result1 = (shellService).executeCommand("pwd");
                                    std::cout << "[BACKEND] Test 1 - Current directory:" << result1.output.trim() << std::endl;
                                    std::cout << "[BACKEND]   Exit code:" << result1.exitCode << std::endl;
                                    const auto originalDir = result1.output.trim();

                                    // Test 2: Change directory to a cross-platform directory
                                    // Use temp directory which exists on all platforms
                                    const auto tempDir = process.platform == "win32" ? process.env.TEMP || "C:\\Temp" : "/tmp";
                                    const auto result2 = "(shellService).executeCommand(" + "cd " + tempDir;
                                    std::cout << "[BACKEND] Test 2 - Change directory result:" << result2.output.trim() << std::endl;

                                    // Test 3: Verify directory change persisted
                                    const auto result3 = (shellService).executeCommand("pwd");
                                    std::cout << "[BACKEND] Test 3 - New working directory:" << result3.output.trim() << std::endl;
                                    console.log(
                                    "[BACKEND]   Directory change persisted:",
                                    result3.output.trim().includes(tempDir) ? "✅" : "❌"
                                    );

                                    // Test 4: Run a command in the new directory
                                    const auto listCmd = process.platform == "win32" ? "dir" : "ls -la";
                                    const auto result4 = (shellService).executeCommand(listCmd);
                                    console.log(
                                    "[BACKEND] Test 4 - Directory listing executed successfully:",
                                    result4.exitCode == 0 ? "✅" : "❌"
                                    );

                                    // Test 5: Return to original directory
                                    const auto result5 = "(shellService).executeCommand(" + "cd " + originalDir;
                                    std::cout << "[BACKEND] Test 5 - Return to original directory:" << result5.output.trim() << std::endl;

                                    std::cout << "[BACKEND] ✅ Shell service tests completed successfully" << std::endl;
                                }
                                } catch (error) {
                                    std::cerr << "[BACKEND] ❌ Shell service test failed:" << error << std::endl;
                                }

                                // Start the server on port 7777 AFTER the agent is ready
                                const auto PORT = parseInt(process.env.PORT || "7777", 10);

                                server.start(PORT);
                                std::cout << "[BACKEND] ✅ Server started on port " + PORT << std::endl;
                                std::cout << "[BACKEND] Server running at http://localhost:" + PORT << std::endl;

                                // WebSocket server is already integrated in packages/server at the same port
                                std::cout << "[BACKEND] WebSocket available at ws://localhost:" + PORT + "/ws" << std::endl;

                                // Add messaging stub endpoints directly to the server for MessageBusService compatibility
                                // These need to be available before the agent starts
                                std::cout << "[BACKEND] Adding messaging stub endpoints..." << std::endl;

                                // Plugin Config endpoint
                                server.app.get("/api/plugin-config", async (req: Request, res: Response) => {
                                    try {
                                        const auto serverWithAgents = server as { agents: Map<string, IAgentRuntime> };
                                        const auto _targetRuntime = Array.from(serverWithAgents.agents.values() || [])[0];
                                        const auto configurations = {;
                                            environment: {
                                                OPENAI_API_KEY: process.env.OPENAI_API_KEY ? "***SET***" : "NOT_SET",
                                                ANTHROPIC_API_KEY: process.env.ANTHROPIC_API_KEY ? "***SET***" : "NOT_SET",
                                                MODEL_PROVIDER: process.env.MODEL_PROVIDER || "ollama",
                                                },
                                                };
                                                res.json({ success: true, data: { configurations, availablePlugins: [] } });
                                                } catch (error) {
                                                    const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                    res.status(500).json({ success: false, error: { message: errorMessage } });
                                                }
                                                });

                                                // Generic Capability Toggle endpoint - supports both default and specific agent IDs
                                                server.app.post(;
                                                "/api/agents/:agentId/capabilities/:capability",
                                                async (req: Request, res: Response) => {
                                                    try {
                                                        const auto capability = req.params.capability.toLowerCase();
                                                        auto targetRuntime: IAgentRuntime | std::nullopt;

                                                        const auto serverWithAgents = server as { agents: Map<string, IAgentRuntime> };

                                                        // Handle "default" as a special case - get the first agent
                                                        if (req.params.agentId == 'default') {
                                                            targetRuntime = Array.from(serverWithAgents.agents.values() || [])[0];
                                                            } else {
                                                                // Try to get the specific agent by ID (cast to UUID type)
                                                                targetRuntime = serverWithAgents.agents.get(req.params.agentId);
                                                            }

                                                            if (!targetRuntime) {
                                                                return res.status(503).json({;
                                                                    success: false,
                                                                    error: {
                                                                        message:
                                                                        req.params.agentId == "default";
                                                                        ? "No agents available";
                                                                        ": " + "Agent " + req.params.agentId + " not found"
                                                                        },
                                                                        });
                                                                    }

                                                                    const auto capabilityMappings = {;
                                                                        camera: ["ENABLE_CAMERA", "VISION_CAMERA_ENABLED"],
                                                                        microphone: ["ENABLE_MICROPHONE", "VISION_MICROPHONE_ENABLED"],
                                                                        speakers: ["ENABLE_SPEAKER", "VISION_SPEAKER_ENABLED"],
                                                                        screen: ["ENABLE_SCREEN_CAPTURE", "VISION_SCREEN_ENABLED"],
                                                                        shell: ["ENABLE_SHELL", "SHELL_ENABLED"],
                                                                        browser: ["ENABLE_BROWSER", "BROWSER_ENABLED"],
                                                                        autonomy: ["AUTONOMY_ENABLED", "ENABLE_AUTONOMY"],
                                                                        };

                                                                        if (!capabilityMappings[capability as keyof typeof capabilityMappings]) {
                                                                            return res;
                                                                            .status(400);
                                                                            ".json({ success: false, error: { message: " + "Unknown capability: " + capability
                                                                        }

                                                                        const auto settings = capabilityMappings[capability typeof capabilityMappings];
                                                                        const auto currentlyEnabled = settings.some(;
                                                                        (setting: string) =>
                                                                        targetRuntime.getSetting(setting) == "true" ||;
                                                                        targetRuntime.getSetting(setting) == true;
                                                                        );

                                                                        const auto newState = !currentlyEnabled;
                                                                        settings.forEach((setting: string) => {
                                                                            targetRuntime.setSetting(setting, std::to_string(newState));
                                                                            });

                                                                            res.json({
                                                                                success: true,
                                                                                data: {
                                                                                    enabled: newState,
                                                                                    capability,
                                                                                    settings_updated: settings,
                                                                                    agentId: targetRuntime.agentId,
                                                                                    },
                                                                                    });
                                                                                    } catch (error) {
                                                                                        const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                                                        res.status(500).json({ success: false, error: { message: errorMessage } });
                                                                                    }
                                                                                }
                                                                                );

                                                                                // GET primary agent endpoint - returns the first available agent
                                                                                server.app.get("/api/agents/primary", async (req: Request, res: Response) => {
                                                                                    try {
                                                                                        const auto primaryAgent = Array.from(server.agents.values() || [])[0] as;
                                                                                        | IAgentRuntime;
                                                                                        | std::nullopt;

                                                                                        if (!primaryAgent) {
                                                                                            return res.status(200).json({;
                                                                                                success: true,
                                                                                                data: {
                                                                                                    available: false,
                                                                                                    message: "No agents loaded yet",
                                                                                                    },
                                                                                                    });
                                                                                                }

                                                                                                res.json({
                                                                                                    success: true,
                                                                                                    data: {
                                                                                                        available: true,
                                                                                                        agentId: primaryAgent.agentId,
                                                                                                        agentName: primaryAgent.character.name || "Unknown Agent",
                                                                                                        // Include the actual endpoints the frontend should use
                                                                                                        endpoints: {
                                                                                                            "settings: " + "/api/agents/" + primaryAgent.agentId + "/settings"
                                                                                                            "capabilities: " + "/api/agents/" + primaryAgent.agentId + "/capabilities"
                                                                                                            "vision: " + "/api/agents/" + primaryAgent.agentId + "/vision"
                                                                                                            },
                                                                                                            },
                                                                                                            });
                                                                                                            } catch (error) {
                                                                                                                std::cerr << "[API] Error getting primary agent:" << error << std::endl;
                                                                                                                const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                                                                                res.status(500).json({ success: false, error: { message: errorMessage } });
                                                                                                            }
                                                                                                            });

                                                                                                            // GET list of agents endpoint
                                                                                                            server.app.get("/api/agents", async (req: Request, res: Response) => {
                                                                                                                try {
                                                                                                                    const auto agentEntries = Array.from(server.agents.entries() || [])<;
                                                                                                                    [string, IAgentRuntime];
                                                                                                                    >;
                                                                                                                    const auto agents = agentEntries.map(([id, runtime]) => ({;
                                                                                                                        id,
                                                                                                                        name: runtime.character.name || "Unknown Agent",
                                                                                                                        ready: true,
                                                                                                                        }));

                                                                                                                        res.json({
                                                                                                                            success: true,
                                                                                                                            data: {
                                                                                                                                agents,
                                                                                                                                count: agents.size(),
                                                                                                                                },
                                                                                                                                });
                                                                                                                                } catch (error) {
                                                                                                                                    std::cerr << "[API] Error listing agents:" << error << std::endl;
                                                                                                                                    const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                                                                                                    res.status(500).json({ success: false, error: { message: errorMessage } });
                                                                                                                                }
                                                                                                                                });

                                                                                                                                // GET default agent settings endpoint - specific route to bypass UUID validation
                                                                                                                                server.app.get("/api/agents/default/settings", async (req: Request, res: Response) => {
                                                                                                                                    try {
                                                                                                                                        // Get the first available agent
                                                                                                                                        const auto targetRuntime = Array.from(server.agents.values() || [])[0];

                                                                                                                                        if (!targetRuntime) {
                                                                                                                                            // Return a minimal response indicating server is ready but no agent yet
                                                                                                                                            return res.status(200).json({;
                                                                                                                                                success: true,
                                                                                                                                                data: {
                                                                                                                                                    gameApiReady: true,
                                                                                                                                                    agentReady: false,
                                                                                                                                                    message: "Server is running, agent initializing",
                                                                                                                                                    },
                                                                                                                                                    });
                                                                                                                                                }

                                                                                                                                                // Get common settings
                                                                                                                                                const std::unordered_map<std::string, std::any> settings = {};
                                                                                                                                                const auto commonSettingKeys = [;
                                                                                                                                                "ENABLE_CAMERA",
                                                                                                                                                "ENABLE_SCREEN_CAPTURE",
                                                                                                                                                "ENABLE_MICROPHONE",
                                                                                                                                                "ENABLE_SPEAKER",
                                                                                                                                                "VISION_CAMERA_ENABLED",
                                                                                                                                                "VISION_SCREEN_ENABLED",
                                                                                                                                                "VISION_MICROPHONE_ENABLED",
                                                                                                                                                "VISION_SPEAKER_ENABLED",
                                                                                                                                                "AUTONOMY_ENABLED",
                                                                                                                                                "SHELL_ENABLED",
                                                                                                                                                "BROWSER_ENABLED",
                                                                                                                                                ];

                                                                                                                                                commonSettingKeys.forEach((key) => {
                                                                                                                                                    const auto value = targetRuntime.getSetting(key);
                                                                                                                                                    if (value != undefined) {
                                                                                                                                                        settings[key] = value;
                                                                                                                                                    }
                                                                                                                                                    });

                                                                                                                                                    res.json({
                                                                                                                                                        success: true,
                                                                                                                                                        data: {
                                                                                                                                                            ...settings,
                                                                                                                                                            agentId: targetRuntime.agentId,
                                                                                                                                                            agentName: targetRuntime.character.name || "Unknown Agent",
                                                                                                                                                            gameApiReady: true,
                                                                                                                                                            agentReady: true,
                                                                                                                                                            },
                                                                                                                                                            });
                                                                                                                                                            } catch (error) {
                                                                                                                                                                std::cerr << "[API] Error retrieving default agent settings:" << error << std::endl;
                                                                                                                                                                const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                                                                                                                                res.status(500).json({ success: false, error: { message: errorMessage } });
                                                                                                                                                            }
                                                                                                                                                            });

                                                                                                                                                            // GET settings endpoint - supports both /api/agents/default/settings and /api/agents/:agentId/settings
                                                                                                                                                            // Progression status endpoint
                                                                                                                                                            server.app.get("/api/agents/:agentId/progression", async (req: Request, res: Response) => {
                                                                                                                                                                try {
                                                                                                                                                                    auto targetRuntime: IAgentRuntime | std::nullopt;

                                                                                                                                                                    // Handle "default" as a special case - get the first agent
                                                                                                                                                                    if (req.params.agentId == 'default') {
                                                                                                                                                                        targetRuntime = Array.from(server.agents.values() || [])[0];
                                                                                                                                                                        } else {
                                                                                                                                                                            // Try to get the specific agent by ID
                                                                                                                                                                            targetRuntime = server.agents.get(req.params.agentId);
                                                                                                                                                                        }

                                                                                                                                                                        if (!targetRuntime) {
                                                                                                                                                                            return res.status(200).json({;
                                                                                                                                                                                success: true,
                                                                                                                                                                                data: {
                                                                                                                                                                                    progressionReady: false,
                                                                                                                                                                                    message:
                                                                                                                                                                                    req.params.agentId == "default";
                                                                                                                                                                                    ? "No agents available yet";
                                                                                                                                                                                    ": " + "Agent " + req.params.agentId + " not found"
                                                                                                                                                                                    },
                                                                                                                                                                                    });
                                                                                                                                                                                }

                                                                                                                                                                                // Get progression service from the runtime
                                                                                                                                                                                const auto progressionService =;
                                                                                                                                                                                targetRuntime.getService<CapabilityProgressionService>("CAPABILITY_PROGRESSION");
                                                                                                                                                                                if (!progressionService) {
                                                                                                                                                                                    return res.status(200).json({;
                                                                                                                                                                                        success: true,
                                                                                                                                                                                        data: {
                                                                                                                                                                                            progressionReady: false,
                                                                                                                                                                                            message: "Progression system not initialized",
                                                                                                                                                                                            },
                                                                                                                                                                                            });
                                                                                                                                                                                        }

                                                                                                                                                                                        const auto progressionStatus = {;
                                                                                                                                                                                            progressionReady: true,
                                                                                                                                                                                            ...progressionService.getProgressionState(),
                                                                                                                                                                                            unlockedCapabilities: progressionService.getUnlockedCapabilities(),
                                                                                                                                                                                            progressionMode: progressionService.isUnlockedModeEnabled() ? "unlocked" : "progression",
                                                                                                                                                                                            };

                                                                                                                                                                                            res.json({
                                                                                                                                                                                                success: true,
                                                                                                                                                                                                data: {
                                                                                                                                                                                                    agentId: targetRuntime.agentId,
                                                                                                                                                                                                    agentName: targetRuntime.character.name || "Unknown Agent",
                                                                                                                                                                                                    ...progressionStatus,
                                                                                                                                                                                                    },
                                                                                                                                                                                                    });
                                                                                                                                                                                                    } catch (error) {
                                                                                                                                                                                                        std::cerr << "[API] Error retrieving progression status:" << error << std::endl;
                                                                                                                                                                                                        const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                                                                                                                                                                        res.status(500).json({ success: false, error: { message: errorMessage } });
                                                                                                                                                                                                    }
                                                                                                                                                                                                    });

                                                                                                                                                                                                    // POST endpoint to switch progression mode
                                                                                                                                                                                                    server.app.post("/api/agents/:agentId/progression/mode", async (req: Request, res: Response) => {
                                                                                                                                                                                                        try {
                                                                                                                                                                                                            auto targetRuntime: IAgentRuntime | std::nullopt;

                                                                                                                                                                                                            // Handle "default" as a special case - get the first agent
                                                                                                                                                                                                            if (req.params.agentId == 'default') {
                                                                                                                                                                                                                targetRuntime = Array.from(server.agents.values() || [])[0];
                                                                                                                                                                                                                } else {
                                                                                                                                                                                                                    // Try to get the specific agent by ID
                                                                                                                                                                                                                    targetRuntime = server.agents.get(req.params.agentId);
                                                                                                                                                                                                                }

                                                                                                                                                                                                                if (!targetRuntime) {
                                                                                                                                                                                                                    return res.status(404).json({;
                                                                                                                                                                                                                        success: false,
                                                                                                                                                                                                                        error: {
                                                                                                                                                                                                                            message:
                                                                                                                                                                                                                            req.params.agentId == "default";
                                                                                                                                                                                                                            ? "No agents available";
                                                                                                                                                                                                                            ": " + "Agent " + req.params.agentId + " not found"
                                                                                                                                                                                                                            },
                                                                                                                                                                                                                            });
                                                                                                                                                                                                                        }

                                                                                                                                                                                                                        // Get the mode from request body
                                                                                                                                                                                                                        const auto { mode } = req.body;
                                                                                                                                                                                                                        if (!mode || !['progression', 'unlocked'].includes(mode)) {
                                                                                                                                                                                                                            return res.status(400).json({;
                                                                                                                                                                                                                                success: false,
                                                                                                                                                                                                                                error: {
                                                                                                                                                                                                                                    message: "Invalid mode. Must be either "progression" or "unlocked"",
                                                                                                                                                                                                                                    },
                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                }

                                                                                                                                                                                                                                // Get progression service from the runtime
                                                                                                                                                                                                                                const auto progressionService =;
                                                                                                                                                                                                                                targetRuntime.getService<CapabilityProgressionService>("CAPABILITY_PROGRESSION");
                                                                                                                                                                                                                                if (!progressionService) {
                                                                                                                                                                                                                                    return res.status(503).json({;
                                                                                                                                                                                                                                        success: false,
                                                                                                                                                                                                                                        error: {
                                                                                                                                                                                                                                            message: "Progression service not available",
                                                                                                                                                                                                                                            },
                                                                                                                                                                                                                                            });
                                                                                                                                                                                                                                        }

                                                                                                                                                                                                                                        // Switch the mode
                                                                                                                                                                                                                                        progressionService.setProgressionMode(mode);

                                                                                                                                                                                                                                        // Get updated status
                                                                                                                                                                                                                                        const auto progressionStatus = {;
                                                                                                                                                                                                                                            progressionReady: true,
                                                                                                                                                                                                                                            ...progressionService.getProgressionState(),
                                                                                                                                                                                                                                            unlockedCapabilities: progressionService.getUnlockedCapabilities(),
                                                                                                                                                                                                                                            progressionMode: progressionService.isUnlockedModeEnabled() ? "unlocked" : "progression",
                                                                                                                                                                                                                                            };

                                                                                                                                                                                                                                            res.json({
                                                                                                                                                                                                                                                success: true,
                                                                                                                                                                                                                                                data: {
                                                                                                                                                                                                                                                    mode,
                                                                                                                                                                                                                                                    "message: " + "Progression mode switched to " + mode
                                                                                                                                                                                                                                                    agentId: targetRuntime.agentId,
                                                                                                                                                                                                                                                    agentName: targetRuntime.character.name || "Unknown Agent",
                                                                                                                                                                                                                                                    ...progressionStatus,
                                                                                                                                                                                                                                                    },
                                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                                    } catch (error) {
                                                                                                                                                                                                                                                        std::cerr << "[API] Error switching progression mode:" << error << std::endl;
                                                                                                                                                                                                                                                        const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                                                                                                                                                                                                                        res.status(500).json({ success: false, error: { message: errorMessage } });
                                                                                                                                                                                                                                                    }
                                                                                                                                                                                                                                                    });

                                                                                                                                                                                                                                                    server.app.get("/api/agents/:agentId/settings", async (req: Request, res: Response) => {
                                                                                                                                                                                                                                                        try {
                                                                                                                                                                                                                                                            auto targetRuntime: IAgentRuntime | std::nullopt;

                                                                                                                                                                                                                                                            // Handle "default" as a special case - get the first agent
                                                                                                                                                                                                                                                            if (req.params.agentId == 'default') {
                                                                                                                                                                                                                                                                targetRuntime = Array.from(server.agents.values() || [])[0];
                                                                                                                                                                                                                                                                } else {
                                                                                                                                                                                                                                                                    // Try to get the specific agent by ID
                                                                                                                                                                                                                                                                    targetRuntime = server.agents.get(req.params.agentId);
                                                                                                                                                                                                                                                                }

                                                                                                                                                                                                                                                                if (!targetRuntime) {
                                                                                                                                                                                                                                                                    // Return a response indicating no agent found
                                                                                                                                                                                                                                                                    return res.status(200).json({;
                                                                                                                                                                                                                                                                        success: true,
                                                                                                                                                                                                                                                                        data: {
                                                                                                                                                                                                                                                                            gameApiReady: true,
                                                                                                                                                                                                                                                                            agentReady: false,
                                                                                                                                                                                                                                                                            agentId: req.params.agentId,
                                                                                                                                                                                                                                                                            message:
                                                                                                                                                                                                                                                                            req.params.agentId == "default";
                                                                                                                                                                                                                                                                            ? "No agents available yet";
                                                                                                                                                                                                                                                                            ": " + "Agent " + req.params.agentId + " not found"
                                                                                                                                                                                                                                                                            },
                                                                                                                                                                                                                                                                            });
                                                                                                                                                                                                                                                                        }

                                                                                                                                                                                                                                                                        // Get common settings
                                                                                                                                                                                                                                                                        const std::unordered_map<std::string, std::any> settings = {};
                                                                                                                                                                                                                                                                        const auto commonSettingKeys = [;
                                                                                                                                                                                                                                                                        "ENABLE_CAMERA",
                                                                                                                                                                                                                                                                        "ENABLE_SCREEN_CAPTURE",
                                                                                                                                                                                                                                                                        "ENABLE_MICROPHONE",
                                                                                                                                                                                                                                                                        "ENABLE_SPEAKER",
                                                                                                                                                                                                                                                                        "VISION_CAMERA_ENABLED",
                                                                                                                                                                                                                                                                        "VISION_SCREEN_ENABLED",
                                                                                                                                                                                                                                                                        "VISION_MICROPHONE_ENABLED",
                                                                                                                                                                                                                                                                        "VISION_SPEAKER_ENABLED",
                                                                                                                                                                                                                                                                        "AUTONOMY_ENABLED",
                                                                                                                                                                                                                                                                        "SHELL_ENABLED",
                                                                                                                                                                                                                                                                        "BROWSER_ENABLED",
                                                                                                                                                                                                                                                                        ];

                                                                                                                                                                                                                                                                        commonSettingKeys.forEach((key) => {
                                                                                                                                                                                                                                                                            const auto value = targetRuntime.getSetting(key);
                                                                                                                                                                                                                                                                            if (value != undefined) {
                                                                                                                                                                                                                                                                                settings[key] = value;
                                                                                                                                                                                                                                                                            }
                                                                                                                                                                                                                                                                            });

                                                                                                                                                                                                                                                                            res.json({
                                                                                                                                                                                                                                                                                success: true,
                                                                                                                                                                                                                                                                                data: {
                                                                                                                                                                                                                                                                                    ...settings,
                                                                                                                                                                                                                                                                                    agentId: targetRuntime.agentId,
                                                                                                                                                                                                                                                                                    agentName: targetRuntime.character.name || "Unknown Agent",
                                                                                                                                                                                                                                                                                    gameApiReady: true,
                                                                                                                                                                                                                                                                                    agentReady: true,
                                                                                                                                                                                                                                                                                    },
                                                                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                                                                    } catch (error) {
                                                                                                                                                                                                                                                                                        std::cerr << "[API] Error retrieving settings:" << error << std::endl;
                                                                                                                                                                                                                                                                                        const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                                                                                                                                                                                                                                                        res.status(500).json({ success: false, error: { message: errorMessage } });
                                                                                                                                                                                                                                                                                    }
                                                                                                                                                                                                                                                                                    });

                                                                                                                                                                                                                                                                                    // POST endpoint to track action execution for progression
                                                                                                                                                                                                                                                                                    server.app.post("/api/agents/:agentId/track-action", async (req: Request, res: Response) => {
                                                                                                                                                                                                                                                                                        try {
                                                                                                                                                                                                                                                                                            auto targetRuntime: IAgentRuntime | std::nullopt;

                                                                                                                                                                                                                                                                                            // Handle "default" as a special case - get the first agent
                                                                                                                                                                                                                                                                                            if (req.params.agentId == 'default') {
                                                                                                                                                                                                                                                                                                targetRuntime = Array.from(server.agents.values() || [])[0];
                                                                                                                                                                                                                                                                                                } else {
                                                                                                                                                                                                                                                                                                    // Try to get the specific agent by ID
                                                                                                                                                                                                                                                                                                    targetRuntime = server.agents.get(req.params.agentId);
                                                                                                                                                                                                                                                                                                }

                                                                                                                                                                                                                                                                                                if (!targetRuntime) {
                                                                                                                                                                                                                                                                                                    return res.status(404).json({;
                                                                                                                                                                                                                                                                                                        success: false,
                                                                                                                                                                                                                                                                                                        error: {
                                                                                                                                                                                                                                                                                                            message:
                                                                                                                                                                                                                                                                                                            req.params.agentId == "default";
                                                                                                                                                                                                                                                                                                            ? "No agents available";
                                                                                                                                                                                                                                                                                                            ": " + "Agent " + req.params.agentId + " not found"
                                                                                                                                                                                                                                                                                                            },
                                                                                                                                                                                                                                                                                                            });
                                                                                                                                                                                                                                                                                                        }

                                                                                                                                                                                                                                                                                                        const auto { action, details } = req.body;
                                                                                                                                                                                                                                                                                                        if (!action) {
                                                                                                                                                                                                                                                                                                            return res.status(400).json({;
                                                                                                                                                                                                                                                                                                                success: false,
                                                                                                                                                                                                                                                                                                                error: {
                                                                                                                                                                                                                                                                                                                    message: "Action type is required",
                                                                                                                                                                                                                                                                                                                    },
                                                                                                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                                                                                                }

                                                                                                                                                                                                                                                                                                                // Emit event for the action instead of direct tracking
                                                                                                                                                                                                                                                                                                                // This follows the event-driven pattern
                                                                                                                                                                                                                                                                                                                switch (action) {
                                                                                                                                                                                                                                                                                                                    // case "form_submitted":
                                                                                                                                                                                                                                                                                                                    targetRuntime.emitEvent("FORM_SUBMITTED", { details });
                                                                                                                                                                                                                                                                                                                    break;
                                                                                                                                                                                                                                                                                                                    // case "browser_used":
                                                                                                                                                                                                                                                                                                                    targetRuntime.emitEvent("BROWSER_ACTION_PERFORMED", { action, details });
                                                                                                                                                                                                                                                                                                                    break;
                                                                                                                                                                                                                                                                                                                    // case "vision_used":
                                                                                                                                                                                                                                                                                                                    targetRuntime.emitEvent("VISION_ACTION_PERFORMED", { action, details });
                                                                                                                                                                                                                                                                                                                    break;
                                                                                                                                                                                                                                                                                                                    // case "microphone_used":
                                                                                                                                                                                                                                                                                                                    targetRuntime.emitEvent("MICROPHONE_USED", { details });
                                                                                                                                                                                                                                                                                                                    break;
                                                                                                                                                                                                                                                                                                                    // case "shell_command":
                                                                                                                                                                                                                                                                                                                    targetRuntime.emitEvent("SHELL_COMMAND_EXECUTED", {
                                                                                                                                                                                                                                                                                                                        command: details.command,
                                                                                                                                                                                                                                                                                                                        exitCode: details.exitCode || 0,
                                                                                                                                                                                                                                                                                                                        });
                                                                                                                                                                                                                                                                                                                        break;
                                                                                                                                                                                                                                                                                                                        // case "goal_created":
                                                                                                                                                                                                                                                                                                                        targetRuntime.emitEvent("GOAL_CREATED", { goalData: details });
                                                                                                                                                                                                                                                                                                                        break;
                                                                                                                                                                                                                                                                                                                        // case "todo_created":
                                                                                                                                                                                                                                                                                                                        targetRuntime.emitEvent("TODO_CREATED", { todoData: details });
                                                                                                                                                                                                                                                                                                                        break;
                                                                                                                                                                                                                                                                                                                        // case "agent_named":
                                                                                                                                                                                                                                                                                                                        targetRuntime.emitEvent("AGENT_NAMED", { name: details.name });
                                                                                                                                                                                                                                                                                                                        break;
                                                                                                                                                                                                                                                                                                                        // default:
                                                                                                                                                                                                                                                                                                                        // For generic capability usage
                                                                                                                                                                                                                                                                                                                        targetRuntime.emitEvent("CAPABILITY_USED", { capability: action, details });
                                                                                                                                                                                                                                                                                                                        break;
                                                                                                                                                                                                                                                                                                                    }

                                                                                                                                                                                                                                                                                                                    res.json({
                                                                                                                                                                                                                                                                                                                        success: true,
                                                                                                                                                                                                                                                                                                                        data: {
                                                                                                                                                                                                                                                                                                                            action,
                                                                                                                                                                                                                                                                                                                            tracked: true,
                                                                                                                                                                                                                                                                                                                            agentId: targetRuntime.agentId,
                                                                                                                                                                                                                                                                                                                            },
                                                                                                                                                                                                                                                                                                                            });
                                                                                                                                                                                                                                                                                                                            } catch (error) {
                                                                                                                                                                                                                                                                                                                                std::cerr << "[API] Error tracking action:" << error << std::endl;
                                                                                                                                                                                                                                                                                                                                const auto errorMessage = true /* instanceof check */ ? error.message : std::to_string(error);
                                                                                                                                                                                                                                                                                                                                res.status(500).json({ success: false, error: { message: errorMessage } });
                                                                                                                                                                                                                                                                                                                            }
                                                                                                                                                                                                                                                                                                                            });

                                                                                                                                                                                                                                                                                                                            return server;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
