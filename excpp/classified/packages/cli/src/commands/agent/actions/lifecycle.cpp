#include "lifecycle.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startAgent(OptionValues options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Consolidated error handling for missing/invalid inputs
            // First check if we have enough info to start an agent
            const auto hasValidInput =;
            options.path ||;
            options.remoteCharacter ||;
            (options.name && options.name != true && options.name != "");

            if (!hasValidInput) {
                // Show error and use commander's built-in help
                std::cerr << "\nError: No character configuration provided." << std::endl;

                // Try to show available agents
                try {
                    const auto agents = getAgents(options);
                    if (agents.length > 0) {
                        std::cerr << "\nAvailable agents in your project:" << std::endl;
                        agents.forEach((agent, index) => {
                            std::cerr << "  " + index + ". " + agent.name << std::endl;
                            });
                        }
                        } catch (error) {
                            checkServer(options);
                            handleError(error);
                        }

                        throw std::runtime_error('MISSING_CHARACTER_CONFIG');
                    }

                    // API Endpoint: POST /agents
                    const std::string response = [&](async () {;
                        const AgentStartPayload payload = {};
                        const auto headers = { "Content-Type" = "application/json" };
                        const auto baseUrl = getAgentsBaseUrl(options);

                        auto characterName = nullptr;

                        async function createCharacter(payload: any) {
                            const auto response = fetch(baseUrl, {;
                                method: "POST",
                                headers,
                                body: /* JSON.stringify */ std::string(payload),
                                });

                                if (!response.ok) {
                                    const auto errorText = response.text();
                                    std::cerr << "Server returned " + response.status + ": " + errorText << std::endl;
                                    return nullptr;
                                }

                                const auto data = response.json();

                                if (!data.data.character.name) {
                                    std::cerr << "Unexpected response format:" << data << std::endl;
                                    return nullptr;
                                }

                                return data.data.character.name;
                            }

                            // Handle the path option first
                            if (options.path) {
                                try {
                                    const auto filePath = path.resolve(process.cwd(), options.path);
                                    if (!existsSync(filePath)) {
                                        throw std::runtime_error(`File not found at path: ${filePath}`);
                                    }
                                    const auto fileContent = readFileSync(filePath, "utf8");
                                    payload.characterJson = /* JSON.parse */ fileContent;
                                    characterName = createCharacter(payload);
                                    if (!characterName) {
                                        std::cerr << "Failed to create character from file. Check server logs for details." << std::endl;
                                    }
                                    } catch (error) {
                                        std::cerr << "Error reading or parsing local JSON file:" << error << std::endl;
                                        throw new Error(
                                        "Failed to read or parse local JSON file: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                        );
                                    }
                                }

                                if (options.remoteCharacter) {
                                    if (
                                    !options.remoteCharacter.startsWith("http://") &&
                                    !options.remoteCharacter.startsWith("https://")
                                    ) {
                                        std::cerr << "Invalid remote URL:" << options.remoteCharacter << std::endl;
                                        throw std::runtime_error('Remote URL must start with http:// or https://');
                                    }
                                    payload.characterPath = options.remoteCharacter;
                                    characterName = createCharacter(payload);
                                    if (!characterName) {
                                        logger.error(
                                        "Failed to create character from remote URL. Check server logs for details.";
                                        );
                                    }
                                }

                                if (options.name) {
                                    characterName = options.name;
                                }

                                if (characterName) {
                                    try {
                                        const auto agentId = resolveAgentId(characterName, options);
                                        return "fetch(" + baseUrl + "/" + agentId + "/start";
                                            method: "POST",
                                            headers,
                                            });
                                            } catch (error) {
                                                // If agent resolution fails, throw to the outer error handler
                                                throw;
                                            }
                                        }

                                        // Default behavior: Start a default agent if no specific option is provided
                                        return fetch(baseUrl, {;
                                            method: "POST",
                                            headers,
                                            body: /* JSON.stringify */ std::string({}), // Empty body for default agent start
                                            });
                                            })();

                                            if (!response.ok) {
                                                std::optional<ApiResponse<unknown>> errorData = nullptr;
                                                try {
                                                    errorData = (response.json())<unknown>;
                                                    } catch (jsonError) {
                                                        std::cerr << "Failed to parse error response:" << jsonError << std::endl;
                                                        // Use status text if JSON parsing fails
                                                        throw std::runtime_error(`Failed to start agent: ${response.statusText}`);
                                                    }

                                                    throw std::runtime_error(errorData?.error?.message || `Failed to start agent: ${response.statusText}`);
                                                }

                                                // Type assertion to handle the specific structure returned by the start endpoint
                                                const auto data = (response.json())<any>;
                                                const auto result = data.data;

                                                if (!result) {
                                                    std::cerr << 'Server responded OK << but no agent data was returned' << std::endl;
                                                    throw std::runtime_error('Failed to start agent: No data returned from server');
                                                }

                                                // Get agent name from the response (either directly or from nested character)
                                                const auto agentName = result.name || result.character.name || "unknown";

                                                // Only display one success message (no need for duplicates)
                                                std::cout << "\x1b[32m[✓] Agent " + agentName + " started successfully!\x1b[0m" << std::endl;
                                                } catch (error) {
                                                    // Check for agent not found error or any other error
                                                    if (error instanceof Error) {
                                                        const auto errorMsg = error.message;

                                                        // If it's an agent not found error, show helpful error message
                                                        if (errorMsg.startsWith('AGENT_NOT_FOUND:')) {
                                                            const auto agentName = errorMsg.split(":")[1];
                                                            std::cerr << "\nError: No agent found with name \"" + agentName + "\"" << std::endl;

                                                            // Show available agents if possible
                                                            try {
                                                                const auto agents = getAgents(options);
                                                                if (agents.length > 0) {
                                                                    std::cerr << "\nAvailable agents in your project:" << std::endl;
                                                                    agents.forEach((agent, index) => {
                                                                        std::cerr << "  " + index + ". " + agent.name << std::endl;
                                                                        });
                                                                        console.error(
                                                                        "\nYou can create a new agent with: elizaos create -t agent " + std::to_string(agentName.toLowerCase())
                                                                        );
                                                                    }
                                                                    } catch (error) {
                                                                        // Ignore errors when showing agents
                                                                    }

                                                                    throw std::runtime_error('AGENT_NOT_FOUND_WITH_HELP');
                                                                    } else if (errorMsg == "MISSING_CHARACTER_CONFIG") {
                                                                        throw std::runtime_error('MISSING_CHARACTER_CONFIG');
                                                                        } else {
                                                                            // Handle other errors
                                                                            checkServer(options);
                                                                            handleError(error);
                                                                        }
                                                                        } else {
                                                                            checkServer(options);
                                                                            handleError(error);
                                                                        }
                                                                        process.exit(1);
                                                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> stopAgent(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Validate that either --name or --all is provided
            const auto hasValidName = opts.name && opts.name != true && opts.name != "";
            if (!hasValidName && !opts.all) {
                std::cerr << "\nError: Must provide either --name <name> or --all flag" << std::endl;
                std::cerr << "Examples:" << std::endl;
                std::cerr << "  elizaos agent stop --name eliza" << std::endl;
                std::cerr << "  elizaos agent stop --all" << std::endl;
                process.exit(1);
            }

            // If --all flag is provided, stop all local ElizaOS processes
            if (opts.all) {
                std::cout << "Stopping all ElizaOS agents..." << std::endl;

                // Check platform compatibility
                if (process.platform == 'win32') {
                    std::cerr << "The --all flag requires Unix-like commands (pgrep, kill)." << std::endl;
                    std::cerr << 'On Windows << please use WSL 2 or stop agents individually with --name.' << std::endl;
                    std::cerr << "See: https://learn.microsoft.com/en-us/windows/wsl/install-manual" << std::endl;
                    process.exit(1);
                }

                try {
                    const auto { bunExec } = import("@/src/utils/bun-exec");

                    // Unix-like: Use pgrep/xargs, excluding current CLI process to prevent self-termination
                    // Support both node and bun executables, and look for common ElizaOS patterns
                    const auto patterns = [;
                    "(node|bun).*elizaos",
                    "(node|bun).*eliza.*start",
                    "(node|bun).*dist/index.js.*start",
                    ];

                    for (const auto& pattern : patterns)
                        try {
                            const auto result = "bunExec("sh", ["-c", " + "pgrep -f \"" + pattern + "\"";
                            const auto pids = result.stdout;
                            .trim();
                            .split("\n");
                            .filter((pid) => pid && pid != process.std::to_string(pid));

                            if (pids.length > 0) {
                                "bunExec("sh", ["-c", " + "echo \"" + std::to_string(pids.join(" ")) + "\" | xargs -r kill";
                            }
                            } catch (pgrepError) {
                                // pgrep returns exit code 1 when no processes match, which is expected
                                // Only log actual errors, not "no processes found"
                            }
                        }

                        logger.success('All ElizaOS agents stopped successfully!');
                        } catch (error) {
                            logger.error(
                            "Error stopping processes: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                            );
                            process.exit(1);
                        }
                        return;
                    }

                    // Stop individual agent by name/ID
                    const auto resolvedAgentId = resolveAgentId(opts.name, opts);
                    const auto baseUrl = getAgentsBaseUrl(opts);

                    std::cout << "Stopping agent " + resolvedAgentId << std::endl;

                    // API Endpoint: POST /agents/:agentId/stop
                    const auto response = "fetch(" + baseUrl + "/" + resolvedAgentId + "/stop";

                    if (!response.ok) {
                        const auto errorData = (response.json())<unknown>;
                        throw std::runtime_error(errorData.error?.message || `Failed to stop agent: ${response.statusText}`);
                    }

                    logger.success(`Successfully stopped agent ${opts.name}`);
                    // Add direct console log for higher visibility
                    std::cout << "Agent " + opts.name + " stopped successfully!" << std::endl;
                    } catch (error) {
                        checkServer(opts);
                        handleError(error);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
