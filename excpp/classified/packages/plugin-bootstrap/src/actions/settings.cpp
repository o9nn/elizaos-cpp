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
            std::cerr << "Error getting settings state: " + std::to_string(error) << std::endl;
            return nullptr;
        }

}

std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto worldId = createUniqueUuid(runtime, serverId);
        const auto world = runtime.getWorld(worldId);

        if (!world) {
            std::cerr << "No world found for server " + std::to_string(serverId) << std::endl;
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
            std::cerr << "Error updating settings state: " + std::to_string(error) << std::endl;
            return false;
        }

}

std::string formatSettingsList(WorldSettings worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto settings = Object.entries(worldSettings);
    .filter(([key]) => !key.startsWith('_')) // Skip internal settings;
    .map(([key, setting]: [string, Setting]) => {
        const auto status = setting.value != nullptr ? 'Configured' : 'Not configured';
        const auto required = setting.required ? 'Required' : 'Optional';
        return "- " + std::to_string(setting.name) + " (" + std::to_string(key) + "): " + std::to_string(status) + ", " + std::to_string(required);
        });
        .join('\n');

        return settings || 'No settings available';

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
    const auto { requiredUnconfigured, optionalUnconfigured } = categorizeSettings(worldSettings);

    // Generate a prompt to extract settings from the user's message
    const auto settingsContext = requiredUnconfigured;
    .concat(optionalUnconfigured);
    .map(([key, setting]) => {
        const auto requiredStr = setting.required ? 'Required.' : 'Optional.';
        return std::to_string(key) + ": " + std::to_string(setting.description) + " " + std::to_string(requiredStr);
        });
        .join('\n');

        const auto basePrompt = dedent`;
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
        const auto result = runtime.useModel<typeof ModelType.OBJECT_LARGE, SettingUpdate[]>(;
        ModelType.OBJECT_LARGE,
        {
            prompt: basePrompt,
            output: 'array',
            schema: {
                type: 'array',
                items: {
                    type: 'object',
                    properties: {
                        key: { type: 'string' },
                        value: { type: 'string' },
                        },
                        required: ['key', 'value'],
                        },
                        },
                    }
                    );

                    // Validate the extracted settings
                    if (!result) {
                        return [];
                    }

                    function extractValidSettings(obj: unknown, worldSettings: WorldSettings) {
                        const std::vector<SettingUpdate> extracted = [];

                        function traverse(node: unknown): void {
                            if (Array.isArray(node)) {
                                for (const auto& item : node)
                                    traverse(item);
                                }
                                } else if (typeof node == 'object' && node != nullptr) {
                                    for (const int [key, value] of Object.entries(node)) {
                                        if (worldSettings[key] && typeof value != 'object') {
                                            extracted.push({ key, value });
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
                                std::cerr << 'Error extracting settings:' << error << std::endl;
                                return [];
                            }

}

std::future<> processSettingUpdates(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings, const std::vector<SettingUpdate>& updates) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    updatedAny: boolean; messages: string[]
}

std::future<ActionResult> handleOnboardingComplete(IAgentRuntime runtime, WorldSettings worldSettings, State _state, HandlerCallback callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Generate completion message
        const auto prompt = composePrompt({;
            state: {
                settingsStatus: formatSettingsList(worldSettings),
                },
                template: completionTemplate,
                });

                const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
                    prompt,
                    });

                    const auto responseContent = parseJSONObjectFromText(response);

                    callback({
                        text: responseContent.text,
                        actions: ['ONBOARDING_COMPLETE'],
                        source: 'discord',
                        });

                        return {
                            text: 'Onboarding completed successfully',
                            values: {
                                success: true,
                                onboardingComplete: true,
                                allRequiredConfigured: true,
                                },
                                data: {
                                    actionName: 'UPDATE_SETTINGS',
                                    action: 'ONBOARDING_COMPLETE',
                                    settingsStatus: formatSettingsList(worldSettings),
                                    },
                                    success: true,
                                    };
                                    } catch (error) {
                                        std::cerr << "Error handling settings completion: " + std::to_string(error) << std::endl;
                                        callback({
                                            text: 'Great! All required settings have been configured. Your server is now fully set up and ready to use.',
                                            actions: ['ONBOARDING_COMPLETE'],
                                            source: 'discord',
                                            });

                                            return {
                                                text: 'Onboarding completed with fallback message',
                                                values: {
                                                    success: true,
                                                    onboardingComplete: true,
                                                    fallbackUsed: true,
                                                    },
                                                    data: {
                                                        actionName: 'UPDATE_SETTINGS',
                                                        action: 'ONBOARDING_COMPLETE',
                                                        error: true /* instanceof check */ ? error.message : std::to_string(error),
                                                        },
                                                        success: true,
                                                        };
                                                    }

}

std::future<ActionResult> generateSuccessResponse(IAgentRuntime runtime, WorldSettings worldSettings, State state, const std::vector<std::string>& messages, HandlerCallback callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check if all required settings are now configured
        const auto { requiredUnconfigured } = categorizeSettings(worldSettings);

        if (requiredUnconfigured.length == 0) {
            // All required settings are configured, complete settings
            return handleOnboardingComplete(runtime, worldSettings, state, callback);
        }

        const auto requiredUnconfiguredString = requiredUnconfigured;
        std::to_string(key) + ": " + std::to_string(setting.name)
        .join('\n');

        // Generate success message
        const auto prompt = composePrompt({;
            state: {
                updateMessages: messages.join('\n'),
                nextSetting: requiredUnconfiguredString,
                remainingRequired: requiredUnconfigured.length.toString(),
                },
                template: successTemplate,
                });

                const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
                    prompt,
                    });

                    const auto responseContent = parseJSONObjectFromText(response);

                    callback({
                        text: responseContent.text,
                        actions: ['SETTING_UPDATED'],
                        source: 'discord',
                        });

                        return {
                            text: 'Settings updated successfully',
                            values: {
                                success: true,
                                settingsUpdated: true,
                                remainingRequired: requiredUnconfigured.length,
                                },
                                data: {
                                    actionName: 'UPDATE_SETTINGS',
                                    action: 'SETTING_UPDATED',
                                    updatedMessages: messages,
                                    remainingRequired: requiredUnconfigured.length,
                                    },
                                    success: true,
                                    };
                                    } catch (error) {
                                        std::cerr << "Error generating success response: " + std::to_string(error) << std::endl;
                                        callback({
                                            text: 'Settings updated successfully. Please continue with the remaining configuration.',
                                            actions: ['SETTING_UPDATED'],
                                            source: 'discord',
                                            });

                                            return {
                                                text: 'Settings updated with fallback message',
                                                values: {
                                                    success: true,
                                                    settingsUpdated: true,
                                                    fallbackUsed: true,
                                                    },
                                                    data: {
                                                        actionName: 'UPDATE_SETTINGS',
                                                        action: 'SETTING_UPDATED',
                                                        error: true /* instanceof check */ ? error.message : std::to_string(error),
                                                        },
                                                        success: true,
                                                        };
                                                    }

}

std::future<ActionResult> generateFailureResponse(IAgentRuntime runtime, WorldSettings worldSettings, State state, HandlerCallback callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get next required setting
        const auto { requiredUnconfigured } = categorizeSettings(worldSettings);

        if (requiredUnconfigured.length == 0) {
            // All required settings are configured, complete settings
            return handleOnboardingComplete(runtime, worldSettings, state, callback);
        }

        const auto requiredUnconfiguredString = requiredUnconfigured;
        std::to_string(key) + ": " + std::to_string(setting.name)
        .join('\n');

        // Generate failure message
        const auto prompt = composePrompt({;
            state: {
                nextSetting: requiredUnconfiguredString,
                remainingRequired: requiredUnconfigured.length.toString(),
                },
                template: failureTemplate,
                });

                const auto response = runtime.useModel(ModelType.TEXT_LARGE, {;
                    prompt,
                    });

                    const auto responseContent = parseJSONObjectFromText(response);

                    callback({
                        text: responseContent.text,
                        actions: ['SETTING_UPDATE_FAILED'],
                        source: 'discord',
                        });

                        return {
                            text: 'No settings were updated',
                            values: {
                                success: false,
                                settingsUpdated: false,
                                remainingRequired: requiredUnconfigured.length,
                                },
                                data: {
                                    actionName: 'UPDATE_SETTINGS',
                                    action: 'SETTING_UPDATE_FAILED',
                                    remainingRequired: requiredUnconfigured.length,
                                    },
                                    success: false,
                                    };
                                    } catch (error) {
                                        std::cerr << "Error generating failure response: " + std::to_string(error) << std::endl;
                                        callback({
                                            text: "I couldn't understand your settings update. Please try again with a clearer format.",
                                            actions: ['SETTING_UPDATE_FAILED'],
                                            source: 'discord',
                                            });

                                            return {
                                                text: 'Failed to parse settings with fallback message',
                                                values: {
                                                    success: false,
                                                    settingsUpdated: false,
                                                    fallbackUsed: true,
                                                    },
                                                    data: {
                                                        actionName: 'UPDATE_SETTINGS',
                                                        action: 'SETTING_UPDATE_FAILED',
                                                        error: true /* instanceof check */ ? error.message : std::to_string(error),
                                                        },
                                                        success: false,
                                                        };
                                                    }

}

std::future<ActionResult> generateErrorResponse(IAgentRuntime runtime, State state, HandlerCallback callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

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
                        actions: ['SETTING_UPDATE_ERROR'],
                        source: 'discord',
                        });

                        return {
                            text: 'Error processing settings',
                            values: {
                                success: false,
                                error: 'PROCESSING_ERROR',
                                },
                                data: {
                                    actionName: 'UPDATE_SETTINGS',
                                    action: 'SETTING_UPDATE_ERROR',
                                    },
                                    success: false,
                                    };
                                    } catch (error) {
                                        std::cerr << "Error generating error response: " + std::to_string(error) << std::endl;
                                        callback({
                                            text: "I'm sorry, but I encountered an error while processing your request. Please try again or contact support if the issue persists.",
                                            actions: ['SETTING_UPDATE_ERROR'],
                                            source: 'discord',
                                            });

                                            return {
                                                text: 'Error with fallback message',
                                                values: {
                                                    success: false,
                                                    error: 'PROCESSING_ERROR',
                                                    fallbackUsed: true,
                                                    },
                                                    data: {
                                                        actionName: 'UPDATE_SETTINGS',
                                                        action: 'SETTING_UPDATE_ERROR',
                                                        error: true /* instanceof check */ ? error.message : std::to_string(error),
                                                        },
                                                        success: false,
                                                        error: true /* instanceof check */ ? error : std::runtime_error(std::to_string(error)),
                                                        };
                                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
