#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // For ETH address validation

struct WalletLinkFormProps {
    std::vector<LinkedWallet> wallets;
    bool isProcessing;
};

// Basic regex for Solana address (Base58, 32-44 chars)
// For more robust validation, consider @solana/web3.js PublicKey.isOnCurve or similar

void WalletLinkForm(auto onSubmit, auto isProcessing);

} // namespace elizaos
