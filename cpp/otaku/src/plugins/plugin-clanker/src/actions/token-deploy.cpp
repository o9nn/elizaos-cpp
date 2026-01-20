#include "token-deploy.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::any safeStringify(const std::any& obj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (obj == null || obj == undefined) {
        return obj;
    }

    if (typeof obj == "bigint") {
        return std::to_string(obj);
    }

    if (Array.isArray(obj)) {
        return obj.map(safeStringify);
    }

    if (typeof obj == "object") {
        const std::any result = {};
        for (const int [key, value] of Object.entries(obj)) {
            result[key] = safeStringify(value);
        }
        return result;
    }

    return obj;

}

} // namespace elizaos
