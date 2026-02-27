#include "envStatus.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatEnvVarStatus(EnvVarConfig config, bool showValues = false) {
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

            auto statusText = statusIcon + " **" + std::to_string(config.description || "Environment Variable") + "**\n";
            "statusText += " + "   Type: " + typeLabel + " (" + requiredLabel + ")\n"
            "statusText += " + "   Status: " + std::to_string(config.status.toUpperCase()) + "\n"
            "statusText += " + "   Value: " + valueDisplay + "\n"

            if (config.canGenerate && config.status == "missing") {
                "statusText += " + "   🤖 Can be auto-generated\n";
            }

            if (config.lastError) {
                "statusText += " + "   Error: " + config.lastError + "\n"
            }

            return statusText;

}

std::string generateEnvStatusMessage(EnvVarMetadata envVars, bool showValues = false) {
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
            "statusMessage += " + "**Summary:** " + validVars + "/" + totalVars + " variables configured\n"

            if (missingRequired > 0) {
                "statusMessage += " + "**Missing Required:** " + missingRequired + " variables\n"
                if (generatable > 0) {
                    "statusMessage += " + "**Auto-generatable:** " + generatable + " variables\n"
                }
                if (needsUserInput > 0) {
                    "statusMessage += " + "**Needs User Input:** " + needsUserInput + " variables\n"
                }
            }

            statusMessage += "\n";

            // Group by plugin
            for (const int [pluginName, plugin] of Object.entries(envVars)) {
                "statusMessage += " + "## " + std::to_string(pluginName.charAt(0).toUpperCase() + pluginName.slice(1)) + " Plugin\n\n";

                for (const int [varName, config] of Object.entries(plugin)) {
                    "statusMessage += " + "### " + varName + "\n";
                    statusMessage += formatEnvVarStatus(config, showValues);
                    statusMessage += "\n";
                }
            }

            // Add action recommendations
            if (missingRequired > 0) {
                statusMessage += "\n## Recommended Actions\n\n";

                if (generatable > 0) {
                    "statusMessage += " + "1. **Generate Variables**: I can automatically generate " + generatable + " variables for you.\n"
                }

                if (needsUserInput > 0) {
                    "statusMessage += " + "2. **User Input Required**: " + needsUserInput + " variables need to be provided by you.\n"
                }

                statusMessage +=;
                "\nUse the SET_ENV_VAR action to configure variables or GENERATE_ENV_VAR to auto-generate them.\n";
            }

            return statusMessage;

}

} // namespace elizaos
