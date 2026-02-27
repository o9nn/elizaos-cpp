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



struct StepState {
    std: id;
    std: label;
    "pending" | "processing" | "complete" | "error" status;
    std::optional<std:> statusMessage;
    std::optional<std:> errorMessage;
    std::optional<std:> txHash;
    std::optional<bool> canRetry;
};

struct SubmissionStepProps {
    { formData;
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
    std: consignerAddress;
    std: chain;
    "evm" | "solana" | null activeFamily;
    double selectedTokenDecimals;
    std: selectedTokenSymbol;
    std::optional<std:> selectedTokenName;
    std::optional<std:> selectedTokenAddress;
    std::optional<std: | null> selectedTokenLogoUrl;
    [&]() { return Promise<string> onApproveToken; };
    [&]( onCreateConsignment;
    std::optional<(txHash: std:) { return void> onTxSubmitted; };
    [&](txHash: std:) { return std: getBlockExplorerUrl; };
    [&]() { return void onBack; };
};


} // namespace elizaos
