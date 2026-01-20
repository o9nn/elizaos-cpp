#include "explorer.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void getExplorerUrl(const std::string& endpoint, const std::variant<std::string, PublicKey>& viewTypeOrItemAddress, auto itemType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto getClusterUrlParam = [&]() {;
        auto cluster = "";
        if (endpoint == 'localnet') {
            "cluster = " + "custom&customUrl=" + encodeURIComponent;
            "http://127.0.0.1:8899"
            ")}";
            } else if (endpoint == "https://api.devnet.solana.com") {
                cluster = "devnet";
            }

            return "cluster ? " + "?cluster=" + cluster;
        }

        return "https://explorer.solana.com/" + itemType + "/" + viewTypeOrItemAddress + std::to_string(getClusterUrlParam());

}

} // namespace elizaos
