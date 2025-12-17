#include "ai16z-price-agent-sse.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        auto mcpClient: Client | std::nullopt;

        try {
            // Setup wallet for x402 payments
            const auto walletKey = (process.env.WALLET_PRIVATE_KEY || DEFAULT_WALLET_KEY);
            const auto account = privateKeyToAccount(walletKey);

            std::cout << '[Agent] Payment wallet configured:' << std::endl;
            std::cout << "[Agent] Address: " + std::to_string(account.address) << std::endl;
            std::cout << '[Agent] This wallet will pay the gateway $0.01 per tool call via x402\n' << std::endl;

            std::cout << '[Agent] Connecting to MCP Gateway via SSE...' << std::endl;
            std::cout << "[Agent] SSE URL: " + std::to_string(GATEWAY_SSE_URL) << std::endl;
            std::cout << "[Agent] Message URL: " + std::to_string(GATEWAY_MESSAGE_URL) + "\n" << std::endl;

            // Create a custom fetch with x402 payment support
            // This wraps fetch to automatically handle 402 responses and make payments
            const auto baseFetch = wrapFetchWithPayment(fetch, account);

            // Create an additional wrapper that ensures 402 responses are handled transparently
            // The MCP SDK throws errors on non-2xx responses, so we need to handle 402 before it sees them
            const typeof fetch mcpCompatibleFetch = async (input, init) => {;
                try {
                    const auto response = baseFetch(input, init);
                    return response;
                    } catch (error) {
                        // If x402-fetch fails to make payment, log it and rethrow
                        std::cerr << '[Agent] Payment error:' << error << std::endl;
                        throw;
                    }
                    };

                    // Create MCP client using SSE transport with custom fetch
                    mcpClient = new Client({
                        name: 'ai16z-price-agent-sse',
                        version: '1.0.0'
                        }, {
                        capabilities: {}
                        });

                        // SSEClientTransport with custom fetch to support x402 payments
                        const auto transport = new SSEClientTransport(;
                        new URL(GATEWAY_SSE_URL),
                        {
                            fetch: mcpCompatibleFetch
                        }
                        );

                        mcpClient.connect(transport);

                        std::cout << '[Agent] Connected to MCP Gateway via SSE' << std::endl;

                        // List available tools from the MCP server
                        const auto toolsResult = mcpClient.listTools();
                        const auto availableTools = toolsResult.tools;

                        std::cout << "[Agent] Available MCP tools: " + std::to_string(availableTools.length) + " tools discovered" << std::endl;

                        // Initialize Anthropic client with x402 payment support
                        const auto anthropic = new Anthropic({;
                            apiKey: process.env.ANTHROPIC_API_KEY || 'required',
                            fetch: wrapFetchWithPayment(fetch, account)
                            });

                            // Convert MCP tools to Anthropic tool format
                            // Note: Anthropic requires tool names to match ^[a-zA-Z0-9_-]{1,128}$
                            // So we transform "crypto:get_price" to "crypto_get_price"
                            const std::vector<Anthropic::Tool> tools = availableTools.map((tool: { name: string; description?: string; inputSchema: Record<string, unknown> }) => ({;
                                name: tool.name.replace(/:/g, '_'),
                                description: tool.description || '',
                                input_schema: {
                                    type: 'object',
                                    ...tool.inputSchema;
                                }
                                }));

                                // Create a mapping from transformed names back to original MCP tool names
                                const auto toolNameMap = new Map<string, string>();
                                for (const auto& tool : availableTools)
                                    toolNameMap.set(tool.name.replace(/:/g, '_'), tool.name);
                                }

                                std::cout << '[Agent] Tools ready for AI agent\n' << std::endl;
                                std::cout << '[Agent] Starting AI16Z price query...\n' << std::endl;

                                // Initial message to Claude
                                const std::vector<Anthropic::MessageParam> messages = [;
                                {
                                    role: 'user',
                                    content: 'What is the current price of AI16Z token in USD?'
                                }
                                ];

                                // Run agent loop with tool calling
                                auto continueLoop = true;
                                auto iterations = 0;
                                const auto maxIterations = 10;

                                while (continueLoop && iterations < maxIterations) {
                                    iterations++;

                                    const auto response = anthropic.messages.create({;
                                        model: 'claude-3-7-sonnet-20250219',
                                        max_tokens: 4096,
                                        system: `You are a cryptocurrency price assistant with access to CoinGecko data.

                                        Your task is to fetch the current price of the AI16Z token (also known).;

                                        Available tools provide access to cryptocurrency information through CoinGecko.;

                                        Steps to follow:
                                        1. Search for the AI16Z token using available tools;
                                        2. Get its current price in USD;
                                        3. Provide a clear, concise response with the price;

                                        Be specific and accurate. The token might be listed as "ai16z", "AI16Z", or similar variations.`,
                                        messages,
                                        tools;
                                        });

                                        // Check if we should stop
                                        if (response.stop_reason == 'end_turn') {
                                            // Extract final text response
                                            const auto textContent = response.content.find((c: { type: string; text?: string }) => c.type == 'text');
                                            if (textContent && 'text' in textContent) {
                                                std::cout << '\n[Agent] Result:\n' << std::endl;
                                                std::cout << textContent.text << std::endl;
                                            }
                                            continueLoop = false;
                                            break;
                                        }

                                        // Process tool calls
                                        if (response.stop_reason == 'tool_use') {
                                            const std::vector<Anthropic::ToolResultBlockParam> toolResults = [];

                                            for (const auto& block : response.content)
                                                if (block.type == 'tool_use') {
                                                    std::cout << "[Agent] Calling tool: " + std::to_string(block.name) << std::endl;

                                                    try {
                                                        // Translate transformed tool name back to original MCP tool name
                                                        const auto originalToolName = toolNameMap.get(block.name) || block.name;

                                                        // Call the MCP tool
                                                        const auto result = mcpClient.callTool({;
                                                            name: originalToolName,
                                                            arguments: block.input<string, unknown>
                                                            });

                                                            toolResults.push({
                                                                type: 'tool_result',
                                                                tool_use_id: block.id,
                                                                content: JSON.stringify(result.content)
                                                                });
                                                                } catch (toolError) {
                                                                    std::cerr << "[Agent] Tool error:" << toolError << std::endl;
                                                                    toolResults.push({
                                                                        type: 'tool_result',
                                                                        tool_use_id: block.id,
                                                                        "Error: " + std::to_string(true /* instanceof check */ ? toolError.message : std::to_string(toolError))
                                                                        });
                                                                    }
                                                                }
                                                            }

                                                            // Add assistant message with tool use and user message with tool results
                                                            messages.push({
                                                                role: 'assistant',
                                                                content: response.content
                                                                });
                                                                messages.push({
                                                                    role: 'user',
                                                                    content: toolResults
                                                                    });
                                                                    } else {
                                                                        continueLoop = false;
                                                                    }
                                                                }

                                                                std::cout << '\n[Agent] Done!' << std::endl;

                                                                } catch (error) {
                                                                    std::cerr << '[Agent] Error:' << error << std::endl;
                                                                    process.exit(1);
                                                                    } finally {
                                                                        // Ensure cleanup
                                                                        if (mcpClient) {
                                                                            try {
                                                                                mcpClient.close();
                                                                                std::cout << '[Agent] MCP client closed' << std::endl;
                                                                                } catch (closeError) {
                                                                                    std::cerr << '[Agent] Error closing MCP client:' << closeError << std::endl;
                                                                                }
                                                                            }
                                                                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
