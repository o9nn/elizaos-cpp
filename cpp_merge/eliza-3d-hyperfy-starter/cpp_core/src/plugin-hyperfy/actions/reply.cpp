#include "reply.hpp"
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getFirstAvailableField(const std::unordered_map<std::string, std::string>& obj, const std::vector<std::string>& fields) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    for (const auto& field : fields)
        if (typeof obj[field] == 'string' && obj[field] != '') {
            return obj[field];
        }
    }
    return nullptr;

}

std::optional<Content> extractReplyContent(Memory response, const std::vector<std::string>& replyFieldKeys) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto hasReplyAction = response.content.(std::find(actions.begin(), actions.end(), "REPLY") != actions.end());
    const auto text = getFirstAvailableField(response.content, replyFieldKeys);

    if (!hasReplyAction || !text) return null;

    return {
        ...response.content,
        thought: response.content.thought,
        text,
        actions: ["REPLY"],
        };

}

} // namespace elizaos
