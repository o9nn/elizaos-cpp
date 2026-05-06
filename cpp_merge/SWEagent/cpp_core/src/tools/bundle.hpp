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
    std::unordered_map<std::string, std::string> tools;
    std::optional<std::string | null> stateCommand;
};

/**
 * Tool bundle
 */
class Bundle {
  path: std::string;
  hiddenTools: std::string[];
  private _config?: BundleConfig;

  /* constructor */ (config: { path: std::string; hiddenTools?: std::string[] }) {
    this.path = config.path;
    this.hiddenTools = config.hiddenTools || [];
    this.validateTools();
  }

    // Validate that the bundle path exists

    // Load and validate config

    // Validate tools

      // Convert tool config to Command


} // namespace elizaos
