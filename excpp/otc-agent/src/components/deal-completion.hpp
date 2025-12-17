#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Extended Navigator type for Web Share API with files support
struct ShareData {
    std::optional<std::string> text;
    std::optional<std::string> title;
    std::optional<std::string> url;
    std::optional<std::vector<File>> files;
};

struct NavigatorWithShare {
};

struct DealCompletionProps {
    std::string quoteId;
    std::string entityId;
    std::string beneficiary;
    std::string tokenAmount;
    double lockupMonths;
    double discountBps;
    double totalUsd;
    double discountUsd;
    double discountedUsd;
    std::string paymentAmount;
    std::string paymentCurrency;
    std::optional<std::string> transactionHash;
    std::optional<std::string> offerId;
    std::optional<std::string> status;
    std::optional<std::variant<"evm", "solana">> chain;
};

void DealCompletion();

} // namespace elizaos
