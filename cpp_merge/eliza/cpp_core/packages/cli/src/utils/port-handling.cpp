#include "port-handling.hpp"
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> isPortFree(double port) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Promise[&]((resolve) {;
        const auto server = net.createServer();

        server.once[&]("error", () { return resolve(false)); };
        server.once[&]("listening", () {
            server.close();
            resolve(true);
            });

            server.listen(port);
            });

}

std::future<double> findNextAvailablePort(double startPort) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto port = startPort;
    while (!(isPortFree(port))) {
        port++;
    }
    return port;

}

} // namespace elizaos
