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

import "server-only";
;
;
;
;
;

/**
 * Retrieves user wallet data from the database.
 *
 * @param username The GitHub username
 * @returns A promise resolving to wallet linking data or null
 */
std::future<WalletLinkingData | null> getUserWalletData(const std::string& username);,
  });

  if (userWallets.length > 0) {
    const wallets = userWallets.map((wallet) => ({
      chain: getChainByChainId(wallet.chainId),
      address: wallet.accountAddress,
    }));

    const lastUpdated = userWallets.reduce((latest, wallet) => {
      const walletDate = new Date(wallet.updatedAt);
      return walletDate > latest ? walletDate : latest;
    }, new Date(0));

    return {
      wallets,
      lastUpdated: lastUpdated.toISOString(),
    };
  }

  return null;
}

} // namespace elizaos
