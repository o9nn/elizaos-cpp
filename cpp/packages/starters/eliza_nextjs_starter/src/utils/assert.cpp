#include "assert.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

asserts condition assert(const std::any& condition, const std::string& message) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!condition) {
            // in production, log error to console
            if (process.env.NODE_ENV != "development") {
                std::cerr << "Assertion Failed:" << message << std::endl;
                } else {
                    throw std::runtime_error("Assertion Failed: " + message);
                }
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
