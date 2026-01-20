#include "uuid.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

UUID asUUID(const std::string& id) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto validUuid = validateUuid(id);
        if (!validUuid) {
            throw std::runtime_error(`Invalid UUID format: ${id}`);
        }
        return id.toLowerCase();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

UUID generateUuidFromString(const std::string& input) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return stringToUuid(input);

}

} // namespace elizaos
