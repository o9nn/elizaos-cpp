#include "settings.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<WorldSettings>> getWorldSettings(IAgentRuntime runtime, const std::string& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto worldId = createUniqueUuid(runtime, serverId);
        const auto world = runtime.getWorld(worldId);

        if (!world || !world.metadata.settings) {
            return nullptr;
        }

        return world.metadata.settings;
        } catch (error) {
            std::cerr << "Error getting settings state: " + error << std::endl;
            return nullptr;
        }

}

std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto worldId = createUniqueUuid(runtime, serverId);
        const auto world = runtime.getWorld(worldId);

        if (!world) {
            std::cerr << "No world found for server " + serverId << std::endl;
            return false;
        }

        // Initialize metadata if it doesn't exist
        if (!world.metadata) {
            world.metadata = {}
        }

        // Update settings state
        world.metadata.settings = worldSettings;

        // Save updated world
        runtime.updateWorld(world);

        return true;
        } catch (error) {
            std::cerr << "Error updating settings state: " + error << std::endl;
            return false;
        }

}

std::string formatSettingsList(WorldSettings worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto settings = Object.entries(worldSettings);
    .filter(([key]) => !key.startsWith("_")) // Skip internal settings;
    .map(([key, setting]) => {
        const auto status = setting.value != nullptr ? "Configured" : "Not configured";
        const auto required = setting.required ? "Required" : "Optional";
        return "- " + setting.name + " (" + key + "): " + status + ", " + required;
        });
        .join("\n");

        return settings || "No settings available";

}

void categorizeSettings(WorldSettings worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    configured: [string, Setting][];
    requiredUnconfigured: [string, Setting][];
    optionalUnconfigured: [string, Setting][];

}

std::future<std::vector<SettingUpdate>> extractSettingValues(IAgentRuntime runtime, Memory _message, State state, WorldSettings worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Find what settings need to be configured
    const auto { requiredUnconfigured, optionalUnconfigured } =;
    categorizeSettings(worldSettings);

    // Generate a prompt to extract settings from the user's message
    const auto settingsContext = requiredUnconfigured;
    .concat(optionalUnconfigured);
    .map(([key, setting]) => {
        const auto requiredStr = setting.required ? "Required." : "Optional.";
        return key + ": " + setting.description + " " + requiredStr;
        });
        .join("\n");

        const auto basePrompt = "dedent";
        I need to extract settings values from the user's message.;

        Available settings:
    ${settingsContext}

    User message: ${state.text}

    For each setting mentioned in the user's message, extract the value.;

    Only return settings that are clearly mentioned in the user's message.;
    If a setting is mentioned but no clear value is provided, do not include it.;
    `;

    try {
        // Use runtime.useModel directly with strong typing
        const auto result = runtime.useModel<;
        typeof ModelType.OBJECT_LARGE,
        SettingUpdate[];
        >(ModelType.OBJECT_LARGE, {
            prompt: basePrompt,
            output: "array",
            schema: {
                type: "array",
                items: {
                    type: "object",
                    properties: {
                        key: { type: "string" },
                        value: { type: "string" },
                        },
                        required: ["key", "value"],
                        },
                        },
                        });

                        // Validate the extracted settings
                        if (!result) {
                            return [];
                        }

                        function extractValidSettings(obj: unknown, worldSettings: WorldSettings) {
                            const auto extracted = [];

                            function traverse(node: unknown): void {
                                if (Array.isArray(node)) {
                                    for (const auto& item : node)
                                        traverse(item);
                                    }
                                    } else if (typeof node == "object" && node != nullptr) {
                                        for (const int [key, value] of Object.entries(node)) {
                                            if (worldSettings[key] && typeof value != "object") {
                                                extracted.push_back({ key, value });
                                                } else {
                                                    traverse(value);
                                                }
                                            }
                                        }
                                    }

                                    traverse(obj);
                                    return extracted;
                                }

                                const auto extractedSettings = extractValidSettings(result, worldSettings);

                                return extractedSettings;
                                } catch (error) {
                                    std::cerr << "Error extracting settings:" << error << std::endl;
                                    return [];
                                }

}

std::future<> processSettingUpdates(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings, const std::vector<SettingUpdate>& updates) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    updatedAny: boolean; messages: string[]
}

std::future<void> handleOnboardingComplete(IAgentRuntime runtime, WorldSettings worldSettings, State state, HandlerCallback callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Generate completion message
        const auto prompt = composePrompt({;
            state: {
                settingsStatus: formatSettingsList(worldSettings),
                ...state.values,
                bio: runtime.character.bio,
                agentName: runtime.character.name,
                recentMessages: state.text,
                },
                template: completionTemplate,
                });

                const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
                    prompt,
                    });

                    const auto responseContent = parseJSONObjectFromText(response);

                    callback({
                        text: responseContent.text,
                        actions: ["ONBOARDING_COMPLETE"],
                        source: state.message.content.source,
                        });
                        } catch (error) {
                            std::cerr << "Error handling settings completion: " + error << std::endl;
                            callback({
                                text: "Great! All required settings have been configured. Your server is now fully set up and ready to use.",
                                actions: ["ONBOARDING_COMPLETE"],
                                source: state.message.content.source,
                                });
                            }

}

std::future<void> generateSuccessResponse(IAgentRuntime runtime, WorldSettings worldSettings, State state, const std::vector<std::string>& messages, HandlerCallback callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check if all required settings are now configured
        const auto { requiredUnconfigured } = categorizeSettings(worldSettings);

        if (requiredUnconfigured.length == 0) {
            // All required settings are configured, complete settings
            handleOnboardingComplete(runtime, worldSettings, state, callback);
            return;
        }

        const auto requiredUnconfiguredString = requiredUnconfigured;
        ".map(([key, setting]) => " + key + ": " + setting.name
        .join("\n");

        // Generate success message
        const auto prompt = composePrompt({;
            state: {
                updateMessages: messages.join("\n"),
                nextSetting: requiredUnconfiguredString,
                remainingRequired: requiredUnconfigured.std::to_string(length),
                },
                template: successTemplate,
                });

                const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
                    prompt,
                    });

                    const auto responseContent = parseJSONObjectFromText(response);

                    callback({
                        text: responseContent.text,
                        actions: ["SETTING_UPDATED"],
                        source: "discord",
                        });
                        } catch (error) {
                            std::cerr << "Error generating success response: " + error << std::endl;
                            callback({
                                text: "Settings updated successfully. Please continue with the remaining configuration.",
                                actions: ["SETTING_UPDATED"],
                                source: "discord",
                                });
                            }

}

std::future<void> generateFailureResponse(IAgentRuntime runtime, WorldSettings worldSettings, State state, HandlerCallback callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get next required setting
        const auto { requiredUnconfigured } = categorizeSettings(worldSettings);

        if (requiredUnconfigured.length == 0) {
            // All required settings are configured, complete settings
            handleOnboardingComplete(runtime, worldSettings, state, callback);
            return;
        }

        const auto requiredUnconfiguredString = requiredUnconfigured;
        ".map(([key, setting]) => " + key + ": " + setting.name
        .join("\n");

        // Generate failure message
        const auto prompt = composePrompt({;
            state: {
                nextSetting: requiredUnconfiguredString,
                remainingRequired: requiredUnconfigured.std::to_string(length),
                },
                template: failureTemplate,
                });

                const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
                    prompt,
                    });

                    const auto responseContent = parseJSONObjectFromText(response);

                    callback({
                        text: responseContent.text,
                        actions: ["SETTING_UPDATE_FAILED"],
                        source: "discord",
                        });
                        } catch (error) {
                            std::cerr << "Error generating failure response: " + error << std::endl;
                            callback({
                                text: "I couldn't understand your settings update. Please try again with a clearer format.",
                                actions: ["SETTING_UPDATE_FAILED"],
                                source: "discord",
                                });
                            }

}

std::future<void> generateErrorResponse(IAgentRuntime runtime, State state, HandlerCallback callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto prompt = composePromptFromState({;
            state,
            template: errorTemplate,
            });

            const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
                prompt,
                });

                const auto responseContent = parseJSONObjectFromText(response);

                callback({
                    text: responseContent.text,
                    actions: ["SETTING_UPDATE_ERROR"],
                    source: "discord",
                    });
                    } catch (error) {
                        std::cerr << "Error generating error response: " + error << std::endl;
                        callback({
                            text: "I'm sorry, but I encountered an error while processing your request. Please try again or contact support if the issue persists.",
                            actions: ["SETTING_UPDATE_ERROR"],
                            source: "discord",
                            });
                        }

}

} // namespace elizaos
