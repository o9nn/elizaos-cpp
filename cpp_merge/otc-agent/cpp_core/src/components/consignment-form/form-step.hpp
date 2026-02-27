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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct FormStepProps {
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
    std::optional<std:> selectedTokenBalance;
    std::optional<double> selectedTokenDecimals;
    std::optional<std:> selectedTokenSymbol;
};

void FormStep(auto updateFormData, auto onNext, auto onBack, auto selectedTokenBalance, auto selectedTokenDecimals, auto selectedTokenSymbol);

} // namespace elizaos
