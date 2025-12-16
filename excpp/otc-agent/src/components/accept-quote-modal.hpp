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



struct AcceptQuoteModalProps {
    bool isOpen;
    std::optional<std::optional<OTCQuote>> initialQuote;
};

using StepState = std::variant<, "amount", "sign", "creating", "await_approval", "paying", "complete">;

// --- Token Metadata from cache ---
struct TokenMetadata {
    std::string symbol;
    std::string name;
    std::string logoUrl;
    std::string contractAddress;
};

// Client-side token metadata cache (permanent - token metadata doesn't change)

std::optional<TokenMetadata> getCachedTokenMetadata(const std::string& chain, const std::string& symbol);

void setCachedTokenMetadata(const std::string& chain, const std::string& symbol, TokenMetadata metadata);

std::optional<TokenMetadata> loadCachedTokenMetadata(const std::string& chain, const std::string& symbol);

// Contract bytecode cache - keyed by address, stores whether contract exists with TTL
// TTL of 5 minutes allows for contract deployment during development
struct ContractCacheEntry {
    bool exists;
    double cachedAt;
};

std::optional<bool> getContractExists(const std::string& key);

void setContractExists(const std::string& key, bool exists);

// --- Consolidated Modal State ---
struct ModalState {
    double tokenAmount;
    std::variant<"ETH", "USDC", "SOL"> currency;
    StepState step;
    bool isProcessing;
    std::optional<std::string> error;
    bool requireApprover;
    bool contractValid;
    std::optional<std::string> solanaTokenMint;
    std::optional<TokenMetadata> tokenMetadata;
    std::optional<std::string> completedTxHash;
    std::optional<std::string> completedOfferId;
};

using ModalAction = std::variant<, { type: "SET_TOKEN_AMOUNT">; payload: number }

ModalState modalReducer(ModalState state, ModalAction action);


} // namespace elizaos
