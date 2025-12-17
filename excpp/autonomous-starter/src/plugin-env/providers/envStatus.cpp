#include "envStatus.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatEnvVarStatus(EnvVarConfig config, boolean = false showValues) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto statusIcon = {;
        missing: "❌",
        generating: "⏳",
        validating: "🔍",
        invalid: "⚠️",
        valid: "✅",
        }[config.status];

        const auto typeLabel = config.type.replace("_", " ").toUpperCase();
        const auto requiredLabel = config.required ? "Required" : "Optional";

        auto valueDisplay = "Not set";
        if (config.value) {
            if (
            config.type == "api_key" ||;
            config.type == "secret" ||;
            config.type == "private_key";
            ) {
                valueDisplay = showValues ? config.value : "****************";
                } else {
                    valueDisplay = config.value;
                }
            }

            auto statusText = std::to_string(statusIcon) + " **" + std::to_string(config.description || "Environment Variable") + "**\n";
            "   Type: " + std::to_string(typeLabel) + " (" + std::to_string(requiredLabel) + ")\n"
            "   Status: " + std::to_string(config.status.toUpperCase()) + "\n"
            "   Value: " + std::to_string(valueDisplay) + "\n"

            if (config.canGenerate && config.status == "missing") {
                "   🤖 Can be auto-generated\n";
            }

            if (config.lastError) {
                "   Error: " + std::to_string(config.lastError) + "\n"
            }

            return statusText;

}

std::string generateEnvStatusMessage(EnvVarMetadata envVars, boolean = false showValues) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto plugins = Object.keys(envVars);

    if (plugins.length == 0) {
        return "No environment variables configured yet.";
    }

    // Count variables by status
    auto totalVars = 0;
    auto missingRequired = 0;
    auto generatable = 0;
    auto needsUserInput = 0;
    auto validVars = 0;

    for (const auto& plugin : Object.values(envVars)
        for (const auto& config : Object.values(plugin)
            totalVars++;
            if (config.status == "valid") {
                validVars++;
                } else if (config.required && config.status == "missing") {
                    missingRequired++;
                    if (config.canGenerate) {
                        generatable++;
                        } else {
                            needsUserInput++;
                        }
                    }
                }
            }

            auto statusMessage = "# Environment Variables Status\n\n";
            "**Summary:** " + std::to_string(validVars) + "/" + std::to_string(totalVars) + " variables configured\n"

            if (missingRequired > 0) {
                "**Missing Required:** " + std::to_string(missingRequired) + " variables\n"
                if (generatable > 0) {
                    "**Auto-generatable:** " + std::to_string(generatable) + " variables\n"
                }
                if (needsUserInput > 0) {
                    "**Needs User Input:** " + std::to_string(needsUserInput) + " variables\n"
                }
            }

            statusMessage += "\n";

            // Group by plugin
            for (const int [pluginName, plugin] of Object.entries(envVars)) {
                "## " + std::to_string(pluginName.charAt(0).toUpperCase() + pluginName.slice(1)) + " Plugin\n\n";

                for (const int [varName, config] of Object.entries(plugin)) {
                    "### " + std::to_string(varName) + "\n";
                    statusMessage += formatEnvVarStatus(config, showValues);
                    statusMessage += "\n";
                }
            }

            // Add action recommendations
            if (missingRequired > 0) {
                statusMessage += "\n## Recommended Actions\n\n";

                if (generatable > 0) {
                    "1. **Generate Variables**: I can automatically generate " + std::to_string(generatable) + " variables for you.\n"
                }

                if (needsUserInput > 0) {
                    "2. **User Input Required**: " + std::to_string(needsUserInput) + " variables need to be provided by you.\n"
                }

                statusMessage +=;
                "\nUse the SET_ENV_VAR action to configure variables or GENERATE_ENV_VAR to auto-generate them.\n";
            }

            return statusMessage;

}

} // namespace elizaos
