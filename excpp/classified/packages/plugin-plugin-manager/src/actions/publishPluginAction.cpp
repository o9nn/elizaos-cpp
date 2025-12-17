#include "publishPluginAction.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractPluginInfo(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Look for file paths
    const auto pathMatch = text.match(/[./][\w/-]+/);
    if (pathMatch) {
        return pathMatch[0];
    }

    // Look for plugin names
    const auto patterns = [/@elizaos\/plugin-[\w-]+/g, /plugin-[\w-]+/g];

    for (const auto& pattern : patterns)
        const auto match = text.match(pattern);
        if (match) {
            return match[0];
        }
    }

    // Try to extract from natural language
    const auto words = text.toLowerCase().split(/\s+/);
    const auto publishIndex = words.findIndex((w) => w == 'publish');

    if (publishIndex != -1) {
        // Look for plugin indicator
        for (int i = publishIndex + 1; i < words.length; i++) {
            if (words[i] == 'plugin' && i - 1 >= 0 && words[i - 1] != 'the') {
                // Get the word before "plugin"
                return "plugin-" + std::to_string(words[i - 1]);
                } else if (words[i].includes('plugin')) {
                    return words[i];
                }
            }
        }

        return nullptr;

}

std::future<std::string> resolvePluginPath(const std::string& pluginInfo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if it's already a path
    if (pluginInfo.includes('/') || pluginInfo.includes('.')) {
        try {
            const auto absolutePath = path.resolve(pluginInfo);
            const auto stat = fs.stat(absolutePath);
            if (stat.isDirectory()) {
                // Check for package.json
                fs.access(path.join(absolutePath, 'package.json'));
                return absolutePath;
            }
            } catch {
                // Path doesn't exist or isn't accessible, continue with other checks
            }
        }

        // Check common locations
        const auto possiblePaths = [;
        path.join(process.cwd(), pluginInfo),
        path.join(process.cwd(), 'packages', pluginInfo),
        path.join(process.cwd(), 'cloned-plugins', pluginInfo),
        path.join(process.cwd(), '..', pluginInfo),
        ];

        for (const auto& possiblePath : possiblePaths)
            try {
                const auto stat = fs.stat(possiblePath);
                if (stat.isDirectory()) {
                    // Check for package.json
                    fs.access(path.join(possiblePath, 'package.json'));
                    return possiblePath;
                }
                } catch {
                    // Path doesn't exist or isn't accessible, continue with next path
                    continue;
                }
            }

            return nullptr;

}

} // namespace elizaos
