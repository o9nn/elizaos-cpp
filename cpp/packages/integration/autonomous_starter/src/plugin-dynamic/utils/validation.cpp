#include "validation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool validatePrompt(Memory message) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!message || !message.content || !message.content.text) {
        return false;
    }

    // Check if the text content is meaningful
    const auto text = message.content.text.trim();
    return text.size() > 0;

}

bool isValidJsonSpecification(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        /* JSON.parse */ text;
        return true;
        } catch {
            return false;
        }

}

} // namespace elizaos
