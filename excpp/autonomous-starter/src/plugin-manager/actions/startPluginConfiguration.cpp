#include "startPluginConfiguration.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<std::string>> extractPluginNameFromMessage(IAgentRuntime runtime, const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // First try simple extraction patterns
    const auto patterns = [;
    /configure\s+(?:the\s+)?(\w+)\s+plugin/i,
    /setup\s+(?:the\s+)?(\w+)\s+plugin/i,
    /(\w+)\s+plugin\s+config/i,
    /set\s+up\s+(?:the\s+)?(\w+)\s+plugin/i,
    /configure\s+(\w+)/i,
    ];

    for (const auto& pattern : patterns)
        const auto match = text.match(pattern);
        if (match && match[1]) {
            return match[1].toLowerCase();
        }
    }

    // Try using AI to extract plugin name
    try {
        const auto prompt = "Extract the plugin name from this user message about plugin configuration: "" + std::to_string(text);

        If the user mentions a specific plugin name, return just the plugin name (lowercase, no spaces).;
        If no specific plugin is mentioned, return "unknown".;

        Examples:
        - "configure the openai plugin" → "openai";
        - "setup discord plugin" → "discord";
        - "I want to configure twitter" → "twitter";
        - "setup plugin environment variables" → "unknown";

        Plugin name:`;

        const auto result = runtime.useModel("text", {;
            prompt,
            temperature: 0.1,
            maxTokens: 50,
            });

            const auto extracted = result.trim().toLowerCase();
            if (
            extracted &&;
            extracted != "unknown" &&;
            extracted.length > 0 &&;
            extracted.length < 50;
            ) {
                return extracted;
            }
            } catch (error) {
                std::cout << "[startPluginConfiguration] AI extraction failed:" << error << std::endl;
            }

            return nullptr;

}

} // namespace elizaos
