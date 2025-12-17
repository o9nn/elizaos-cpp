#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".button.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct StepState {
    std::string id;
    std::string label;
    std::variant<"pending", "processing", "complete", "error"> status;
    std::optional<std::string> statusMessage;
    std::optional<std::string> errorMessage;
    std::optional<std::string> txHash;
    std::optional<bool> canRetry;
};

struct SubmissionStepProps {
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
    std::string consignerAddress;
    std::string chain;
    std::variant<"evm", "solana"> activeFamily;
    double selectedTokenDecimals;
    std::string selectedTokenSymbol;
    std::optional<std::string> selectedTokenName;
    std::optional<std::string> selectedTokenAddress;
    std::optional<std::optional<std::string>> selectedTokenLogoUrl;
};


} // namespace elizaos
