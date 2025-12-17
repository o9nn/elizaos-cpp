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



struct ChainConfig {
    std::string chainId;
    ElementType icon;
};

/**
 * Configuration for currently supported blockchain chains
 *
 * Each chain entry contains:
 * - chainId: CAIP-2 blockchain identifier (mainnet chain IDs used)
 * - validator: Function to validate wallet addresses for this chain
 * - icon: React component for displaying the chain's icon
 *
 * @see https://github.com/ChainAgnostic/CAIPs/blob/master/CAIPs/caip-2.md
 */

/**
 * List of supported chain names
 * @see SUPPORTED_CHAINS
 */

/**
 * Look up the CAIP-2 chain identifier for a given chain name
 * @param chain The name of the blockchain (e.g., "ethereum", "solana")
 * @returns The CAIP-2 chain identifier if found, empty string otherwise
 */
std::string getChainId(const std::string& chain);

/**
 * Reverse lookup the chain name for a given CAIP-2 chain identifier
 * @param chainId The CAIP-2 chain identifier (e.g., "eip155:1", "solana:5eykt4UsFv8P8NJdTREpY1vzqKqZKvdp")
 * @returns The chain if found, empty string otherwise
 */
std::string getChainByChainId(const std::string& chainId);

/**
 * Creates a CAIP-10 account identifier from a chain ID and address
 * account_id: chain_id + ":" + account_address
 *
 * @param chainId The CAIP-2 chain identifier
 * @param address The account address on the specified chain
 * @returns A CAIP-10 compliant account identifier
 * @see https://github.com/ChainAgnostic/CAIPs/blob/master/CAIPs/caip-10.md
 */
std::string createAccountId(const std::string& chainId, const std::string& address);

/**
 * Validates a wallet address based on a given chain's validator function
 * @param address The wallet address to validate
 * @param chain The blockchain name (e.g., "ethereum", "solana")
 * @returns True if the address is valid for the chain, false otherwise
 */
bool validateAddress(const std::string& address, const std::string& chain);

} // namespace elizaos
