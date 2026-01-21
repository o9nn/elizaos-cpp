#include "discordHelper.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getDiscordJs() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!discordJsImport) {
            try {
                // Try to import discord.js normally
                discordJsImport = import("discord.js");
                } catch (error) {
                    std::cerr << 'Error importing discord.js << trying fallback method:' << error << std::endl;

                    // If that fails (e.g., due to node: protocol issues), use a workaround
                    // This assumes the discord.js package is installed in node_modules
                    try {
                        // Use the 'module' package to create a require function
                        const auto { createRequire } = import("module");
                        const auto require = createRequire(import.meta.url);
                        discordJsImport = require("discord.js");
                        } catch (fallbackError) {
                            std::cerr << "Discord.js fallback import also failed:" << fallbackError << std::endl;
                            throw std::runtime_error('Failed to import discord.js: ' + fallbackError);
                        }
                    }
                }

                return discordJsImport;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> getPermissionsBitField() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto discord = getDiscordJs();
    return discord.PermissionsBitField;

}

std::future<void> getSlashCommandBuilder() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto discord = getDiscordJs();
    return discord.SlashCommandBuilder;

}

} // namespace elizaos
