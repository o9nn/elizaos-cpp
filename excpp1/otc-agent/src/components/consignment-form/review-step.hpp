#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".button.hpp"
#include ".multiwallet.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ReviewStepProps {
    std::string tokenId;
    std::string amount;
    bool isNegotiable;
    double fixedDiscountBps;
    double fixedLockupDays;
    double minDiscountBps;
    double maxDiscountBps;
    double minLockupDays;
    double maxLockupDays;
    std::string minDealAmount;
    std::string maxDealAmount;
    bool isFractionalized;
    bool isPrivate;
    double maxPriceVolatilityBps;
    double maxTimeToExecuteSeconds;
    std::optional<std::variant<"evm", "solana">> requiredChain;
    std::optional<bool> isConnectedToRequiredChain;
    std::optional<bool> privyReady;
    std::optional<std::string> selectedTokenSymbol;
    std::optional<double> selectedTokenDecimals;
};


} // namespace elizaos
