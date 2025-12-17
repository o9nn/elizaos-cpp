#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TransactionMonitor {
public:
    void addTransaction(const std::string& hash);
    void updateTransaction(const std::string& hash, const std::optional<TransactionStatus>& status);
    TransactionStatus getTransaction(const std::string& hash);
    std::future<TransactionStatus> waitForTransaction(const std::string& hash, const std::any& provider, number = 1 confirmations);
};

bigint estimateGasWithBuffer(bigint estimatedGas, number = 1::2 buffer);


} // namespace elizaos
