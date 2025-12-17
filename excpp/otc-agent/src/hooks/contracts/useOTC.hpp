#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Helper to get OTC address from deployments or env - cached at module level

Address getOtcAddress();

// Configuration for contract reading with dynamic ABIs
struct ReadContractConfig {
    Address address;
    Abi abi;
    std::string functionName;
    std::optional<std::vector<readonly unknown>> args;
};

// Log type for transaction receipts with topics
struct TransactionLog {
    Address address;
    bigint blockNumber;
    double logIndex;
    double transactionIndex;
    bool removed;
};

// Type-safe wrapper for readContract that handles wagmi client and dynamic ABIs
// The client type is inferred, we only need to specify the return type
// Uses type assertion to bypass viem's strict authorizationList requirement

void useOTC(); {
  // OTC address from env vars or deployment config

  // Use wagmi's public client which automatically handles all configured chains

  // Track previous data to only log when actually changed

    // Only log if data actually changed

  // Using type assertion to avoid deep type instantiation issue

  // Type assertion to avoid deep type instantiation with wagmi's complex generics
  // wagmi's useReadContracts has deeply nested generics that cause TS2589
  // eslint-disable-next-line @typescript-eslint/no-explicit-any

  // Using type assertion to avoid deep type instantiation issue

  // Type assertion to avoid deep type instantiation with wagmi's complex generics
  // eslint-disable-next-line @typescript-eslint/no-explicit-any

  // Wrapper to handle writeContractAsync with dynamic ABIs
  // wagmi's types require chain/account which are inferred from context

      // Compute the contract tokenId (keccak256 of the symbol)
      // The symbol is passed directly from the selected token, no DB lookup needed

      // Notify caller that tx was submitted (before waiting for receipt)
      // This allows UI to update immediately with tx hash

      // Wait for transaction receipt and parse the consignmentId from the event
      // Use manual polling to avoid RPC timeout/rate limit issues

      // Quick poll for receipt - if not found fast, use fallback ID
      // Backend can resolve actual consignmentId later via tx hash

          // Receipt not found yet

      // If we got a receipt, parse the consignment ID from the event

        // Find ConsignmentCreated event from the OTC contract

      // Fallback: use a hash of the tx hash as the consignment ID
      // This allows the flow to continue even if RPC is rate limited
      // The actual consignment ID can be looked up later from the tx

      // Warn if there might be a network mismatch

  // Helper to extract contract address from tokenId format: "token-{chain}-{address}"
      // Format is: token-chain-address, so join everything after the second dash
    // Fallback: assume it's already an address

      // Simply extract the contract address from the tokenId
      // The tokenId format is "token-{chain}-{contractAddress}"
      return extractContractAddress(tokenId);

  // Helper to get exact required payment amount

  // Check if emergency refunds are enabled

  // Track previous offers data to only log on actual changes

      // Contract returns array: [consignmentId, tokenId, beneficiary, tokenAmount, discountBps, createdAt, unlockTime,
      //   priceUsdPerToken, maxPriceDeviation, ethUsdPrice, currency, approved, paid, fulfilled, cancelled, payer, amountPaid]

    // Only log when offers data actually changes


} // namespace elizaos
