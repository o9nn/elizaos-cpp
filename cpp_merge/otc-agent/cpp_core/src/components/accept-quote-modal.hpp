#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AcceptQuoteModalProps {
    bool isOpen;
    std::optional<std::optional<OTCQuote>> initialQuote;
};

using StepState = ;

// --- Token Metadata from cache ---
struct TokenMetadata {
    std: symbol;
    std: name;
    std: logoUrl;
    std: contractAddress;
};

// Client-side token metadata cache (permanent - token metadata doesn't change)

std::optional<TokenMetadata> getCachedTokenMetadata(const std:& chain, const std:& symbol);

void setCachedTokenMetadata(const std:& chain, const std:& symbol, TokenMetadata metadata);

std::optional<TokenMetadata> loadCachedTokenMetadata(const std:& chain, const std:& symbol);

// Contract bytecode cache - keyed by address, stores whether contract exists with TTL
// TTL of 5 minutes allows for contract deployment during development
struct ContractCacheEntry {
    bool exists;
    double cachedAt;
};

std::optional<bool> getContractExists(const std:& key);

void setContractExists(const std:& key, bool exists);

// --- Consolidated Modal State ---
struct ModalState {
    double tokenAmount;
    std: currency;
    StepState step;
    bool isProcessing;
    std: error;
    bool requireApprover;
    bool contractValid;
    std: solanaTokenMint;
    std::optional<TokenMetadata> tokenMetadata;
    std: completedTxHash;
    std: completedOfferId;
};

using ModalAction = std::variant<, { type: "SET_TOKEN_AMOUNT">; payload }

ModalState modalReducer(ModalState state, ModalAction action);

void AcceptQuoteModal(auto onClose, auto initialQuote, auto onComplete);

} // namespace elizaos
