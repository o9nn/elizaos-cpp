#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "services/cdp.service.hpp"
#include "types.hpp"
#include "utils/coingecko.hpp"
#include "utils/entity.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SwapParams {
    CdpNetwork network;
    std::string fromToken;
    std::string toToken;
    std::optional<std::string> amount;
    std::optional<double> percentage;
    std::optional<double> slippageBps;
};

/**
 * Native token placeholder address for CDP swaps
 * CDP SDK uses this special address to represent native gas tokens (ETH, MATIC, etc.)
 * The SDK internally handles the native token  no need to convert to wrapped versions
 * 
 * Reference: https://docs.cdp.coinbase.com/server-wallets/v2/evm-features/swaps
 */

/**
 * Wrapped token addresses for when users explicitly want wrapped tokens
 * (as opposed to native gas tokens)
 * 
 * Addresses verified from CoinGecko and official block explorers:
 * - WETH on Ethereum: Standard WETH9 contract
 * - WETH on Base/Optimism: 0x4200...0006 (OP Stack standard)
 * - WETH on Arbitrum: Native WETH on Arbitrum One
 * - WETH on Polygon: Bridged from Ethereum via PoS Bridge
 * - WMATIC on Polygon: Wrapped MATIC
 */

/**
 * Resolve token to address using CoinGecko
 * Handles both symbols and addresses
 * 
 * IMPORTANT: CDP SDK supports native gas tokens using a special placeholder address.
 * - Native tokens (ETH, MATIC): Use 0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE
 * - Wrapped tokens (WETH, WMATIC): Use actual contract addresses
 * 
 * Always validates addresses with CoinGecko to prevent fake/invalid addresses.
 * The LLM may generate addresses that look valid but don't exist.
 * This function ensures only real, verified tokens are used in swaps.
 * 
 * Reference: https://docs.cdp.coinbase.com/server-wallets/v2/evm-features/swaps
 */
  
  // For native ETH - CDP uses special native token address
  // EXCEPTION: On Polygon, ETH refers to WETH (bridged ETH), not the native gas token
  
  // For explicit WETH - use actual WETH contract address
  
  // For native MATIC/POL on Polygon - use native token address
  // Note: POL exists as ERC20 on Ethereum mainnet, but is NOT a native gas token there
  // POL on Ethereum would fall through to CoinGecko resolution (ERC20 contract address)
  
  // For explicit WMATIC on Polygon - use actual WMATIC contract address
  
  // If it looks like an address, validate it with CoinGecko to prevent fake addresses
  
  // Try to resolve symbol to address via CoinGecko

/**
 * Note: CDP swaps require Permit2 token approval before execution.
 * 
 * The CDP service handles this in two steps:
 * 1. Approve the token for Permit2 contract (0x000000000022D473030F116dDEE9F6B43aC78BA3)
 * 2. Execute the swap using account.swap()
 * 
 * Permit2 is a token approval contract that provides a secure way to manage
 * ERC20 token approvals for swaps across different protocols.
 * 
 * Reference: https://docs.cdp.coinbase.com/trade-api/quickstart#3-execute-a-swap
 */

  // Parameter schema for tool calling
  
      // Check if services are available

      // Ensure the user has a wallet saved

      // Read parameters from state (extracted by multiStepDecisionTemplate)

      // Validate required parameters

      // Validate that we have either amount OR percentage

      // Parse swap parameters with defaults

        // Validate percentage is between 0 and 100

      // Store input parameters for return

      // Resolve token symbols to addresses using CoinGecko

      // Prevent 100% swaps of native tokens (need to keep some for gas)

      // Get decimals for the source token from CoinGecko

      // Helper functions for unit conversion
        return BigInt(integer + paddedFractional);

      // Determine the amount to swap (either specific amount or percentage of balance)
      
        // Percentage-based swap - fetch wallet info to get token balance
        
        // For 100% swaps, fetch actual on-chain balance to avoid rounding errors
          
          // Get actual on-chain balance directly from blockchain

          // For 100% swaps, use the actual on-chain balance
          // Leave a small buffer (0.1%) to account for any potential gas/fee requirements
          // This prevents "insufficient balance" errors due to rounding
          
          // Ensure we don't exceed the actual balance

          // Get token decimals for formatting
          
          // Get wallet info for USD value calculation

          // For non-100% swaps, use cached wallet info (more efficient)
          
          // Find the token in wallet (matching both symbol and address)
            // Check if token matches by address
            // Check if token matches by symbol

          // Get actual balance in base units (wei) to avoid floating point rounding errors
          
          // Calculate percentage: (balance * percentage) / 100
          // Use BigInt arithmetic to avoid rounding errors
          
          // Cap at actual balance to prevent exceeding balance due to any rounding
          
          // Convert to human-readable format for display
          
          // Calculate USD value from already-fetched wallet token data (no extra fetch)
          
        // Specific amount provided - parse to wei

      // Execute the swap using CDP service

      // Try to capture input params even in failure


} // namespace elizaos
