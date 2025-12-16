#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Helper module to isolate discord.js imports and avoid node: protocol issues
 * when the package is dynamically loaded.
 */

// Re-just the types we need
type {
  Guild,
  Channel,
  Client,
  GuildChannel,
  TextChannel,
  VoiceChannel,
  ButtonInteraction,
  ChatInputCommandInteraction,
  CommandInteraction,
  ModalSubmitInteraction,
  SlashCommandBuilder,
  PermissionsBitField,
} from 'discord.js';

// Create a lazy loader for anything that's not just a type
let discordJsImport: any = null;

/**
 * Safely gets discord.js exports, handling potential dynamic import issues
 */
async  catch (error) {
      console.error('Error importing discord.js, trying fallback method:', error);

      // If that fails (e.g., due to node: protocol issues), use a workaround
      // This assumes the discord.js package is installed in node_modules
      try {
        // Use the 'module' package to create a require  = await import('module');
        const require = createRequire(import.meta.url);
        discordJsImport = require('discord.js');
      } catch (fallbackError) {
        console.error('Discord.js fallback import also failed:', fallbackError);
        throw new Error('Failed to import discord.js: ' + fallbackError);
      }
    }
  }

  return discordJsImport;
}

/**
 * Gets the PermissionsBitField from discord.js
 */
async 

/**
 * Gets the SlashCommandBuilder from discord.js
 */
async 

} // namespace elizaos
