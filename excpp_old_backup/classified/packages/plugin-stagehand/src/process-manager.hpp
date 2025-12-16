#include "elizaos/core.hpp"
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



class StagehandProcessManager {
  private process: ChildProcess | null = null;
  private isRunning = false;
  private serverPort: number;
  private binaryPath: string | null = null;

  constructor(serverPort: number = 3456) {
    this.serverPort = serverPort;
    this.binaryPath = this.findBinary();
  }

    // Try architecture-specific binary first, then fall back to platform-only

    // Get the directory where this module is located

    // Check if we're in a Docker container

    // Possible locations for the binary
      // Docker/container paths first

      // For local dev, prioritize JS version to avoid signing issues on macOS

      // When running from plugin directory - arch-specific first
      // When packaged with agentserver
      // When in node_modules
      // Development fallback - run via node
      // Docker fallback - if binary not found, try JS file

    // If no binary found, check if we can run from source

        // Pass through relevant environment variables
        // Pass through display for headful mode in container

      // Determine if we're running a binary, JS file, or TS file

        // Run the binary directly
        // Run TypeScript via tsx (development mode)
        // Run via node (JS file)

        // Check if server is ready

      // Wait for server to be ready

        // Try to establish WebSocket connection for health check

        // Server not ready yet, continue waiting

      // Try graceful shutdown first

      // Force kill after timeout


} // namespace elizaos
