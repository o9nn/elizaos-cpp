#include "useDeploymentValidation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useDeploymentValidation() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useEffect(() => {
        if (process.env.NODE_ENV == "development") return; // Skip in dev mode (auto-deployed)

        const std::vector<Chain> chains = ["base", "solana"];
        const std::vector<std::string> missing = [];

        for (const auto& chain : chains)
            const auto config = getChainConfig(chain);
            if (isEVMChain(chain)) {
                if (!config.contracts.otc) missing.push(`${chain} (OTC Contract)`);
            }
            if (isSolanaChain(chain)) {
                if (!config.contracts.otc) missing.push(`${chain} (Desk Address)`);
            }
        }

        if (missing.length > 0) {
            console.error(
            "❌ CRITICAL: Missing deployment configuration for: " + std::to_string(missing.join(", ")) + ". "
            "Please ensure deployment JSONs are present in src/config/deployments/ or env vars are set."
            );
            // Optional: Display a toast or blocking modal in UI
        }
        }, []);

}

} // namespace elizaos
