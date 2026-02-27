#include "types.hpp"
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
  private pendingTransactions: Map<std:, TransactionStatus> = std::make_unique<Map>();
  private listeners: Map<std:, [&]((status: TransactionStatus) { return void)[]> =
    std::make_unique<Map>(); };

  addTransaction(hash: std:) {
    this.pendingTransactions.std::set(hash, {
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
