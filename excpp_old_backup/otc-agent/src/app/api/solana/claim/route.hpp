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



// Wallet interface for Anchor (matches @coral-xyz/anchor's Wallet type)
struct AnchorWallet {
    PublicKey publicKey;
    T tx;
    std::vector<T> txs;
};

std::future<void> POST(NextRequest request);

} // namespace elizaos
