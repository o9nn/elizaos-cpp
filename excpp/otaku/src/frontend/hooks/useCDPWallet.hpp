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



/**
 * Custom hook to access CDP wallet information
 * 
 * This hook combines multiple CDP hooks and provides a unified interface
 * to access wallet state throughout the application.
 * 
 * @returns {Object} Wallet information including:
 *   - isInitialized: boolean - Whether CDP SDK has finished initializing (IMPORTANT: wait for this before using wallet data)
 *   - isSignedIn: boolean - Whether user is authenticated with CDP wallet
 *   - evmAddress: string | undefined - EVM wallet address (Ethereum, Base, etc.)
 *   - solanaAddress: string | undefined - Solana wallet address
 *   - userEmail: string | undefined - User's email address from CDP
 *   - hasWallet: boolean - Whether user has any wallet connected
 *   - isCdpConfigured: boolean - Whether CDP is properly configured
 *   - signOut: () => Promise<void> - Function to sign out the user
 * 
 * @example
 * ```tsx
 * void MyComponent();
 * ```
 */
void useCDPWallet();

/**
 * Type definition for the wallet info returned by useCDPWallet
 */


} // namespace elizaos
