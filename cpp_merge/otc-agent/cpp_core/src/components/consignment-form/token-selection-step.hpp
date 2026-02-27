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



// Address detection helpers
bool isSolanaAddress(const std:& address);

bool isEvmAddress(const std:& address);

bool isContractAddress(const std:& query);

// Minimum thresholds to filter obvious dust

// Client-side token cache (15 minute TTL)
struct CachedTokens {
    std::vector<TokenWithBalance> tokens;
    std: walletAddress;
    std: chain;
    double cachedAt;
};

void setTokenCache(const std:& walletAddress, const std:& chain, const std::vector<TokenWithBalance>& tokens);

void clearTokenCache(std::optional<std:> walletAddress, std::optional<std:> chain);

struct TokenSelectionProps {
    { tokenId: std: } formData;

std: formatBalance(const std:& balance, double decimals);

std: formatUsd(double usd);

struct LoadingState {
    bool isLoading;
    bool hasLoadedOnce;
};

using LoadingAction = std::variant<{ type: "START_LOADING" }, { type: "FINISH_LOADING" }>;

LoadingState loadingReducer(LoadingState state, LoadingAction action);


} // namespace elizaos
