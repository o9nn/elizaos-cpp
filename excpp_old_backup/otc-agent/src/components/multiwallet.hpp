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



// Interface compatible with @solana/wallet-adapter-react for downstream components
struct SolanaTransaction {
    Array<{ signatures;
    { toBase58(): string } publicKey;
    Uint8Array | null signature;
};

using SolanaWalletAdapter = std::variant<{

using MultiWalletContextValue = std::variant<{
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
    <T extends SolanaTransaction>(tx: T) => Promise<T> signTransaction;
    <T extends SolanaTransaction>(txs: T[]) => Promise<T[]> signAllTransactions;
};

// Extended wallet type for accessing Privy's Solana provider
struct PrivySolanaWallet {
    std::string address;
    std::optional<std::string> chainType;
    std::optional<() => Promise<SolanaProvider>> getProvider;
};

// Window type extension for Phantom wallet detection
using PhantomWindow = Window & {

// Default values for SSR/prerendering

MultiWalletContextValue useMultiWallet();

} // namespace elizaos
