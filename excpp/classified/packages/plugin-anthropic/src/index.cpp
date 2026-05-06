#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getSetting(IAgentRuntime runtime, const std::string& key, std::optional<std::string> defaultValue) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return runtime.getSetting(key) || process.env[key] || defaultValue;

}

std::string getApiKey(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return getSetting(runtime, "ANTHROPIC_API_KEY");

}

bool getExperimentalTelemetry(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto setting = getSetting(runtime, "ANTHROPIC_EXPERIMENTAL_TELEMETRY", "false");
    const auto normalizedSetting = std::to_string(setting).toLowerCase();
    const auto result = normalizedSetting == "true";
    return result;

}

void ensureAnthropicAPIKeyExists(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto apiKey = getApiKey(runtime);
        if (!apiKey) {
            throw new Error(
            "ANTHROPIC_API_KEY is missing. Please set it in your environment or runtime settings.";
            );
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
