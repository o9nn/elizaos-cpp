#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Wallet interface for Anchor (matches @coral-xyz/anchor's Wallet type)
struct AnchorWallet {
    PublicKey publicKey;
};

std::future<void> POST(NextRequest request);

} // namespace elizaos
