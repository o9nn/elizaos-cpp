#include "commands.hpp"
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

/**
 * Tool bundle configuration
 * Converted from sweagent/tools/bundle.py
 */

/**
 * Bundle configuration
 */
struct BundleConfig {
    std::unordered_map<std::string, std::any> tools;
    std::optional<string | null> stateCommand;
};

/**
 * Tool bundle
 */
class Bundle {
  path: string;
  hiddenTools: string[];
  private _config?: BundleConfig;

  constructor(config: { path: string; hiddenTools?: string[] }) {
    this.path = config.path;
    this.hiddenTools = config.hiddenTools || [];
    this.validateTools();
  }

    // Validate that the bundle path exists

    // Load and validate config

    // Validate tools

      // Convert tool config to Command


} // namespace elizaos
