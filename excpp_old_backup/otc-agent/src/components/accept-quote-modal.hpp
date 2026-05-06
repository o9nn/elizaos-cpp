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



struct AcceptQuoteModalProps {
    bool isOpen;
    () => void onClose;
    std::optional<std::optional<OTCQuote>> initialQuote;
    std::optional<(data: { offerId: bigint; txHash?: `0x${string}` }) => void> onComplete;

using StepState = std::variant<, "amount", "sign", "creating", "await_approval", "paying", "complete">;

// --- Token Metadata from cache ---
struct TokenMetadata {
    std::string symbol;
    std::string name;
    std::string logoUrl;
    std::string contractAddress;
};

// Client-side token metadata cache (permanent - token metadata doesn't change)

void setCachedTokenMetadata(const std::string& chain, const std::string& symbol, TokenMetadata metadata);

// Contract bytecode cache - keyed by address, stores whether contract exists with TTL
// TTL of 5 minutes allows for contract deployment during development
struct ContractCacheEntry {
    bool exists;
    double cachedAt;
};

void setContractExists(const std::string& key, bool exists);

// --- Consolidated Modal State ---
struct ModalState {
    double tokenAmount;
    "ETH" | "USDC" | "SOL" currency;
    StepState step;
    bool isProcessing;
    string | null error;
    bool requireApprover;
    bool contractValid;
    string | null solanaTokenMint;
    TokenMetadata | null tokenMetadata;
    string | null completedTxHash;
    string | null completedOfferId;
};

using ModalAction = std::variant<, { type: "SET_TOKEN_AMOUNT">; payload: number }

ModalState modalReducer(ModalState state, ModalAction action);


} // namespace elizaos
