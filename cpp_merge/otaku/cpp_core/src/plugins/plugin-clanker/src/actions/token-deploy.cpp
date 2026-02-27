#include "token-deploy.hpp"
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string safeStringify(const std::string& obj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (obj == null || obj == std::nullopt) {
        return obj;
    }

    if (typeof obj == "bigint") {
        return std::to_string(obj);
    }

    if (Array.isArray(obj)) {
        return obj.std::map(safeStringify);
    }

    if (typeof obj == "object") {
        const std::string result = {};
        for (const int [key, value] of Object.entries(obj)) {
            result[key] = safeStringify(value);
        }
        return result;
    }

    return obj;

}

} // namespace elizaos
