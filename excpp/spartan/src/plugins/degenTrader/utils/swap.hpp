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

;

struct SwapParams {
    std::string fromToken;
    std::string toToken;
    double amount;
    double slippage;
    std::optional<'ExactIn' | 'ExactOut'> swapMode;
};


std::future<{ signature: string }> executeSwap(Connection connection, PublicKey walletPubkey, SwapParams params); = await connection.getRecentBlockhash();
  tx.recentBlockhash = blockhash;
  tx.feePayer = walletPubkey;

  // Send and confirm transaction
  const signature = await connection.sendTransaction(tx, []);
  await connection.confirmTransaction(signature);

  return { signature };
}

std::future<TransactionInstruction> createSwapInstruction(Connection _connection, PublicKey walletPubkey, SwapParams params););
}

std::future<PublicKey> getTokenAccount(Connection _connection, PublicKey walletPubkey, PublicKey _mint);

} // namespace elizaos
