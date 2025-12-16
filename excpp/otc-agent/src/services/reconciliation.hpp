#include "database.hpp"
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

/**
 * State Reconciliation Service
 *
 * Ensures database state matches blockchain contract state.
 * Critical for maintaining data integrity across the system.
 */

// Minimal client interface to avoid viem's "excessively deep" type issues
struct SimplePublicClient {
    (params: { readContract;
    Address address;
    Abi abi;
    std::string functionName;
    std::vector<readonly unknown> args;
    () => Promise<bigint> getBlockNumber;
};

// OnChainOffer matches the struct returned by the OTC contract
struct OnChainOffer {
    Address beneficiary;
    bigint tokenAmount;
    bigint discountBps;
    bigint createdAt;
    bigint unlockTime;
    bigint priceUsdPerToken;
    bigint ethUsdPrice;
    double currency;
    bool approved;
    bool paid;
    bool fulfilled;
    bool cancelled;
    Address payer;
    bigint amountPaid;
};

class ReconciliationService {
  private client: SimplePublicClient;
  private otcAddress: Address | undefined;
  private abi: Abi;

  constructor() {
    // Get chain and RPC URL using centralized logic
    const chain = getChain();
    const rpcUrl = getRpcUrl();

    // Create public client (cast to simple interface to avoid deep type issues)
    this.client = createPublicClient({
      chain,
      transport: http(rpcUrl),
    }) as unknown as SimplePublicClient;

    // Use chain-specific contract address based on NETWORK env var
    try {
      this.otcAddress = getContractAddress();
      console.log(
        `[ReconciliationService] Using contract address: ${this.otcAddress} for network: ${process.env.NETWORK || "localnet"}`,
      );
    } catch (error) {
      console.error(
        "[ReconciliationService] Failed to get contract address:",
        error,
      );
      throw error;
    }
    this.abi = otcArtifact.abi as Abi;
  }

    // Type cast needed - viem's readContract return type is too complex for TypeScript to infer

// Lazy singleton instance - only created when needed (at runtime, not build time)

ReconciliationService getReconciliationService();

std::future<void> runReconciliationTask();

} // namespace elizaos
