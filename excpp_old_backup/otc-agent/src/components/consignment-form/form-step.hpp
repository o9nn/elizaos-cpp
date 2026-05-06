#include ".button.hpp"
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



struct FormStepProps {
    { formData;
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
    (updates: Partial<FormStepProps["formData"]>) => void updateFormData;
    () => void onNext;
    () => void onBack;
    std::optional<std::string> selectedTokenBalance;
    std::optional<double> selectedTokenDecimals;
    std::optional<std::string> selectedTokenSymbol;
};


} // namespace elizaos
