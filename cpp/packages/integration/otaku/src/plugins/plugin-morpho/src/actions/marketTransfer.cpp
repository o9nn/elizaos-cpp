#include "marketTransfer.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getOperationEmoji(const std::string& intent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (intent) {
        // case "supply":
        return "🏦";
        // case "supplyCollateral":
        return "🔐";
        // case "borrow":
        return "💸";
        // case "repay":
        return "💰";
        // case "withdraw":
        return "📤";
        // case "withdrawCollateral":
        return "🔓";
        // default:
        return "⚡";
    }

}

std::string getOperationDescription(const std::string& intent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (intent) {
        // case "supply":
        return "Supply (lend assets to earn yield)";
        // case "supplyCollateral":
        return "Supply Collateral (secure borrowing position)";
        // case "borrow":
        return "Borrow (borrow assets against collateral)";
        // case "repay":
        return "Repay (repay borrowed assets)";
        // case "withdraw":
        return "Withdraw (withdraw supplied assets)";
        // case "withdrawCollateral":
        return "Withdraw Collateral (remove collateral)";
        // default:
        return "Market Operation";
    }

}

} // namespace elizaos
