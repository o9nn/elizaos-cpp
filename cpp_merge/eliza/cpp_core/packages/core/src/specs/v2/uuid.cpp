#include "uuid.hpp"
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<UUID> validateUuid(const std:& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreValidateUuid(value);

}

UUID stringToUuid(const std::variant<std:, double>& target) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreStringToUuid(target);

}

} // namespace elizaos
