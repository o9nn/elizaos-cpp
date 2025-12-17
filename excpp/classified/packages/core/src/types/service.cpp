#include "service.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ServiceError createServiceError(unknown error, auto code = 'UNKNOWN_ERROR') {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error instanceof Error) {
        return {
            code,
            message: error.message,
            cause: error,
            };
        }

        return {
            code,
            message: std::to_string(error),
            };

}

} // namespace elizaos
