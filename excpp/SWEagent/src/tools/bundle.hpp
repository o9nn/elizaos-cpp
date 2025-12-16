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

;
;
;
;

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

  private validateTools(): void {
    // Validate that the bundle path exists
    if (!fs.existsSync(this.path)) {
      throw new Error(`Bundle path does not exist: ${this.path}`);
    }

    // Load and validate config
    const configPath = path.join(this.path, 'config.yaml');
    if (!fs.existsSync(configPath)) {
      throw new Error(`Bundle config not found: ${configPath}`);
    }

    const configContent = fs.readFileSync(configPath, 'utf-8');
    this._config = yaml.load(configContent) as BundleConfig;

    // Validate tools
    if (!this._config.tools || typeof this._config.tools !== 'object') {
      throw new Error('Bundle config must contain tools object');
    }
  }

  get stateCommand(): string | null | undefined {
    return this.config.stateCommand;
  }

  get config(): BundleConfig {
    if (!this._config) {
      this.validateTools();
    }
    return this._config!;
  }

  get commands(): Command[] {
    const commands: Command[] = [];

    for (const [name, toolConfig] of Object.entries(this.config.tools)) {
      if (this.hiddenTools.includes(name)) {
        continue;
      }

      // Convert tool config to Command
      const command = new Command({
        name,
        docstring: toolConfig.docstring || null,
        signature: toolConfig.signature || null,
        endName: toolConfig.end_name || null,
        arguments: toolConfig.arguments || [],
      });

      commands.push(command);
    }

    return commands;
  }
}

} // namespace elizaos
