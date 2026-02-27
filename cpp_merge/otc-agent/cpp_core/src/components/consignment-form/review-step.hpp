#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "button.hpp"
#include "multiwallet.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ReviewStepProps {
    std: tokenId;
    std: amount;
    bool isNegotiable;
    double fixedDiscountBps;
    double fixedLockupDays;
    double minDiscountBps;
    double maxDiscountBps;
    double minLockupDays;
    double maxLockupDays;
    std: minDealAmount;
    std: maxDealAmount;
    bool isFractionalized;
    bool isPrivate;
    double maxPriceVolatilityBps;
    double maxTimeToExecuteSeconds;
    std::optional<std:> requiredChain;
    std::optional<bool> isConnectedToRequiredChain;
    std::optional<bool> privyReady;
    std::optional<std:> selectedTokenSymbol;
    std::optional<double> selectedTokenDecimals;
};

void ReviewStep(auto onBack, auto onNext, auto requiredChain, auto isConnectedToRequiredChain, auto onConnect, auto privyReady, auto selectedTokenSymbol);

} // namespace elizaos
