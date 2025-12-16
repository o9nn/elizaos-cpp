#include "elizaos/core.hpp"
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
 * Initializes the character with the provided runtime, configuration, actions, providers, and evaluators.
 * Registers actions, providers, and evaluators to the runtime. Registers runtime events for "DISCORD_WORLD_JOINED" and "DISCORD_SERVER_CONNECTED".
 *
 * @param {Object} param - Object containing runtime, config, actions, providers, and evaluators.
 * @param {IAgentRuntime} param.runtime - The runtime instance to use.
 * @param {OnboardingConfig} param.config - The configuration for onboarding.
 * @param {Action[]} [param.actions] - Optional array of actions to register.
 * @param {Provider[]} [param.providers] - Optional array of providers to register.
 * @param {Evaluator[]} [param.evaluators] - Optional array of evaluators to register.
 */

  // Register runtime events
    // TODO: Save settings config to runtime
    await initializeAllSystems(runtime, [params.server], config);

  // when booting up into a server we're in, fire a connected event
    await initializeAllSystems(runtime, [params.server], config);

  // Register runtime events
      await initializeOnboarding(runtime, params.world, config);

/**
 * Initializes all systems for the given servers with the provided runtime, servers, and onboarding configuration.
 *
 * @param {IAgentRuntime} runtime - The runtime object that provides functionalities for the agent.
 * @param {Guild[]} servers - The list of servers to initialize systems for.
 * @param {OnboardingConfig} config - The configuration settings for onboarding.
 * @returns {Promise<void>} - A Promise that resolves when all systems have been initialized.
 */
std::future<void> initializeAllSystems(IAgentRuntime runtime, const std::vector<Guild>& servers, OnboardingConfig config);

/**
 * Starts the settings DM with the server owner
 */
std::future<void> startOnboardingDM(IAgentRuntime runtime, Guild guild, UUID worldId);

/**
 * Starts onboarding for Telegram users by sending a deep link message to the group chat.
 *
 * @param {IAgentRuntime} runtime - The runtime instance for the agent
 * @param {World} world - The world object containing configuration
 * @param {any} chat - The Telegram chat object
 * @param {any[]} entities - Array of entities to search for the owner
 * @param {string} botUsername - Username of the Telegram bot
 * @returns {Promise<void>} A promise that resolves when the message is sent
 */
std::future<void> startTelegramOnboarding(IAgentRuntime runtime, World world, const std::any& chat, const std::vector<std::any>& entities, const std::string& botUsername);

} // namespace elizaos
