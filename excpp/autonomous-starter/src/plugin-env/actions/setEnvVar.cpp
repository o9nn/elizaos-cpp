#include "setEnvVar.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<EnvVarUpdate>> extractEnvVarValues(IAgentRuntime runtime, Memory message, State state, EnvVarMetadata envVars) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Generate context for available environment variables
    const auto envVarsContext = Object.entries(envVars);
    .map(([pluginName, plugin]) => {
        return Object.entries(plugin);
        .filter(;
        ([, config]) =>;
        config.status == "missing" || config.status == "invalid",
        );
        .map(([varName, config]) => {
            const auto requiredStr = config.required ? "Required." : "Optional.";
            return pluginName + "." + varName + ": " + config.description + " " + requiredStr;
            });
            .join("\n");
            });
            .filter(Boolean);
            .join("\n");

            if (!envVarsContext) {
                return [];
            }

            const auto prompt = composePrompt({;
                state: {
                    envVarsContext,
                    content: state.text,
                    },
                    template: extractionTemplate,
                    });

                    try {
                        const auto result = runtime.useModel(ModelType.TEXT_LARGE, {;
                            prompt,
                            stopSequences: [],
                            });

                            // Custom parsing for arrays since parseJSONObjectFromText only handles objects
                            auto parsed: any;
                            const auto jsonBlockMatch = "result.match(/" + "json\n([\s\S]*?)\n";

                            try {
                                if (jsonBlockMatch) {
                                    parsed = /* JSON.parse */ jsonBlockMatch[1].trim();
                                    } else {
                                        parsed = /* JSON.parse */ result.trim();
                                    }
                                    } catch (parseError) {
                                        std::cerr << "Error parsing JSON from model response:" << parseError << std::endl;
                                        return [];
                                    }

                                    if (!Array.isArray(parsed)) {
                                        return [];
                                    }

                                    // Validate extracted assignments
                                    const std::vector<EnvVarUpdate> validAssignments = [];

                                    for (const auto& assignment : parsed)
                                        if (
                                        assignment.pluginName &&;
                                        assignment.variableName &&;
                                        assignment.value;
                                        ) {
                                            // Check if the variable exists in our metadata
                                            if (envVars[assignment.pluginName].[assignment.variableName]) {
                                                validAssignments.push_back({
                                                    pluginName: assignment.pluginName,
                                                    variableName: assignment.variableName,
                                                    value: assignment.value,
                                                    });
                                                }
                                            }
                                        }

                                        return validAssignments;
                                        } catch (error) {
                                            std::cerr << "Error extracting environment variable values:" << error << std::endl;
                                            return [];
                                        }

}

std::future<> processEnvVarUpdates(IAgentRuntime runtime, const std::vector<EnvVarUpdate>& updates) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    updatedAny: boolean; messages: string[]
}

void getNextMissingEnvVar(EnvVarMetadata envVars) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    plugin: string; varName: string; config: EnvVarConfig
}

} // namespace elizaos
