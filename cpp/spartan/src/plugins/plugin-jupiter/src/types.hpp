#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct JupiterQuoteParams {
    std::string inputMint;
    std::string outputMint;
    double amount;
    double slippageBps;
};

struct JupiterSwapParams {
    std::any quoteResponse;
    std::string userPublicKey;
    double slippageBps;
};

struct JupiterQuoteResponse {
    std::string inputMint;
    std::string outputMint;
    std::string inAmount;
    std::string outAmount;
    std::string otherAmountThreshold;
    std::string swapMode;
    double slippageBps;
    std::string priceImpactPct;
    std::vector<std::any> routePlan;
    double contextSlot;
    double timeTaken;
};

struct JupiterSwapResponse {
    std::string swapTransaction;
    double lastValidBlockHeight;
    double prioritizationFeeLamports;
};


} // namespace elizaos
