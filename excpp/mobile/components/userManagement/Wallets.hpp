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

;
;
;
;

default  = usePrivy();
  const { create: createEthereumWallet } = useEmbeddedEthereumWallet();
  const { create: createSolanaWallet } = useEmbeddedSolanaWallet();
  const { createWallet } = useCreateWallet();
  const wallets = user?.linked_accounts.filter(
    (account) => account.type === "wallet",
  );

  using ExtendedChainType = std::variant<, "bitcoin-segwit", "stellar", "cosmos", "sui", "tron", "near", "ton", "spark">;
  using chainTypes = std::variant<"ethereum", "solana", ExtendedChainType>;

  const ALL_CHAIN_TYPES: chainTypes[] = [
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

  const createWallets = (chainType: chainTypes) => {
    switch (chainType) {
      case "ethereum":
        return createEthereumWallet({ createAdditional: true });
      case "solana":
        return createSolanaWallet?.({
          createAdditional: true,
          recoveryMethod: "privy",
        });

      default:
        return createWallet({
          chainType: chainType as ExtendedChainType,
        }).catch((err: any) => {
          console.log(err);
          setError(err?.message ? String(err.message) : String(err));
        });
    }
  };
  return (
    <View
      style={{
        display: "flex",
        flexDirection: "column",
        gap: 10,
        borderWidth: 1,
        borderColor: "black",
        padding: 10,
      }}
    >
      <Text>Wallets</Text>

      <View
        style={{
          display: "flex",
          flexDirection: "row",
          gap: 10,
          flexWrap: "wrap",
        }}
      >
        {ALL_CHAIN_TYPES.map((chainType, i) => (
          <Button
            key={`create-wallet-${chainType}-${i}`}
            title={`Create ${chainType} Wallet`}
            onPress={() => createWallets(chainType)}
          />
        ))}
      </View>
      {error && <Text style={{ color: "red" }}>{error}</Text>}
    </View>
  );
}

} // namespace elizaos
