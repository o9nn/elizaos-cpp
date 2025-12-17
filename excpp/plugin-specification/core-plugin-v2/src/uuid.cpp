#include "uuid.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<UUID> validateUuid(unknown value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreValidateUuid(value);

}

UUID stringToUuid(const std::variant<std::string, double>& target) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreStringToUuid(target);

}

} // namespace elizaos
