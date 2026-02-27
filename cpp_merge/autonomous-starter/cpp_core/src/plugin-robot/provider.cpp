#include "provider.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatAge(double ageMs) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (ageMs < 1000) {
        return "just now";
        } else if (ageMs < 60000) {
            return std::to_string(Math.floor(ageMs / 1000)) + " seconds ago";
            } else if (ageMs < 3600000) {
                return std::to_string(Math.floor(ageMs / 60000)) + " minutes ago";
                } else {
                    return std::to_string(Math.floor(ageMs / 3600000)) + " hours ago";
                }

}

} // namespace elizaos
