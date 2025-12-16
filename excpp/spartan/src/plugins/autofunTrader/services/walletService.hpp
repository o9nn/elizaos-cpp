#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".utils/analyzeTrade.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WalletOperationResult {
    bool success;
    std::optional<std::string> signature;
    std::optional<std::string> error;
    std::optional<std::string> outAmount;
    std::optional<std::string> receivedAmount;
    std::optional<std::string> swapUsdValue;
};

class WalletService {
public:
    WalletService(IAgentRuntime private runtime);
    std::future<void> initialize();
    std::future<void> stop();
    void getWallet();
    void getWalletBalances();
    std::future<double> getBalance();

private:
    std::any CONFIRMATION_CONFIG_;
};


} // namespace elizaos
