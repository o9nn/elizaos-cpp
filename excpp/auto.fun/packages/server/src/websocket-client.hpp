#include "redis.hpp"
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



class WebSocketClient {
  // No longer needs Durable Object reference
  // private webSocketDO: DurableObjectNamespace | null = null;

  constructor(/*  - Remove if Env is not used */) {
    // Initialization logic removed - we use the imported singleton manager
    // this.webSocketDO = (env as any).WEBSOCKET_DO || null;
  }

  // Send a message to a specific room (token or global)

  // Send a message to a specific client by ID

  // Helper that returns an object with direct emit method for chaining

// Helper function to get websocket client instance
// Since WebSocketClient now just wraps the singleton manager,
// we might not even need this class/function anymore.
// Code using getWebSocketClient() could potentially just import webSocketManager directly.
// However, keeping it maintains the existing interface.

WebSocketClient getWebSocketClient();

} // namespace elizaos
