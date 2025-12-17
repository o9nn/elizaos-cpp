#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * AI16Z Price Agent (SSE Mode with x402 Payments)
 * 
 * This agent connects to the MCP Gateway via SSE transport and pays
 * $0.01 per tool call using x402 payments.
 * 
 * Architecture:
 *   Agent (x402 wallet) → Gateway (SSE) → CoinGecko (free)
 * 
 * Prerequisites:
 *   1. Start gateway in SSE mode: ./run-gateway-sse.sh
 *   2. Fund wallet with testnet USDC on Base Sepolia
 * 
 * Usage:
 *   ANTHROPIC_API_KEY=your_key bun run ai16z-price-agent-sse.ts
 */

// Default wallet for testing (DO NOT use in production with real funds)

// Gateway SSE endpoints (must match run-gateway-sse.sh)

/**
 * Main agent function
 */
std::future<void> main();

// Handle graceful shutdown

// Run the agent

} // namespace elizaos
