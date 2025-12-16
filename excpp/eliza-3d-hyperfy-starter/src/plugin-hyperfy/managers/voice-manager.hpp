#include ".events.hpp"
#include ".service.hpp"
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

using LiveKitAudioData = {
  participant: string;
  buffer: Buffer;
};

class VoiceManager {
  private runtime: IAgentRuntime;
  private userStates: Map<
    string,
    {
      buffers: Buffer[];
      totalLength: number;
      lastActive: number;
      transcriptionText: string;
    }
  > = new Map();
  private processingVoice: boolean = false;
  private transcriptionTimeout: NodeJS.Timeout | null = null;

  constructor(runtime: IAgentRuntime) {
    this.runtime = runtime;
  }

  start() {
    const service = this.getService();
    const world = service.getWorld();

    world.livekit.on('audio', async (data: LiveKitAudioData) => {
      
      
        const avgAmplitude = sum / sampleCount;
        return avgAmplitude > threshold;
      }

      const playerId = data.participant;
      if (!this.userStates.has(playerId)) {
        this.userStates.set(playerId, {
          buffers: [],
          totalLength: 0,
          lastActive: Date.now(),
          transcriptionText: '',
        });
      }

      const pcmBuffer = data.buffer;
      if (isLoudEnough(pcmBuffer)) {
        this.handleUserBuffer(playerId, pcmBuffer)
      }
    })
  }

  async handleUserBuffer(playerId, buffer) {
    const state = this.userStates.get(playerId);
    try {
      state?.buffers.push(buffer);
      state!.totalLength += buffer.length;
      state!.lastActive = Date.now();
      this.debouncedProcessTranscription(playerId);
    } catch (error) {
      console.error(`Error processing buffer for user ${playerId}:`, error);
    }
  }

  async debouncedProcessTranscription(
    playerId: UUID,
  ) {
    const DEBOUNCE_TRANSCRIPTION_THRESHOLD = 1500; // wait for 1.5 seconds of silence

    if (this.processingVoice) {
      const state = this.userStates.get(playerId);
      state.buffers.length = 0;
      state.totalLength = 0;
      return;
    }

    if (this.transcriptionTimeout) {
      clearTimeout(this.transcriptionTimeout);
    }

    this.transcriptionTimeout = setTimeout(async () => {
      await agentActivityLock.run(async () => {
        this.processingVoice = true;
        try {
          await this.processTranscription(playerId);

          // Clean all users' previous buffers
          this.userStates.forEach((state, _) => {
            state.buffers.length = 0;
            state.totalLength = 0;
            state.transcriptionText = '';
          });
        } finally {
          this.processingVoice = false;
        }
      })
    }, DEBOUNCE_TRANSCRIPTION_THRESHOLD) as unknown as NodeJS.Timeout;
  }

  private async processTranscription(
    playerId: UUID,
  ) {
    const state = this.userStates.get(playerId);
    if (!state || state.buffers.length === 0) return;
    try {
      const inputBuffer = Buffer.concat(state.buffers, state.totalLength);

      state.buffers.length = 0; // Clear the buffers
      state.totalLength = 0;
      // Convert Opus to WAV
      const wavHeader = getWavHeader(inputBuffer.length, 48000);
      const wavBuffer = Buffer.concat([wavHeader, inputBuffer]);
      logger.debug('Starting transcription...');

      const transcriptionText = await this.runtime.useModel(ModelType.TRANSCRIPTION, wavBuffer);

      console.log("[VOICE MANAGER] Transcrtion: ", transcriptionText)
      

      if (transcriptionText && isValidTranscription(transcriptionText)) {
        state.transcriptionText += transcriptionText;
      }

      if (state.transcriptionText.length) {
        const finalText = state.transcriptionText;
        state.transcriptionText = '';
        await this.handleMessage(finalText, playerId);
      }
    } catch (error) {
      console.error(`Error transcribing audio for user ${playerId}:`, error);
    }
  }

  private async handleMessage(
    message: string,
    playerId: UUID,
  ) {
    try {
      if (!message || message.trim() === '' || message.length < 3) {
        return { text: '', actions: ['IGNORE'] };
      }
      const service = this.getService();
      const world = service.getWorld();

      const playerInfo = world.entities.getPlayer(playerId);
      const userName = playerInfo.data.name;
      const name = userName;
      const _currentWorldId = service.currentWorldId;
      const channelId = _currentWorldId;
      const roomId = createUniqueUuid(this.runtime, _currentWorldId || 'hyperfy-unknown-world')
      const entityId = createUniqueUuid(this.runtime, playerId) as UUID

      const type = ChannelType.WORLD;

      // Ensure connection for the sender entity
      await this.runtime.ensureConnection({
        entityId,
        roomId,
        userName,
        name,
        source: 'hyperfy',
        channelId,
        serverId: 'hyperfy',
        type: ChannelType.WORLD,
        worldId: _currentWorldId,
        userId: playerId
      })

      const memory: Memory = {
        id: createUniqueUuid(this.runtime, `${channelId}-voice-message-${Date.now()}`),
        agentId: this.runtime.agentId,
        entityId: entityId,
        roomId,
        content: {
          text: message,
          source: 'hyperfy',
          name: name,
          userName: userName,
          isVoiceMessage: true,
          channelType: type,
        },
        createdAt: Date.now(),
      };

      const callback: HandlerCallback = async (content: Content, _files: any[] = []) => {
        console.info(`[Hyperfy Voice Chat Callback] Received response: ${JSON.stringify(content)}`)
        try {
          const responseMemory: Memory = {
            id: createUniqueUuid(this.runtime, `${memory.id}-voice-response-${Date.now()}`),
            entityId: this.runtime.agentId,
            agentId: this.runtime.agentId,
            content: {
              ...content,
              name: this.runtime.character.name,
              inReplyTo: memory.id,
              isVoiceMessage: true,
              channelType: type,
            },
            roomId,
            createdAt: Date.now(),
          };

          await this.runtime.createMemory(responseMemory, 'messages');

          if (responseMemory.content.text?.trim()) {
            const responseStream = await this.runtime.useModel(
              ModelType.TEXT_TO_SPEECH,
              content.text
            );
            if (responseStream) {
              const audioBuffer = await convertToAudioBuffer(responseStream);
              const emoteManager = service.getEmoteManager();
              const emote = content.emote as string || "TALK";
              emoteManager.playEmote(emote);
              await this.playAudio(audioBuffer);
            }
          }

          return [responseMemory];
        } catch (error) {
          console.error('Error in voice message callback:', error);
          return [];
        }
      };

      agentActivityLock.enter();
      // Emit voice-specific events
      this.runtime.emitEvent([hyperfyEventType.VOICE_MESSAGE_RECEIVED], {
        runtime: this.runtime,
        message: memory,
        callback,
        onComplete: () => {
          agentActivityLock.exit();
        },
      });
    } catch (error) {
      console.error('Error processing voice message:', error);
    }
  }

  async playAudio(audioBuffer) {
    if (this.processingVoice) {
      logger.info(`[VOICE MANAER] Current voice is processing.....`)
      return;
    }

    const service = this.getService();
    const world = service.getWorld();
    this.processingVoice = true;

    try {
      await world.livekit.publishAudioStream(audioBuffer);
    } catch(error) {
      logger.error(error)
    } finally {
      this.processingVoice = false;
    }
  }

  private getService() {
    return this.runtime.getService<HyperfyService>(HyperfyService.serviceType);
  }

  
}
} // namespace elizaos
