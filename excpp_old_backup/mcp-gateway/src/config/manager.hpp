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



class ConfigManager {
  private config: GatewayConfig | null = null;

  loadFromFile(filePath: string): GatewayConfig {
    try {
      const fileContent = readFileSync(filePath, 'utf-8');
      
      // Substitute environment variables in the content
      const processedContent = this.substituteEnvVars(fileContent);
      
      let rawConfig: unknown;
      
      if (filePath.endsWith('.yaml') || filePath.endsWith('.yml')) {
        rawConfig = parseYAML(processedContent);
      } else {
        rawConfig = JSON.parse(processedContent);
      }

      this.config = GatewayConfigSchema.parse(rawConfig);
      this.validateConfig(this.config);
      
      return this.config;
    } catch (error) {
      throw new Error(`Failed to load config from ${filePath}: ${error}`);
    }
  }

  /**
   * Substitute environment variables in config content
   * Supports ${VAR_NAME} and ${VAR_NAME:-default} syntax
   */

      // If no value and no default, return empty string or keep original


} // namespace elizaos
