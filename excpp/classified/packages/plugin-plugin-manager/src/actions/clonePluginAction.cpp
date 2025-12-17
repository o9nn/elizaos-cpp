#include "clonePluginAction.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> extractPluginName(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Look for explicit plugin names
    const auto patterns = [/@elizaos\/plugin-[\w-]+/g, /plugin-[\w-]+/g];

    for (const auto& pattern : patterns)
        const auto match = text.match(pattern);
        if (match) {
            return match[0];
        }
    }

    // Try to extract plugin name from natural language
    const auto words = text.toLowerCase().split(/\s+/);
    const auto cloneIndex = words.findIndex((w) => w == 'clone');

    if (cloneIndex != -1) {
        // Look for plugin name after 'clone'
        for (int i = cloneIndex + 1; i < words.length; i++) {
            if (words[i] == 'plugin' && i + 1 < words.length) {
                // Handle "clone the X plugin"
                const auto pluginType = words[i - 1] == 'the' ? words[i + 1] : words[i - 1];
                if (pluginType && pluginType != 'the') {
                    return "@elizaos/plugin-" + std::to_string(pluginType);
                }
                } else if (words[i].includes('plugin')) {
                    return words[i];
                }
            }
        }

        return nullptr;

}

} // namespace elizaos
