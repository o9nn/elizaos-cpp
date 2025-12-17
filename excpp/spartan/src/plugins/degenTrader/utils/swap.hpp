#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SwapParams {
    std::string fromToken;
    std::string toToken;
    double amount;
    double slippage;
    std::optional<std::variant<'ExactIn', 'ExactOut'>> swapMode;
};

  // Create transaction

  // Add swap instruction

  // Get recent blockhash

  // Send and confirm transaction

std::future<TransactionInstruction> createSwapInstruction(Connection _connection, PublicKey walletPubkey, SwapParams params);

std::future<PublicKey> getTokenAccount(Connection _connection, PublicKey walletPubkey, PublicKey _mint);

} // namespace elizaos
