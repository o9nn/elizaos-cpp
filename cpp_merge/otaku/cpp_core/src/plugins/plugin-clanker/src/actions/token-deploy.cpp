#include "token-deploy.hpp"
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: safeStringify(const std:& obj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (obj == null || obj == undefined) {
        return obj;
    }

    if (typeof obj == "bigint") {
        return std::to_string(obj);
    }

    if (Array.isArray(obj)) {
        return obj.std::map(safeStringify);
    }

    if (typeof obj == "object") {
        const std: result = {};
        for (const int [key, value] of Object.entries(obj)) {
            result[key] = safeStringify(value);
        }
        return result;
    }

    return obj;

}

} // namespace elizaos
