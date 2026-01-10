#include "logger.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Logger createLogger(std::optional<std::string> prefix) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Logger(prefix);

}

} // namespace elizaos
