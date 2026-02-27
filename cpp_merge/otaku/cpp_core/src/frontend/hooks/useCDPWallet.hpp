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



/**
 * Custom hook to access CDP wallet information
 * 
 * This hook combines multiple CDP hooks and provides a unified interface
 * to access wallet state throughout the application.
 * 
 * @returns {Object} Wallet information including:
 *   - isInitialized - Whether CDP SDK has finished initializing (IMPORTANT: wait for this before using wallet data)
 *   - isSignedIn - Whether user is authenticated with CDP wallet
 *   - evmAddress: std::string | std::nullopt - EVM wallet address (Ethereum, Base, etc.)
 *   - solanaAddress: std::string | std::nullopt - Solana wallet address
 *   - userEmail: std::string | std::nullopt - User's email address from CDP
 *   - hasWallet - Whether user has std::string wallet connected
 *   - isCdpConfigured - Whether CDP is properly configured
 *   - signOut: [&]() { return Promise<void> - Function to sign out the user
 * 
 * @example
 * """tsx
 * void MyComponent(); };
 * """
 */
void useCDPWallet();

/**
 * Type definition for the wallet info returned by useCDPWallet
 */


} // namespace elizaos
