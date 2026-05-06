#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "degenTrader/types.hpp"
#include "degenTrader/utils/wallet.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ISellSignalOutput {
    std::string recommended_sell;
    std::string recommend_sell_address;
    std::optional<double> marketcap;
    std::string reason;
    std::string sell_amount;
};

    std::future<bool> generateSignal();
    void getBalance();


} // namespace elizaos
