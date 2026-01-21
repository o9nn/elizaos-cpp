#include "settings.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string generateStatusMessage(IAgentRuntime runtime, WorldSettings worldSettings, bool isOnboarding, std::optional<State> state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Format settings for display
        const auto formattedSettings = Object.entries(worldSettings);
        .map(([key, setting]) => {
            if (typeof setting != 'object' || !setting.name) return null;

            const auto description = setting.description || "";
            const auto usageDescription = setting.usageDescription || "";

            // Skip settings that should be hidden based on visibility function
            if (setting.visibleIf && !setting.visibleIf(worldSettings)) {
                return nullptr;
            }

            return {
                key,
                name: setting.name,
                value: formatSettingValue(setting, isOnboarding),
                description,
                usageDescription,
                required: setting.required,
                configured: setting.value != nullptr,
                };
                });
                .filter(Boolean);

                // Count required settings that are not configured
                const auto requiredUnconfigured = formattedSettings.filter(;
                [&](s) { return s.required && !s.configured; }
                ).size();

                // Generate appropriate message
                if (isOnboarding) {
                    const auto settingsList = formattedSettings;
                    .map((s) => {
                        const auto label = s.required ? "(Required)" : "(Optional)";
                        return std::to_string(s.key) + ": " + std::to_string(s.value) + " " + label + "\n(" + std::to_string(s.name) + ") " + std::to_string(s.usageDescription);
                        });
                        .join("\n\n");

                        const auto validKeys = "Valid setting keys: " + std::to_string(Object.keys(worldSettings).join(", "));

                        const auto commonInstructions = "Instructions for " + runtime.character.name;
                        - Only update settings if the user is clearly responding to a setting you are currently asking about.;
                        - If the user"s reply clearly maps to a setting and a valid value, you **must** call the UPDATE_SETTINGS action with the correct key and value. Do not just respond with a message saying it"s updated — it must be an action.;
                        - Never hallucinate settings or respond with values not listed above.;
                        - Do not call UPDATE_SETTINGS just because the user has started onboarding or you think a setting needs to be configured. Only update when the user clearly provides a specific value for a setting you are currently asking about.;
                        "- Answer setting-related questions using only the name, description, and value from the list.";

                        if (requiredUnconfigured > 0) {
                        return "# PRIORITY TASK: Onboarding with " + std::to_string(state.senderName);

                        ${runtime.character.name} needs to help the user configure ${requiredUnconfigured} required settings:

                    ${settingsList}

                ${validKeys}

            ${commonInstructions}

            "- Prioritize configuring required settings before optional ones.";
        }

        return `All required settings have been configured. Here's the current configuration:;

    ${settingsList}

    ${validKeys}

    "${commonInstructions}";
    }

    // Non-onboarding context - list all public settings with values and descriptions
    return "## Current Configuration\n\n" + std::to_string();
        requiredUnconfigured > 0;
        "? " + "IMPORTANT!: " + requiredUnconfigured + " required settings still need configuration. " + runtime.character.name + " should get onboarded with the OWNER.\n\n"
        : "All required settings are configured.\n\n"
        }${formattedSettings;
        ".map((s) => " + "### " + std::to_string(s.name) + "\n**Value:** " + std::to_string(s.value) + "\n**Description:** " + std::to_string(s.description)
        ".join("\n\n")}";
        } catch (error) {
            std::cerr << "Error generating status message: " + error << std::endl;
            return "Error generating configuration status.";
        }

}

} // namespace elizaos
