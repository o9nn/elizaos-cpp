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
;

const SEED_CONFIG = "config";
const SEED_BONDING_CURVE = "bonding_curve";

const useProgram = () => {
  const wallet = useWallet();
  const { connection } = useConnection();

  const program = useMemo(() => {
    if (
      !wallet.publicKey ||
      !wallet.signTransaction ||
      !wallet.signAllTransactions
    ) {
      return null;
    }

    const provider = new AnchorProvider(
      connection,
      {
        publicKey: wallet.publicKey,
        signTransaction: wallet.signTransaction,
        signAllTransactions: wallet.signAllTransactions,
      },
      AnchorProvider.defaultOptions(),
    );

    // Use the imported IDL for typing, cast to any to bypass potential strict type mismatch
    const program = new Program<Autofun>(IDL, provider);

    return program;
  }, [
    connection,
    wallet.publicKey,
    wallet.signAllTransactions,
    wallet.signTransaction,
  ]);

  return program;
};

} // namespace elizaos
