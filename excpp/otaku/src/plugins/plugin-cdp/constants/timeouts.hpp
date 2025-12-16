#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

import type { PublicClient } from "viem";
;

/**
 * Transaction timeout constants for CDP plugin
 * All timeouts in milliseconds
 */

/** Standard timeout for transaction confirmation (20 seconds) */
const TX_CONFIRMATION_TIMEOUT = 20_000;

/** Extended timeout for bridge operations (2 minutes / 120 seconds) */
const BRIDGE_CONFIRMATION_TIMEOUT = 120_000;

/** Bridge status polling interval (2 seconds) */
const BRIDGE_POLL_INTERVAL = 2000;

/** Maximum bridge polling attempts (60 × 2s = 2 minutes) */
const BRIDGE_MAX_POLL_ATTEMPTS = BRIDGE_CONFIRMATION_TIMEOUT / BRIDGE_POLL_INTERVAL;

/**
 * Wait for transaction confirmation and verify it succeeded
 * @param publicClient Viem public client
 * @param hash Transaction hash
 * @param operationType Type of operation for error messages (e.g., "swap", "transfer", "NFT transfer")
 * @param timeout Timeout in milliseconds (defaults to TX_CONFIRMATION_TIMEOUT)
 * @throws Error if transaction reverts or times out
 */
std::future<void> waitForTxConfirmation(PublicClient publicClient, `0x${string}` hash, string = "transaction" operationType, number = TX_CONFIRMATION_TIMEOUT timeout); confirmation...`);
  
  const receipt = await publicClient.waitForTransactionReceipt({ 
    hash,
    timeout,
  });
  
  if (receipt.status !== 'success') {
    throw new Error(
      `${operationType.charAt(0).toUpperCase() + operationType.slice(1)} transaction reverted on-chain. ` +
      `The ${operationType} failed - likely due to insufficient balance, slippage, or price impact.`
    );
  }
  
  logger.info(`[Transaction Confirmation] ${operationType} confirmed successfully in block ${receipt.blockNumber}`);
}


} // namespace elizaos
