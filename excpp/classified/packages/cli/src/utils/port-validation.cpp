#include "port-validation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double validatePort(const std::string& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto port = Number.parseInt(value, 10);
        if (Number.isNaN(port) || port <= 0 || port > 65535) {
            throw std::runtime_error('Port must be a number between 1 and 65535');
        }
        return port;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
