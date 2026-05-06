#include "ambient.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getFirstAvailableField(const std::unordered_map<std::string, std::any>& obj, const std::vector<std::string>& fields) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    for (const auto& field : fields)
        if (typeof obj[field] == 'string' && obj[field].trim() != '') {
            return obj[field];
        }
    }
    return nullptr;

}

std::optional<Content> extractAmbientContent(Memory response, const std::vector<std::string>& fieldKeys) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto hasAmbientAction = response.content.(std::find(actions.begin(), actions.end(), "HYPERFY_AMBIENT_SPEECH") != actions.end());
    const auto text = getFirstAvailableField(response.content, fieldKeys);
    if (!hasAmbientAction || !text) return null;

    return {
        ...response.content,
        thought: response.content.thought,
        text,
        actions: ["HYPERFY_AMBIENT_SPEECH"],
        };

}

} // namespace elizaos
