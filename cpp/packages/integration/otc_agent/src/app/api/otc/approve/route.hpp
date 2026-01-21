#pragma once
#include <algorithm>
#include <any>
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



// Helper to safely read from contract bypassing viem's strict authorizationList requirement
using ReadContractFn = std::function<std::future<std::any>(std::any)>;

std::future<void> POST(NextRequest request);;

    // Auto-fulfill (backend pays)

    // Token mint comes from the offer itself (multi-token support)

      // Pay with SOL
      // Pay with USDC

  // Resolve approver account: prefer PK; else use testWalletPrivateKey from deployment; else impersonate
  // eslint-disable-next-line @typescript-eslint/no-explicit-any

      // Impersonate approver on Anvil

  // Ensure single approver mode (dev convenience)

  // Poll for offer to exist (tx might still be pending)
  // This handles the case where frontend calls us immediately after tx submission

    // Check if offer exists (beneficiary is set when offer is created)

  // Validate that the offer price hasn't diverged too much from market price
  // This prevents abuse from stale quotes or manipulated pool prices

  // Skip price validation for local development (Anvil has mock prices)

    // Get token info from the offer
    // offer.priceUsdPerToken is in 8 decimals (Chainlink format)

    // Find the specific token associated with this offer
    // Primary method: Use the on-chain tokenId (keccak256 hash of symbol) to look up token

    // The offer.tokenId is a bytes32 (keccak256 of token symbol)

    // Fallback: Try to find via quote (if we have a matching quote by beneficiary)

    // Last resort fallback: Use first active Base token (for backwards compatibility)

        // Reject if divergence exceeds threshold (skip on local network)
    // Log but don't block - fail open for price validation errors

  // Approve immediately

  // Update quote status and financial data if we can find it

      // Calculate financial values from on-chain offer data

      // totalUsd = (tokenAmount * priceUsdPerToken) / 1e18 (result in 8 decimals)

      // discountUsd = totalUsd * discountBps / 10000

      // discountedUsd = totalUsd - discountUsd

      // Determine payment currency and amount based on offer currency

        // Calculate required ETH
        // USDC

      // Update quote status

      // Update quote with financial data from contract

  // If still not approved (multi-approver deployments), escalate approvals

    // Load known approver and agent from deployment file

      // Re-read state after each attempt

  // Final verification that offer was approved

  // Check if approver should also fulfill

  // If approver-only fulfill is enabled, backend pays immediately after approval

      // Calculate required payment

        // ETH payment required

        // USDC payment - need to approve first

        // Approve USDC

      // Fulfill offer

      // Check if offer got paid by another transaction during our attempt

        // Offer is approved but not paid - this is a real error

  // Return success

} // namespace elizaos
