#include ".hyperfy/src/core/systems/System.hpp"
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



struct LiveKitInitOptions {
    std::string wsUrl;
    std::string token;
};

class AgentLiveKit extends System {
  private room: Room | null = null;
  private audioSource: AudioSource | null = null;
  private localTrack: LocalAudioTrack | null = null;

  constructor(world: any) {
    super(world);
  }

    await dispose();

  // Framework stubs
  // init() {}


} // namespace elizaos
