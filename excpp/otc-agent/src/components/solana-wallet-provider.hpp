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

"use client";

;
;
;
;
;

/**
 * Get Solana network from unified chain config
 */


/**
 * Get Solana RPC endpoint - supports proxy path or full URL
 */
${configUrl}`;
    }
    // SSR fallback - will be replaced on client
    return "https://api.mainnet-beta.solana.com";
  }

  return configUrl;
}

: {
  children: React.ReactNode;
}) {
  useRenderTracker("SolanaWalletProvider");

  const network = useMemo(() => getSolanaNetwork(), []);
  const endpoint = useMemo(() => getSolanaEndpoint(), []);
  const hasLoggedInit = useRef(false);

  // Log only once on mount, not on every render
  useEffect(() => {
    if (hasLoggedInit.current) return;
    hasLoggedInit.current = true;

    if (process.env.NODE_ENV === "development") {
      console.log("[SolanaConnectionProvider] Provider initialized with:", {
        network,
        endpoint,
        chainConfig: SUPPORTED_CHAINS.solana.id,
      });
    }
  }, [network, endpoint]);

  return (
    <ConnectionProvider endpoint={endpoint}>{children}</ConnectionProvider>
  );
}

} // namespace elizaos
