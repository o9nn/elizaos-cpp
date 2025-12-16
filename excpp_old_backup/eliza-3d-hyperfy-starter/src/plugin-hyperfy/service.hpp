#include "elizaos/core.hpp"
#include "hyperfy/src/core/createNodeClientWorld.js.hpp"
#include "managers//emote-manager.js.hpp"
#include "managers//message-manager.js.hpp"
#include "managers//voice-manager.js.hpp"
#include "managers/behavior-manager.js.hpp"
#include "managers/build-manager.js.hpp"
#include "managers/puppeteer-manager.js.hpp"
#include "physx/loadPhysX.js.hpp"
#include "systems/actions.js.hpp"
#include "systems/controls.hpp"
#include "systems/environment.js.hpp"
#include "systems/liveKit.js.hpp"
#include "systems/loader.hpp"
#include "utils.hpp"
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



class HyperfyService extends Service {
  static serviceType = 'hyperfy'
  capabilityDescription = 'Manages connection and interaction with a Hyperfy world.'

  private world: any | null = null
  private controls: AgentControls | null = null
  private isConnectedState: boolean = false
  private wsUrl: string | null = null
  private _currentWorldId: UUID | null = null
  private processedMsgIds: Set<string> = new Set()

  private playerNamesMap: Map<string, string> = new Map()
  private appearanceIntervalId: NodeJS.Timeout | null = null
  private appearanceSet: boolean = false
  private nameSet: boolean = false

  private connectionTime: number | null = null
  private behaviorManager: BehaviorManager;
  private emoteManager: EmoteManager;
  private messageManager: MessageManager;
  private voiceManager: VoiceManager;
  private puppeteerManager: PuppeteerManager;
  private buildManager: BuildManager;

  public get currentWorldId(): UUID | null {
    return this._currentWorldId
  }

      // Temporarily comment out AgentLoader to test for updateTransform error

      // HACK: Overwriting `chat.add` to prevent crashes caused by the original implementation.
      // This ensures safe handling of chat messages and avoids unexpected errors from undefined fields.

        // emit chat event
        // maybe broadcast

  /**
   * Uploads the character's avatar model and associated emote animations,
   * sets the avatar URL locally, updates emote hash mappings,
   * and notifies the server of the new avatar.
   * 
   * This function handles all assets required for character expression and animation.
   */

      // Apply avatar locally

      // Upload emotes

      // Notify server

    // Check if both are already set

        // Stop polling if both tasks are complete

        // Condition checks player/ID/network readiness for name, adds assetsUrl for avatar

             // --- Set Name (if not already done) ---

             // --- Set Avatar (if not already done AND assets URL ready) ---

             // Update waiting log

  /**
   * Checks if the service is currently connected to a Hyperfy world.
   */

  /**
   * Changes the agent's display name.
   */

          // 2. Update local state immediately
          // Update the name map

          // --- Use agentPlayer.modify for local update --- >

    // Pre-populate processed IDs with existing messages

      // Wait for player entity (ensures world/chat exist too)

      // Step 1: Identify new messages and update processed set
        // Check timestamp FIRST - only consider messages newer than connection time
            // console.debug(`[Chat Sub] Ignoring historical/old message ID ${msg?.id} (ts: ${messageTimestamp})`);
            // Ensure historical messages are marked processed if encountered *before* connectionTime was set (edge case)

        // Check if we've already processed this message ID (secondary check for duplicates)

      // Step 2: Process only the newly found messages


} // namespace elizaos
