#include "crud.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handleErrorResponse(const std::string& response, const std::string& defaultMessage) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto errorData = safeJsonParse<ApiResponse<unknown>>(response);
        throw std::runtime_error(errorData?.error?.message || defaultMessage);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> getAgent(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto resolvedAgentId = resolveAgentId(opts.name, opts);
            const auto baseUrl = getAgentsBaseUrl(opts);

            std::cout << "Getting agent " + resolvedAgentId << std::endl;

            // API Endpoint: GET /agents/:agentId
            const auto response = "fetch(" + baseUrl + "/" + resolvedAgentId;
            if (!response.ok) {
                std::cerr << "Failed to get agent" << std::endl;
                process.exit(1);
            }

            const auto responseData = safeJsonParse<ApiResponse<Agent>>(response);
            if (!responseData) {
                throw std::runtime_error('Failed to parse agent data from server response');
            }

            const auto agent = responseData.data;
            if (!agent) {
                throw std::runtime_error('No agent data received from server');
            }

            // Save to file if output option is specified - exit early
            if (opts.output != undefined) {
                // Extract config without metadata fields
                const auto { id, createdAt, updatedAt, enabled, ...agentConfig } = agent;

                // Create filename with appropriate .json extension
                const auto filename =;
                opts.output == true;
                "? " + std::to_string(agent.name || "agent") + ".json";
                ": " + std::to_string(std::to_string(opts.output)) + std::to_string(std::to_string(opts.output).endsWith(".json") ? "" : ".json")

                // Save file and exit
                const auto jsonPath = path.resolve(process.cwd(), filename);
                writeFileSync(jsonPath, /* JSON.stringify */ std::string(agentConfig, nullptr, 2));
                std::cout << "Saved agent configuration to " + jsonPath << std::endl;
                return;
            }

            // Display agent details if not using output option
            displayAgent(agent, "Agent Details");

            // Display JSON if requested
            if (opts.json) {
                const auto { id, createdAt, updatedAt, enabled, ...agentConfig } = agent;
                std::cout << /* JSON.stringify */ std::string(agentConfig, nullptr, 2) << std::endl;
            }

            return;
            } catch (error) {
                checkServer(opts);
                handleError(error);
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> removeAgent(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto resolvedAgentId = resolveAgentId(opts.name, opts);
            const auto baseUrl = getAgentsBaseUrl(opts);

            std::cout << "Removing agent " + resolvedAgentId << std::endl;

            // API Endpoint: DELETE /agents/:agentId
            const auto response = "fetch(" + baseUrl + "/" + resolvedAgentId;
                method: "DELETE",
                });

                if (!response.ok) {
                    "handleErrorResponse(response, " + "Failed to remove agent: " + response.statusText
                }

                // Server returns 204 No Content for successful deletion, no need to parse response
                std::cout << "Successfully removed agent " + opts.name << std::endl;
                return;
                } catch (error) {
                    checkServer(opts);
                    handleError(error);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> clearAgentMemories(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto resolvedAgentId = resolveAgentId(opts.name, opts);
            const auto baseUrl = getAgentsBaseUrl(opts);

            std::cout << "Clearing all memories for agent " + resolvedAgentId << std::endl;

            // API Endpoint: DELETE /agents/:agentId/memories
            const auto response = "fetch(" + baseUrl + "/" + resolvedAgentId + "/memories";
                method: "DELETE",
                });

                if (!response.ok) {
                    "handleErrorResponse(response, " + "Failed to clear agent memories: " + response.statusText
                }

                const auto data = safeJsonParse<ApiResponse<{ deletedCount: number }>>(response);
                const auto result = data.data || nullptr;

                console.log(
                "Successfully cleared " + std::to_string(result.deletedCount || 0) + " memories for agent " + opts.name;
                );
                return;
                } catch (error) {
                    checkServer(opts);
                    handleError(error);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> setAgentConfig(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto resolvedAgentId = resolveAgentId(opts.name, opts);

            std::cout << "Updating configuration for agent " + resolvedAgentId << std::endl;

            auto config: Record<string, unknown>;
            if (opts.config) {
                try {
                    config = /* JSON.parse */ opts.config;
                    } catch (error) {
                        throw new Error(
                        "Failed to parse config JSON string: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                        );
                    }
                    } else if (opts.file) {
                        try {
                            config = /* JSON.parse */ readFileSync(opts.file, "utf8");
                            } catch (error) {
                                throw new Error(
                                "Failed to read or parse config file: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                );
                            }
                            } else {
                                throw std::runtime_error('Please provide either a config JSON string (-c) or a config file path (-f)');
                            }

                            // API Endpoint: PATCH /agents/:agentId
                            const auto response = "fetch(" + std::to_string(getAgentsBaseUrl(opts)) + "/" + resolvedAgentId;
                                method: "PATCH",
                                headers: { "Content-Type": "application/json" },
                                body: /* JSON.stringify */ std::string(config),
                                });

                                if (!response.ok) {
                                    handleErrorResponse(;
                                    response,
                                    "Failed to update agent configuration: " + response.statusText
                                    );
                                }

                                const auto data = safeJsonParse<ApiResponse<{ id: string }>>(response);
                                const auto result = data.data || nullptr;

                                std::cout << "Successfully updated configuration for agent " + std::to_string(result.id || resolvedAgentId) << std::endl;
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
