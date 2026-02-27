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



// Extended Navigator type for Web Share API with files support
struct ShareData {
    std::optional<std:> text;
    std::optional<std:> title;
    std::optional<std:> url;
    std::optional<std::vector<File>> files;
};

struct NavigatorWithShare {
    std::optional<[&](data?: ShareData) { return boolean> canShare; };
    std::optional<[&](data?: ShareData) { return Promise<void>> share; };
};

struct DealCompletionProps {
    { quote;
    std: quoteId;
    std: entityId;
    std: beneficiary;
    std: tokenAmount;
    double lockupMonths;
    double discountBps;
    double totalUsd;
    double discountUsd;
    double discountedUsd;
    std: paymentAmount;
    std: paymentCurrency;
    std::optional<std:> transactionHash;
    std::optional<std:> offerId;
    std::optional<std:> status;
    std::optional<"evm" | "solana"> chain;
};

void DealCompletion(DealCompletionProps { quote });

} // namespace elizaos
