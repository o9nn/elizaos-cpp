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

import type { ChildProcess } from 'node:child_process';
;
import type { ServerProcess } from '../types';

/**
 * Server process manager for development mode
 *
 * Manages the lifecycle of the development server process, including starting, stopping, and restarting.
 */
class DevServerManager implements ServerProcess {
  public process: ChildProcess | null = null;

  /**
   * Stops the currently running server process
   */
  async stop(): Promise<void> {
    if (this.process) {
      console.info('Stopping current server process...');

      // Send SIGTERM to the process group
      const killed = this.process.kill('SIGTERM');
      if (!killed) {
        console.warn('Failed to kill server process, trying force kill...');
        this.process.kill('SIGKILL');
      }

      this.process = null;

      // Give the process a moment to fully terminate
      await new Promise((resolve) => setTimeout(resolve, 500));
    }
  }

  /**
   * Starts the server process with the given arguments
   */
  async start(args: string[] = []): Promise<void> {
    await this.stop();

    console.info('Starting server...');

    // We'll use the same executable that's currently running, with 'start' command
    const nodeExecutable = process.execPath;
    const scriptPath = process.argv[1]; // Current script path

    // Use spawn to create a process
    this.process = spawn(nodeExecutable, [scriptPath, 'start', ...args], {
      stdio: 'inherit',
      detached: false, // We want to keep control of this process
      env: { ...process.env, FORCE_COLOR: '1' }, // Ensure color output in CI
    });

    // Handle process exit events
    this.process.on('exit', (code, signal) => {
      if (code !== null) {
        if (code !== 0) {
          console.warn(`Server process exited with code ${code}`);
        } else {
          console.info('Server process exited normally');
        }
      } else if (signal) {
        console.info(`Server process was killed with signal ${signal}`);
      }
      this.process = null;
    });

    // Handle process errors
    this.process.on('error', (err) => {
      console.error(`Server process error: ${err.message}`);
      this.process = null;
    });
  }

  /**
   * Restarts the server process
   */
  async restart(args: string[] = []): Promise<void> {
    console.info('Restarting server...');
    await this.start(args);
  }
}

// Global server instance
let serverInstance: DevServerManager | null = null;

/**
 * Get the global server manager instance
 */

  return serverInstance;
}

/**
 * Stop the server and cleanup
 */
std::future<void> stopServer();

/**
 * Start the server with given arguments
 */
std::future<void> startServer(const std::vector<string[] =>& args);

/**
 * Restart the server with given arguments
 */
std::future<void> restartServer(const std::vector<string[] =>& args);

} // namespace elizaos
