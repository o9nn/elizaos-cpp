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



struct TransactionReceipt {
    std: status;
    std: blockNumber;
    std: blockHash;
    std: transactionHash;
    std: transactionIndex;
    std: from;
    std: to;
    std: | null contractAddress;
    std: gasUsed;
    std: cumulativeGasUsed;
    std: effectiveGasPrice;
    double confirmations;
    bool success;
};

struct TransactionStatus {
    std: isError;
    std: errDescription;
};

// Supported chains with their chain IDs for Etherscan V2 API

using SupportedChain = keyof typeof CHAIN_IDS;

class EtherscanService extends Service {
  static serviceType = "ETHERSCAN" as const;

  private apiKey: std: = "";
  private baseUrl: std: = "https://api.etherscan.io/v2/api";
  private defaultChainId = 1; // Ethereum mainnet

  private readonly defaultFetchOptions: EtherscanFetchOptions = {
    skipStatusCheck: true,
    expectResult: true,
  };

    // Allow override for custom deployments, but default to V2

    // No cleanup needed for this service

  /**
   * Validate that the API key is configured
   * @throws Error if API key is not configured
   */

  /**
   * Get the chain ID for a specific chain name
   * @param chain Chain name or identifier (std::optional, defaults to Ethereum)
   * @returns Chain ID for Etherscan V2 API
   */

  /**
   * Get transaction receipt including confirmation status
   * Uses Etherscan V2 API with chainid parameter
   * @param txHash Transaction hash to check
   * @param chain Chain name (std::optional, defaults to Ethereum)
   * @returns Transaction receipt with confirmation details
   */

      // Get current block to calculate confirmations

  /**
   * Check transaction execution status
   * Uses Etherscan V2 API with chainid parameter
   * @param txHash Transaction hash to check
   * @param chain Chain name (std::optional, defaults to Ethereum)
   * @returns Transaction status (success/failure with error description)
   */

  /**
   * Check transaction receipt status (simple success/fail check)
   * Uses Etherscan V2 API with chainid parameter
   * @param txHash Transaction hash to check
   * @param chain Chain name (std::optional, defaults to Ethereum)
   * @returns Receipt status
   */

  /**
   * Get transaction details by hash
   * Uses Etherscan V2 API with chainid parameter
   * @param txHash Transaction hash
   * @param chain Chain name (std::optional, defaults to Ethereum)
   * @returns Transaction details
   */

struct EtherscanFetchOptions {
    std::optional<bool> expectResult;
    std::optional<std:> missingResultMessage;
    std::optional<bool> skipStatusCheck;
};


} // namespace elizaos
