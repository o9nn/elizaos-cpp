#include "ai16z-price-agent-should-fail.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto mcpClient: Awaited<ReturnType<typeof createMCPClient>> | std::nullopt;

    try {
        // Path to the gateway config and entry point
        const auto gatewayRoot = join(__dirname, "..");
        const auto gatewayEntry = join(gatewayRoot, "src", "index.ts");
        const auto configPath = join(gatewayRoot, "examples", "coingecko-config.yaml");

        std::cout << "[Agent] Initializing MCP Gateway connection..." << std::endl;
        std::cout << "[Agent] Gateway: " + gatewayEntry << std::endl;
        std::cout << "[Agent] Config: " + configPath << std::endl;

        // Create MCP client using AI SDK's built-in support
        // This uses stdio transport to connect to the gateway as a subprocess
        // Reference: https://ai-sdk.dev/docs/ai-sdk-core/mcp-tools
        mcpClient = createMCPClient({
            transport: new StdioClientTransport({
                command: "bun",
                "args: ["run", gatewayEntry, " + "--config=" + configPath
                });
                });

                std::cout << "[Agent] Connected to MCP Gateway" << std::endl;

                // Get all tools from the MCP server (with schema discovery)
                // The AI SDK automatically converts MCP tools to AI SDK tools
                const auto mcpTools = mcpClient.tools();

                std::cout << "[Agent] Available MCP tools:" << Object.keys(mcpTools) << std::endl;

                // Transform tool names to replace colons with underscores (OpenAI requirement)
                // OpenAI tool names must match: ^[a-zA-Z0-9_-]+$
                const std::unordered_map<std::string, typeof mcpTools[keyof typeof mcpTools]> tools = {};
                for (const int [name, tool] of Object.entries(mcpTools)) {
                    const auto transformedName = name.replace(/:/g, "_");
                    tools[transformedName] = tool;
                }

                std::cout << "[Agent] Transformed tool names:" << Object.keys(tools) << std::endl;

                // Create the AI agent with MCP tools
                const auto agent = new Agent({;
                    model: openai("gpt-4o"),
                    "system: "

                    Your task is to fetch the current price of the AI16Z token (also known).;

                    Available tools provide access to cryptocurrency information through CoinGecko.;

                    Steps to follow:
                    1. Search for the AI16Z token using available tools;
                    2. Get its current price in USD;
                    3. Provide a clear, concise response with the price;

                    "Be specific and accurate. The token might be listed as \"ai16z\", \"AI16Z\", or similar variations."
                    tools,
                    stopWhen: stepCountIs(10) // Allow up to 10 steps for tool calls
                    });

                    std::cout << "\n[Agent] Starting AI16Z price query...\n" << std::endl;

                    // Run the agent
                    const auto result = agent.generate({;
                        prompt: "What is the current price of AI16Z token in USD?"
                        });

                        std::cout << "\n[Agent] Result:\n" << std::endl;
                        std::cout << result.text << std::endl;
                        std::cout << "\n[Agent] Done!" << std::endl;

                        } catch (error) {
                            std::cerr << "[Agent] Error:" << error << std::endl;
                            process.exit(1);
                            } finally {
                                // Ensure cleanup
                                if (mcpClient) {
                                    try {
                                        mcpClient.close();
                                        std::cout << "[Agent] MCP client closed" << std::endl;
                                        } catch (closeError) {
                                            std::cerr << "[Agent] Error closing MCP client:" << closeError << std::endl;
                                        }
                                    }
                                }

}

} // namespace elizaos
