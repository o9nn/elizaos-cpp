#include "services/agents.hpp"
#include "services/audio.hpp"
#include "services/auth.hpp"
#include "services/cdp.hpp"
#include "services/entities.hpp"
#include "services/gamification.hpp"
#include "services/jobs.hpp"
#include "services/media.hpp"
#include "services/memory.hpp"
#include "services/messaging.hpp"
#include "services/runs.hpp"
#include "services/server.hpp"
#include "services/sessions.hpp"
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
  public readonly sessions: SessionsService;
  public readonly runs: RunsService;
  public readonly entities: EntitiesService;
  public readonly cdp: CdpService;
  public readonly auth: AuthService;
  public readonly jobs: JobsService;
  public readonly gamification: GamificationService;

  private services: any[];

  constructor(config: ApiClientConfig) {
    // Initialize all services with the same config
    this.agents = new AgentsService(config);
    this.messaging = new MessagingService(config);
    this.memory = new MemoryService(config);
    this.audio = new AudioService(config);
    this.media = new MediaService(config);
    this.server = new ServerService(config);
    this.system = new SystemService(config);
    this.sessions = new SessionsService(config);
    this.runs = new RunsService(config);
    this.entities = new EntitiesService(config);
    this.cdp = new CdpService(config);
    this.auth = new AuthService(config);
    this.jobs = new JobsService(config);
    this.gamification = new GamificationService(config);
    
    // Keep track of all services for bulk operations
    this.services = [
      this.agents,
      this.messaging,
      this.memory,
      this.audio,
      this.media,
      this.server,
      this.system,
      this.sessions,
      this.runs,
      this.entities,
      this.cdp,
      this.auth,
      this.jobs,
      this.gamification,
    ];
  }

  /**
   * Set authentication token for all API requests
   * Call this after successful login to authenticate all subsequent requests
   * 
   * @param token JWT authentication token
   */
  setAuthToken(token: string) {
    for (const service of this.services) {
      if (service && typeof service.setAuthToken === 'function') {
        service.setAuthToken(token);
      }
    }
  }

  /**
   * Clear authentication token from all services
   * Call this on logout or when token expires
   */
  clearAuthToken() {
    for (const service of this.services) {
      if (service && typeof service.clearAuthToken === 'function') {
        service.clearAuthToken();
      }
    }
  }

  /**
   * Create a new ElizaClient instance
   */
  static create(config: ApiClientConfig): ElizaClient {
    return new ElizaClient(config);
  }
}

} // namespace elizaos
