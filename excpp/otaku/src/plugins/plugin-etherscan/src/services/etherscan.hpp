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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TransactionReceipt {
    std::string status;
    std::string blockNumber;
    std::string blockHash;
    std::string transactionHash;
    std::string transactionIndex;
    std::string from;
    std::string to;
    std::string contractAddress;
    std::string gasUsed;
    std::string cumulativeGasUsed;
    std::string effectiveGasPrice;
    double confirmations;
    bool success;
};

struct TransactionStatus {
    std::string isError;
    std::string errDescription;
};

// Supported chains with their chain IDs for Etherscan V2 API

using SupportedChain = keyof typeof CHAIN_IDS;

class EtherscanService {
public:
    EtherscanService(IAgentRuntime runtime);
    std::string capabilityDescription() const;
    static std::future<EtherscanService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime runtime);
    std::future<void> stop();
    void validateApiKey();
    double getChainId(std::optional<std::string> chain);
    std::future<TransactionReceipt> getTransactionReceipt(const std::string& txHash, std::optional<std::string> chain);
    std::future<TransactionStatus> getTransactionStatus(const std::string& txHash, std::optional<std::string> chain);
    Promise< getTransactionReceiptStatus(const std::string& txHash, std::optional<std::string> chain);
    std::future<std::unordered_map<std::string, std::any>> getTransactionByHash(const std::string& txHash, std::optional<std::string> chain);
};

struct EtherscanFetchOptions {
    std::optional<bool> expectResult;
    std::optional<std::string> missingResultMessage;
    std::optional<bool> skipStatusCheck;
};


} // namespace elizaos
