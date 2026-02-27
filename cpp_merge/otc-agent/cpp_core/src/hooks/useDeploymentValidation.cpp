#include "useDeploymentValidation.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useDeploymentValidation() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useEffect[&](() {
        if (std::getenv("NODE_ENV") == "development") return; // Skip in dev mode (auto-deployed)

        const std::vector<Chain> chains = ["base", "solana"];
        const std::vector<std::string> missing = [];

        for (const auto& chain : chains)
            const auto config = getChainConfig(chain);
            if (isEVMChain(chain)) {
                if (!config.contracts.otc) missing.push("" + std::to_string(chain) + " (OTC Contract)");
            }
            if (isSolanaChain(chain)) {
                if (!config.contracts.otc) missing.push("" + std::to_string(chain) + " (Desk Address)");
            }
        }

        if (missing.size() > 0) {
            console.error(
            "❌ CRITICAL: Missing deployment configuration for: " + std::to_string(missing.join(", ")) + ". "
            "Please ensure deployment JSONs are present in src/config/deployments/ or env vars are set."
            );
            // Optional: Display a toast or blocking modal in UI
        }
        }, []);

}

} // namespace elizaos
