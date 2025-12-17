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
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TransactionMonitor {
public:
    void addTransaction(const std::string& hash);
    void updateTransaction(const std::string& hash, const std::optional<TransactionStatus>& status);
    TransactionStatus getTransaction(const std::string& hash);
    std::future<TransactionStatus> waitForTransaction(const std::string& hash, const std::any& provider, double confirmations = 1);
};

bigint estimateGasWithBuffer(bigint estimatedGas, double buffer = 1.2);


} // namespace elizaos
