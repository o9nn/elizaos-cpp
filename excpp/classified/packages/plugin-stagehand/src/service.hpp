#include "elizaos/core.hpp"
#include "process-manager.js.hpp"
#include "types.hpp"
#include "websocket-client.js.hpp"
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



class BrowserSession {
  constructor(
    public id: string,
    public createdAt: Date = new Date()
  ) {}

class StagehandService extends Service {
  static serviceType = StagehandServiceType.STAGEHAND;
  capabilityDescription = 'Browser automation service using Stagehand for web interactions';

  private sessions: Map<string, BrowserSession> = new Map();
  private currentSessionId: string | null = null;
  private processManager: StagehandProcessManager;
  private client: StagehandWebSocketClient;
  private isInitialized = false;

  constructor(protected runtime: IAgentRuntime) {
    super(runtime);
    const port = parseInt(runtime.getSetting('STAGEHAND_SERVER_PORT') || '3456', 10);
    this.processManager = new StagehandProcessManager(port);
    this.client = new StagehandWebSocketClient(`ws://localhost:${port}`);
  }

      // Start the Stagehand server process
        // Don't throw - allow the service to start but in a degraded state

      // Initialize WebSocket client

      // Process manager is already started in the static start method

      // Wait for server to be ready (Playwright installed, etc.)

    // Try to get current session first

    // Create a new session if none exists


} // namespace elizaos
