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



using LiveKitAudioData = {

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

      bool isLoudEnough(Buffer pcmBuffer, auto threshold = 1000);

          // Clean all users' previous buffers

      // Convert Opus to WAV

      bool isValidTranscription(const std::string& text);

      // Ensure connection for the sender entity

      // Emit voice-specific events

  
} // namespace elizaos
