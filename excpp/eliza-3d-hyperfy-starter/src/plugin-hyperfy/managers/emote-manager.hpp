#include ".constants.js.hpp"
#include ".hyperfy/src/core/extras/playerEmotes.js.hpp"
#include ".service.js.hpp"
#include ".utils.hpp"
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









class EmoteManager {
  private emoteHashMap: Map<string, string>
  private currentEmoteTimeout: NodeJS.Timeout | null
  private movementCheckInterval: NodeJS.Timeout | null = null;
  private runtime: IAgentRuntime;

  constructor(runtime) {
    this.runtime = runtime;
    this.emoteHashMap = new Map()
    this.currentEmoteTimeout = null
  }

  async uploadEmotes() {
    for (const emote of EMOTES_LIST) {
      try {
        const moduleDirPath = getModuleDirectory();
        const emoteBuffer = await fs.readFile(moduleDirPath + emote.path);
        const emoteMimeType = "model/gltf-binary";

        const emoteHash = await hashFileBuffer(emoteBuffer);
        const emoteExt = emote.path.split(".").pop()?.toLowerCase() || "glb";
        const emoteFullName = `${emoteHash}.${emoteExt}`;
        const emoteUrl = `asset://${emoteFullName}`;

        console.info(
          `[Appearance] Uploading emote '${emote.name}' as ${emoteFullName} (${(emoteBuffer.length / 1024).toFixed(2)} KB)`
        );

        const emoteFile = new File([emoteBuffer], path.basename(emote.path), {
          type: emoteMimeType,
        });

        const service = this.getService();
        const world = service.getWorld();
        const emoteUploadPromise = world.network.upload(emoteFile);
        const emoteTimeout = new Promise((_resolve, reject) =>
          setTimeout(() => reject(new Error("Upload timed out")), 30000)
        );

        await Promise.race([emoteUploadPromise, emoteTimeout]);

        this.emoteHashMap.set(emote.name, emoteFullName);
        console.info(
          `[Appearance] Emote '${emote.name}' uploaded: ${emoteUrl}`
        );
      } catch (err: any) {
        console.error(
          `[Appearance] Failed to upload emote '${emote.name}': ${err.message}`,
          err.stack
        );
      }
    }
  }

  playEmote(name: string) {
    const fallback = (Emotes as Record<string, string>)[name];
    const hashName = this.emoteHashMap.get(name) || fallback;
    const service = this.getService();
    const world = service.getWorld();

    if (!hashName) {
      console.warn(`[Emote] Emote '${name}' not found.`);
      return;
    }

    const agentPlayer = world?.entities?.player;
    if (!agentPlayer) {
      console.warn("[Emote] Player entity not found.");
      return;
    }

    const emoteUrl = hashName.startsWith('asset://') ? hashName : `asset://${hashName}`;
    agentPlayer.data.effect = agentPlayer.data.effect || {};
    agentPlayer.data.effect.emote = emoteUrl;

    console.info(`[Emote] Playing '${name}' → ${emoteUrl}`);

    this.clearTimers();

    // Get duration from EMOTES_LIST
    const emoteMeta = EMOTES_LIST.find(e => e.name === name);
    const duration = emoteMeta?.duration || 1.5;

    this.movementCheckInterval = setInterval(() => {
      if (agentPlayer.moving) {
        logger.info(`[EmoteManager] '${name}' cancelled early due to movement`);
        this.clearEmote(agentPlayer);
      }
    }, 100);

    this.currentEmoteTimeout = setTimeout(() => {
      if (agentPlayer.data.effect?.emote === emoteUrl) {
        logger.info(`[EmoteManager] '${name}' finished after ${duration}s`);
        this.clearEmote(agentPlayer);
      }
    }, duration * 1000);
  }

  private clearEmote(player) {
    if (player.data?.effect) {
      player.data.effect.emote = null;
    }
    this.clearTimers();
  }

  private clearTimers() {
    if (this.currentEmoteTimeout) {
      clearTimeout(this.currentEmoteTimeout);
      this.currentEmoteTimeout = null;
    }
    if (this.movementCheckInterval) {
      clearInterval(this.movementCheckInterval);
      this.movementCheckInterval = null;
    }
  }

  private getService() {
    return this.runtime.getService<HyperfyService>(HyperfyService.serviceType);
  }
}

} // namespace elizaos
