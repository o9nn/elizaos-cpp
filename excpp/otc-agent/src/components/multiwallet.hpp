#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Interface compatible with @solana/wallet-adapter-react for downstream components
struct SolanaTransaction {
    std::optional<std::vector<uint8_t>> signature;
};

using SolanaWalletAdapter = std::function<void()>;

using MultiWalletContextValue = std::optional<{
  // Active chain family - derived from connection state + user preference
  // null when no wallet is connected

  // EVM-specific chain selection (Base, BSC, etc.)

  // Connection status

  // EVM wallet state

  // Solana wallet state

  // Privy auth state

  // Helpers

  // Auth methods

// Solana provider interface (Privy doesn't these types)
struct SolanaProvider {
};

// Extended wallet type for accessing Privy's Solana provider
struct PrivySolanaWallet {
    std::string address;
    std::optional<std::string> chainType;
};

// Window type extension for Phantom wallet detection
using PhantomWindow = void*;

void MultiWalletProvider();

// Default values for SSR/prerendering

MultiWalletContextValue useMultiWallet();

} // namespace elizaos
