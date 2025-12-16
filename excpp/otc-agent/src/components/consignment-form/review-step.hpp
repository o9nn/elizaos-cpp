#include ".button.hpp"
#include ".multiwallet.hpp"
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



struct ReviewStepProps {
    { formData;
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
    () => void onBack;
    () => void onNext;
    std::optional<"evm" | "solana" | null> requiredChain;
    std::optional<bool> isConnectedToRequiredChain;
    std::optional<() => void> onConnect;
    std::optional<bool> privyReady;
    std::optional<std::string> selectedTokenSymbol;
    std::optional<double> selectedTokenDecimals;
};


} // namespace elizaos
