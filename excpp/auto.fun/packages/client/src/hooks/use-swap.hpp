#include "use-config-account.hpp"
#include "use-mev-protection.hpp"
#include "use-slippage.hpp"
#include "use-transaction-speed.hpp"
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



struct SwapParams {
    "buy" | "sell" style;
    double amount;
    std::string tokenAddress;
    std::optional<IToken> token;
    double reserveToken;
    double reserveLamport;
};

    /** If token is currently migration, or has failed or if status does not exist, we should never even get here */


} // namespace elizaos
