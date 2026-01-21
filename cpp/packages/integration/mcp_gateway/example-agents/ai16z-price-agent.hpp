#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * AI16Z Price Agent
 * 
 * This agent uses the Anthropic SDK to interact with the MCP Gateway
 * and fetch the current price of AI16Z token using CoinGecko MCP tools.
 * 
 * This version uses stdio transport with a free gateway configuration.
 * For paid x402 integration, see ai16z-price-agent-sse.ts
 * 
 * Reference: https://docs.anthropic.com/en/docs/build-with-claude/mcp
 * 
 * Usage:
 *   ANTHROPIC_API_KEY=your_key bun run ai16z-price-agent.ts
 */

// Default wallet for testing (DO NOT use in production with real funds)

/**
 * Main agent function
 */
std::future<void> main();

// Handle graceful shutdown

// Run the agent
} // namespace elizaos
