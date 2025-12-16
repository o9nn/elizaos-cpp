#include "services/agents.hpp"
#include "services/audio.hpp"
#include "services/media.hpp"
#include "services/memory.hpp"
#include "services/messaging.hpp"
#include "services/server.hpp"
#include "services/system.hpp"
#include "types/base.hpp"
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
;
;
;

class ElizaClient {
  public readonly agents: AgentsService;
  public readonly messaging: MessagingService;
  public readonly memory: MemoryService;
  public readonly audio: AudioService;
  public readonly media: MediaService;
  public readonly server: ServerService;
  public readonly system: SystemService;

  constructor(config: ApiClientConfig) {
    // Initialize all services with the same config
    this.agents = new AgentsService(config);
    this.messaging = new MessagingService(config);
    this.memory = new MemoryService(config);
    this.audio = new AudioService(config);
    this.media = new MediaService(config);
    this.server = new ServerService(config);
    this.system = new SystemService(config);
  }

  /**
   * Create a new ElizaClient instance
   */
  static create(config: ApiClientConfig): ElizaClient {
    return new ElizaClient(config);
  }
}

} // namespace elizaos
