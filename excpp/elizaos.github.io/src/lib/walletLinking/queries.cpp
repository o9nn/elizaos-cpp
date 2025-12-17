#include "queries.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<WalletLinkingData>> getUserWalletData(const std::string& username) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto userWallets = db.query.walletAddresses.findMany({;
        where: and(
        eq(walletAddresses.userId, username),
        eq(walletAddresses.isActive, true),
        ),
        columns: {
            chainId: true,
            accountAddress: true,
            updatedAt: true,
            },
            });

            if (userWallets.length > 0) {
                const auto wallets = userWallets.map((wallet) => ({;
                    chain: getChainByChainId(wallet.chainId),
                    address: wallet.accountAddress,
                    }));

                    const auto lastUpdated = userWallets.reduce((latest, wallet) => {;
                        const auto walletDate = new Date(wallet.updatedAt);
                        return walletDate > latest ? walletDate : latest;
                        }, new Date(0));

                        return {
                            wallets,
                            lastUpdated: lastUpdated.toISOString(),
                            };
                        }

                        return nullptr;

}

} // namespace elizaos
