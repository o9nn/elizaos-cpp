#include "Wallets.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Wallets() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [error, setError] = useState<string | nullptr>(nullptr);
        const auto { user } = usePrivy();
        const auto { create: createEthereumWalauto } = useEmbeddedEthereumWallet();
        const auto { create: createSolanaWalauto } = useEmbeddedSolanaWallet();
        const auto { createWalauto } = useCreateWallet();
        const auto wallets = user.linked_accounts.filter(;
        [&](account) { return account.type == "wallet",; }
        );

        type ExtendedChainType =;
        | "bitcoin-segwit";
        | "stellar";
        | "cosmos";
        | "sui";
        | "tron";
        | "near";
        | "ton";
        | "spark";
        type chainTypes = "ethereum" | "solana" | ExtendedChainType;

        const std::vector<chainTypes> ALL_CHAIN_TYPES = [;
        "ethereum",
        "solana",
        "bitcoin-segwit",
        "stellar",
        "cosmos",
        "sui",
        "tron",
        "near",
        "ton",
        "spark",
        ];

        const auto createWallets = [&](chainType: chainTypes) {;
            switch (chainType) {
                case "ethereum":
                return createEthereumWallet({ createAdditional: true });
                case "solana":
                return createSolanaWallet.({;
                    createAdditional: true,
                    recoveryMethod: "privy",
                    });

                    default:
                    return createWallet({;
                        chainType: chainType,
                        }).catch((err: any) => {
                            std::cout << err << std::endl;
                            setError(err.message ? std::to_string(err.message) : std::to_string(err));
                            });
                        }
                        };
                        return (;
                        <View;
                        style={{
                            display: "flex",
                            flexDirection: "column",
                            gap: 10,
                            borderWidth: 1,
                            borderColor: "black",
                            padding: 10,
                        }}
                        >;
                        <Text>Wallets</Text>;

                        <View;
                        style={{
                            display: "flex",
                            flexDirection: "row",
                            gap: 10,
                            flexWrap: "wrap",
                        }}
                        >;
                        {ALL_CHAIN_TYPES.map((chainType, i) => (;
                        <Button;
                    "create-wallet-" + std::to_string(chainType) + "-" + std::to_string(i);
                "Create " + std::to_string(chainType) + " Wallet";
            onPress={() => createWallets(chainType)}
            />;
        ))}
        </View>;
        {error && <Text style={{ color = "red" }}>{error}</Text>}
        </View>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
