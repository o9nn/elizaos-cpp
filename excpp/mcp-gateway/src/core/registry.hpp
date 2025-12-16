#include ".types/index.hpp"
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



class GatewayRegistry {
  private tools = new Map<string, AggregatedTool>();
  private resources = new Map<string, AggregatedResource>();
  private prompts = new Map<string, AggregatedPrompt>();
  private config: GatewayConfig;
  private logger: Console;

  constructor(config: GatewayConfig, logger: Console = console) {
    this.config = config;
    this.logger = logger;
  }

  /**
   * Refresh the registry by collecting capabilities from all connected servers
   */


} // namespace elizaos
