#include "jito.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void getJitoEndpoint(JitoRegion region) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return JitoEndpoints[region];

}

std::future<void> sendTxUsingJito(auto region) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto rpcEndpoint = getJitoEndpoint(region);
        const auto encodedTx = bs58.encode(serializedTx);
        const auto payload = {;
            jsonrpc: "2.0",
            id: 1,
            method: "sendTransaction",
            params: [encodedTx],
            };
            const auto res = "fetch(" + rpcEndpoint + "?bundleOnly=true";
                method: "POST",
                body: /* JSON.stringify */ std::string(payload),
                headers: { "Content-Type": "application/json" },
                });
                const auto json = (res.json());
                if (json.error) {
                    throw std::runtime_error(json.error.message);
                }
                return json;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
