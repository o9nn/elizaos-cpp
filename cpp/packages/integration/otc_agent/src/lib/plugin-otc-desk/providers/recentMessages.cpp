#include "recentMessages.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getEntityUsername(Entity entity) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!entity.metadata) return "unknown";
    // Try common sources for username
    for (const auto& source : ["web", "discord", "telegram", "twitter"])
        const auto sourceData = entity.metadata[source];
        if (sourceData && typeof sourceData == "object") {
            const auto meta = sourceData;
            if (meta.username) return meta.username;
        }
    }
    return "unknown";

}

} // namespace elizaos
