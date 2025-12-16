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

;

import type { Guild } from 'discord.js';

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
const initCharacter = async ({
  runtime,
  config,
  actions,
  providers,
  evaluators,
}: {
  runtime: IAgentRuntime;
  config: OnboardingConfig;
  actions?: Action[];
  providers?: Provider[];
  evaluators?: Evaluator[];
}): Promise<void> => {
  if (actions) {
    for (const action of actions) {
      runtime.registerAction(action);
    }
  }

  if (providers) {
    for (const provider of providers) {
      runtime.registerProvider(provider);
    }
  }

  if (evaluators) {
    for (const evaluator of evaluators) {
      runtime.registerEvaluator(evaluator);
    }
  }

  // Register runtime events
  runtime.registerEvent('DISCORD_WORLD_JOINED', async (params: { server: Guild }) => {
    // TODO: Save settings config to runtime
    await initializeAllSystems(runtime, [params.server], config);
  });

  // when booting up into a server we're in, fire a connected event
  runtime.registerEvent('DISCORD_SERVER_CONNECTED', async (params: { server: Guild }) => {
    await initializeAllSystems(runtime, [params.server], config);
  });

  // Register runtime events
  runtime.registerEvent(
    'TELEGRAM_WORLD_JOINED',
    async (params: { world: World; entities: any[]; chat: any; botUsername: string }) => {
      await initializeOnboarding(runtime, params.world, config);
      await startTelegramOnboarding(
        runtime,
        params.world,
        params.chat,
        params.entities,
        params.botUsername
      );
    }
  );
};

/**
 * Initializes all systems for the given servers with the provided runtime, servers, and onboarding configuration.
 *
 * @param {IAgentRuntime} runtime - The runtime object that provides functionalities for the agent.
 * @param {Guild[]} servers - The list of servers to initialize systems for.
 * @param {OnboardingConfig} config - The configuration settings for onboarding.
 * @returns {Promise<void>} - A Promise that resolves when all systems have been initialized.
 */
std::future<void> initializeAllSystems(IAgentRuntime runtime, const std::vector<Guild>& servers, OnboardingConfig config);
      if (existingWorld?.metadata?.settings) {
        logger.debug('Onboarding already initialized for server', server.id);
        continue;
      }

      // Initialize onboarding for this server
      const world: World = {
        id: worldId,
        name: server.name,
        serverId: server.id,
        agentId: runtime.agentId,
        metadata: {
          roles: {
            [ownerId]: Role.OWNER,
          },
          ownership: {
            ownerId: ownerId,
          },
        },
      };
      await runtime.ensureWorldExists(world);
      // await initializeOnboarding(runtime, world, config);
      // await startOnboardingDM(runtime, server, worldId);
      console.log('world', world);
    }
  } catch (error) {
    logger.error('Error initializing systems:', error);
    throw error;
  }
}

/**
 * Starts the settings DM with the server owner
 */
std::future<void> startOnboardingDM(IAgentRuntime runtime, Guild guild, UUID worldId); for server ${guild.id}`);
      throw new Error(`Could not fetch owner with ID ${guild.ownerId}`);
    }

    const onboardingMessages = [
      'Hi! I need to collect some information to get set up. Is now a good time?',
      'Hey there! I need to configure a few things. Do you have a moment?',
      'Hello! Could we take a few minutes to get everything set up?',
    ];

    const randomMessage = onboardingMessages[Math.floor(Math.random() * onboardingMessages.length)];
    const msg = await owner.send(randomMessage);
    const roomId = createUniqueUuid(runtime, msg.channel.id);

    await runtime.ensureRoomExists({
      id: roomId,
      name: `Chat with ${owner.user.username}`,
      source: 'discord',
      type: ChannelType.DM,
      channelId: msg.channelId,
      serverId: guild.id,
      worldId: worldId,
    });

    const entity = await runtime.getEntityById(runtime.agentId);

    if (!entity) {
      await runtime.createEntity({
        id: runtime.agentId,
        names: [runtime.character.name],
        agentId: runtime.agentId,
      });
    }
    // Create memory of the initial message
    await runtime.createMemory(
      {
        agentId: runtime.agentId,
        entityId: runtime.agentId,
        roomId: roomId,
        content: {
          text: randomMessage,
          actions: ['BEGIN_ONBOARDING'],
        },
        createdAt: Date.now(),
      },
      'messages'
    );

    logger.info(`Started settings DM with owner ${owner.id} for server ${guild.id}`);
  } catch (error) {
    logger.error(`Error starting DM with owner: ${error}`);
    throw error;
  }
}

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
  });

  if (!ownerId) {
    logger.warn('no ownerId found');
  }

  const telegramClient = runtime.getService('telegram') as any;

  // Fallback: send deep link to the group chat
  const onboardingMessageDeepLink = [
    `Hello @${ownerUsername}! Could we take a few minutes to get everything set up?`,
    `Please click this link to start chatting with me: https://t.me/${botUsername}?start=onboarding`,
  ].join(' ');

  await telegramClient.messageManager.sendMessage(chat.id, { text: onboardingMessageDeepLink });
  logger.info(`Sent deep link to group chat ${chat.id} for owner ${ownerId || 'unknown'}`);
}

} // namespace elizaos
