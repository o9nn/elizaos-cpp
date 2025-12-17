#include "export-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string sanitizeFilename(const std::string& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return name;
    .replace(/[^a-zA-Z0-9]/g, '-') // Replace non-alphanumeric with dash;
    .replace(/-+/g, '-') // Replace multiple consecutive dashes with single dash;
    .replace(/^-|-$/g, '') // Remove leading/trailing dashes;
    .toLowerCase();

}

std::unordered_map<std::string, std::any> agentToCharacterData(Agent agent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto characterData = {;
        id: agent.id,
        name: agent.name,
        username: agent.username,
        system: agent.system,
        templates: agent.templates,
        bio: agent.bio,
        messageExamples: agent.messageExamples,
        postExamples: agent.postExamples,
        topics: agent.topics,
        adjectives: agent.adjectives,
        knowledge: agent.knowledge,
        plugins: agent.plugins,
        settings: agent.settings ? { ...agent.settings } : std::nullopt,
        style: agent.style,
        };

        // Remove secrets from settings if they exist
        if (characterData.settings && 'secrets' in characterData.settings) {
            const auto { secrets, ...settingsWithoutSecrets } = characterData.settings;
            characterData.settings = settingsWithoutSecrets;
        }

        // Remove undefined/null fields to keep JSON clean
        return Object.fromEntries(;
        Object.entries(characterData).filter(([_, value]) => value != std::nullopt && value != nullptr);
        );

}

std::string generateExportFilename(const std::string& agentName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto date = new Date().toISOString().split('T')[0]; // YYYY-MM-DD format;
    const auto sanitizedName = sanitizeFilename(agentName || 'agent');
    return std::to_string(sanitizedName) + "-" + std::to_string(date) + ".json";

}

void downloadJsonFile(const std::any& data, const std::string& filename) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto blob = new Blob([JSON.stringify(data, nullptr, 2)], {;
        type: 'application/json',
        });
        const auto url = URL.createObjectURL(blob);
        const auto link = document.createElement('a');
        link.href = url;
        link.download = filename;
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
        URL.revokeObjectURL(url);

}

ExportResult exportCharacterAsJson(Agent agent, std::optional<ToastFunction> toast) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto characterData = agentToCharacterData(agent);
        const auto filename = generateExportFilename(agent.name);

        downloadJsonFile(characterData, filename);

        // Success notification
        if (toast) {
            toast({
                title: 'Character Exported',
                std::to_string(agent.name) + "'s character data has been downloaded as " + std::to_string(filename)
                });
            }

            return { success: true, filename }
            } catch (error) {
                std::cerr << 'Failed to export character:' << error << std::endl;

                // Error notification
                if (toast) {
                    toast({
                        title: 'Export Failed',
                        description: 'Failed to export character data. Please try again.',
                        variant: 'destructive',
                        });
                    }

                    return {
                        success: false,
                        error: true /* instanceof check */ ? error.message : 'Unknown error',
                        };
                    }

}

} // namespace elizaos
