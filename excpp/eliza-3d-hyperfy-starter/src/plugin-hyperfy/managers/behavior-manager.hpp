#include ".service.hpp"
#include ".templates.hpp"
#include ".utils.hpp"
#include "elizaos/core.hpp"
#include "guards.hpp"
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
;
;
;
;

const TIME_INTERVAL_MIN = 15000; // 15 seconds
const TIME_INTERVAL_MAX = 30000; // 30 seconds


class BehaviorManager {
  private isRunning: boolean = false;
  private runtime: IAgentRuntime;
  
  constructor(runtime: IAgentRuntime) {
    this.runtime = runtime;
  }

  /**
   * Starts the behavior loop if not already running and prerequisites are met.
   */
  public start(): void {
    if (this.isRunning) {
      logger.warn("[BehaviorManager] Already running");
      return;
    }

    this.isRunning = true;
    logger.info(`[BehaviorManager] Starting behavior loop for player`);

    this.runLoop().catch((err) =>
      logger.error("[BehaviorManager] Fatal error in run loop:", err)
    );
  }


  /**
   * Stops the behavior loop
   */
  public stop(): void {
    if (!this.isRunning) {
      logger.warn("[BehaviorManager] Not running");
      return;
    }

    this.isRunning = false;
    logger.info("[BehaviorManager] Stopped behavior loop");
  }

  /**
   * Main loop that waits for each behavior to finish
   */
  private async runLoop(): Promise<void> {
    while (this.isRunning) {
      try {
        await this.executeBehavior();
      } catch (error) {
        logger.error("[BehaviorManager] Error in behavior:", error);
      }

      // Short delay between behaviors
      const delay = TIME_INTERVAL_MIN + Math.floor(Math.random() * (TIME_INTERVAL_MAX - TIME_INTERVAL_MIN));
      await new Promise((resolve) => setTimeout(resolve, delay));
    }
  }

  private getService() {
    return this.runtime.getService<HyperfyService>(HyperfyService.serviceType);
  }

  /**
   * Executes a behavior
   */
  private async executeBehavior(): Promise<void> {
    const service = this.getService();
    if (!service) {
      logger.error("[BehaviorManager] Cannot start — service not available");
      return;
    }

    const world = service.getWorld();
    if (!world) {
      logger.error("[BehaviorManager] Cannot start — world not found");
      return;
    }

    const player = world.entities?.player;
    if (!player) {
      logger.error("[BehaviorManager] Cannot start — player entity not found");
      return;
    }
    // TODO: There may be slow post-processing in the bootstrap plugin's message handler.
    // Investigate long tail after message handling, especially in emitEvent or runtime methods.
    if (agentActivityLock.isActive()) {
      logger.info("[BehaviorManager] Skipping behavior — message activity in progress");
      return;
    }

    const _currentWorldId = service.currentWorldId;
    
    const elizaRoomId = createUniqueUuid(this.runtime, _currentWorldId || 'hyperfy-unknown-world')
    const entityId = createUniqueUuid(this.runtime, this.runtime.agentId);

    const newMessage: Memory = {
      id: createUniqueUuid(this.runtime, Date.now().toString()),
      content: {
        text: '',
        type: 'text',
      },
      roomId: elizaRoomId,
      worldId: _currentWorldId,
      entityId,
    };

    const state = await this.runtime.composeState(newMessage);

    const actionsData = await getHyperfyActions(
      this.runtime, 
      newMessage, 
      state, [
        'HYPERFY_GOTO_ENTITY',
        'HYPERFY_WALK_RANDOMLY',
        'HYPERFY_USE_ITEM',
        'HYPERFY_UNUSE_ITEM',
        'HYPERFY_AMBIENT_SPEECH',
        'REPLY',
        'IGNORE',
      ]
    );

    const actionsText = actionsData.length > 0 ? formatActions(actionsData) : '';

    const responsePrompt = composePromptFromState({ state, template: autoTemplate(actionsText) });

    // decide
    const response = await this.runtime.useModel(ModelType.TEXT_LARGE, {
      prompt: responsePrompt,
    });

    const parsedXml = parseKeyValueXml(response);

    console.log('****** response\n', parsedXml)

    const responseMemory = {
      content: {
        thought: parsedXml.thought,
        text: parsedXml.text,
        actions: parsedXml.actions,
        providers: parsedXml.providers,
        emote: parsedXml.emote,
      },
      entityId: createUniqueUuid(this.runtime, this.runtime.agentId),
      roomId: elizaRoomId,
    };

    const name = world.entities.player.data.name;
    await this.runtime.ensureConnection({
      entityId: entityId,
      roomId: elizaRoomId,
      userName: name,
      name,
      source: 'hyperfy',
      channelId: _currentWorldId,
      serverId: 'hyperfy',
      type: ChannelType.WORLD,
      worldId: _currentWorldId,
      userId: world.entities.player.data.id
    })

    const callback: HandlerCallback = async (responseContent: Content): Promise<Memory[]> => {
      console.info(`[Hyperfy Auto Callback] Received response: ${JSON.stringify(responseContent)}`)
      const emote = responseContent.emote as string;
      const callbackMemory: Memory = {
        id: createUniqueUuid(this.runtime, Date.now().toString()),
        entityId: this.runtime.agentId,
        agentId: this.runtime.agentId,
        content: {
          ...responseContent,
          channelType: ChannelType.WORLD,
          emote
        },
        roomId: elizaRoomId,
        createdAt: Date.now(),
      };

      await this.runtime.createMemory(callbackMemory, 'messages');

      if (emote) {
        const emoteManager = service.getEmoteManager();
        emoteManager.playEmote(emote);
      }

      if (responseContent.text) {
        const messageManager = service.getMessageManager();
        messageManager.sendMessage(responseContent.text)
      }
      
      return [];
    };
    
    await this.runtime.processActions(
      newMessage,
      [responseMemory],
      state,
      callback
    );

    await this.runtime.evaluate(newMessage, state, true, callback, [
      responseMemory,
    ]);
  }
}

} // namespace elizaos
