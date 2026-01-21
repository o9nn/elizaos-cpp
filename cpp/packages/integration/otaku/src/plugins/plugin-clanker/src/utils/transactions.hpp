#include ".types.hpp"
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



class TransactionMonitor {
  private pendingTransactions: Map<string, TransactionStatus> = new Map();
  private listeners: Map<string, ((status: TransactionStatus) => void)[]> =
    new Map();

  addTransaction(hash: string): void {
    this.pendingTransactions.set(hash, {
      hash,
      status: "pending",
      confirmations: 0,
    });
  }

    // Notify listeners

    // Remove if completed

          // Check again in 5 seconds

bigint estimateGasWithBuffer(bigint estimatedGas, number = 1.2 buffer);


} // namespace elizaos
