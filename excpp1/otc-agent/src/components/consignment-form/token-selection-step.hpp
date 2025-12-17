#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".button.hpp"
#include ".multiwallet.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Address detection helpers
bool isSolanaAddress(const std::string& address);

bool isEvmAddress(const std::string& address);

bool isContractAddress(const std::string& query);

// Minimum thresholds to filter obvious dust

// Client-side token cache (15 minute TTL)
struct CachedTokens {
    std::vector<TokenWithBalance> tokens;
    std::string walletAddress;
    std::string chain;
    double cachedAt;
};

void setTokenCache(const std::string& walletAddress, const std::string& chain, const std::vector<TokenWithBalance>& tokens);

void clearTokenCache(std::optional<std::string> walletAddress, std::optional<std::string> chain);

struct TokenSelectionProps {
};

std::string formatBalance(const std::string& balance, double decimals);

std::string formatUsd(double usd);

struct LoadingState {
    bool isLoading;
    bool hasLoadedOnce;
};

using LoadingAction = std::variant<std::any, std::any>;

LoadingState loadingReducer(LoadingState state, LoadingAction action);


} // namespace elizaos
