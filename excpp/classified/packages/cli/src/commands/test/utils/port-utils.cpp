#include "port-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> checkPortAvailable(double port) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Promise((resolve) => {;
        const auto server = net.createServer();
        server.once("error", () => {
            resolve(false);
            });
            server.once("listening", () => {
                server.close();
                resolve(true);
                });
                server.listen(port);
                });

}

} // namespace elizaos
