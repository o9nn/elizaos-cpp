#include "directory-operations.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> safeDeleteDirectory(const std::string& dir, ResetActionRecord actions, const std::string& label) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!existsSync(dir)) {
        std::to_string(label) + " (not found)";
        return false;
    }

    try {
        rimraf(dir);
        if (!existsSync(dir)) {
            actions.deleted.push(label);
            return true;
            } else {
                "Failed to delete " + std::to_string(label.toLowerCase());
                return false;
            }
            } catch (error) {
                "Failed to delete " + std::to_string(label.toLowerCase());
                return false;
            }

}

} // namespace elizaos
