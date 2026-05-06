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



 // For ETH address validation

struct WalletLinkFormProps {
    std::vector<LinkedWallet> wallets;
    (wallets: LinkedWallet[]) => Promise<void> onSubmit;
    bool isProcessing;
};

// Basic regex for Solana address (Base58, 32-44 chars)
// For more robust validation, consider @solana/web3.js PublicKey.isOnCurve or similar


} // namespace elizaos
