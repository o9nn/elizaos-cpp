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



using LinkedWallet = z::infer<typeof LinkedWalletSchema>;
using WalletLinkingData = z::infer<typeof WalletLinkingDataSchema>;

/**
 * Parses wallet linking data from a given README content string.
 * Data is expected to be in JSON format within specific comment markers.
 * @param readmeContent The string content of the README file.
 * @returns The parsed and validated wallet linking data, or null if no valid data found.
 */
std::optional<WalletLinkingData> parseWalletLinkingDataFromReadme(const std::string& readmeContent);

/**
 * Generates an updated README content string with the provided wallet linking data.
 * It will replace an existing wallet section if found, or append a new one.
 * The wallet information is stored as JSON in a hidden HTML comment.
 * @param currentReadme The current content of the README file.
 * @param wallets Array of wallet information to store.
 * @returns The updated README content string.
 */
  // Validate wallets array using Zod before generating content

  // Validate the complete data structure

/**
 * Generates the wallet section content for a README file
 * @param wallets Array of wallet information to store
 * @returns The formatted wallet section string with markers
 */
std::string generateReadmeWalletSection(const std::vector<LinkedWallet>& wallets);

/**
 * Helper function to get a wallet address for a specific chain
 * @param data The wallet linking data
 * @param chain The chain to look for (case insensitive)
 * @returns The wallet address or empty string if not found
 */
std::string getWalletAddressForChain(const std::optional<WalletLinkingData>& data, const std::string& chain);

} // namespace elizaos
