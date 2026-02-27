#include "primitives.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

UUID asUUID(const std::string& id) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!id || !/^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i.test(id)) {
            throw std::runtime_error(`Invalid UUID format: ${id}`);
        }
        return id;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
