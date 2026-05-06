#include "readmeUtils.hpp"
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



/**
 * Retrieves user wallet data from the database.
 *
 * @param username The GitHub username
 * @returns A promise resolving to wallet linking data or null
 */
std::future<WalletLinkingData | null> getUserWalletData(const std::string& username);

} // namespace elizaos
