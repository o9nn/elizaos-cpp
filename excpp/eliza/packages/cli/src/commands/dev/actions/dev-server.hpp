#include ".types.hpp"
#include ".utils/build-utils.hpp"
#include ".utils/file-watcher.hpp"
#include ".utils/server-manager.hpp"
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

/**
 * Start development mode with file watching and auto-restart
 *
 * Sets up a development environment with automatic rebuilding and server restarting when files change.
 */
std::future<void> startDevMode(DevOptions options); = context;
  const isProject = directoryType.type === 'elizaos-project';
  const isPlugin = directoryType.type === 'elizaos-plugin';
  const isMonorepo = directoryType.type === 'elizaos-monorepo';

  // Log project type
  if (isProject) {
    console.info('Identified as an ElizaOS project package');
  } else if (isPlugin) {
    console.info('Identified as an ElizaOS plugin package');
  } else if (isMonorepo) {
    console.info('Identified as an ElizaOS monorepo');
  } else {
    console.warn(
      `Not in a recognized ElizaOS project, plugin, or monorepo directory. Current directory is: ${directoryType.type}. Running in standalone mode.`
    );
  }

  // Prepare CLI arguments for the start command
  const cliArgs: string[] = [];

  // Pass through port option
  if (options.port) {
    cliArgs.push('--port', options.port.toString());
  }

  // Pass through configure option
  if (options.configure) {
    cliArgs.push('--configure');
  }

  // Handle characters - pass through to start command
  if (options.character) {
    if (Array.isArray(options.character)) {
      cliArgs.push('--character', ...options.character);
    } else {
      cliArgs.push('--character', options.character);
    }
  }

  // Function to rebuild and restart the server
  const rebuildAndRestart = async () => {
    try {
      // Ensure the server is stopped first
      await serverManager.stop();

      // Perform rebuild
      await performRebuild(context);

      console.log('✓ Rebuild successful, restarting...');

      // Start the server with the args
      await serverManager.start(cliArgs);
    } catch (error) {
      console.error(
        `Error during rebuild and restart: ${error instanceof Error ? error.message : String(error)}`
      );
      // Try to restart the server even if build fails
      if (!serverManager.process) {
        console.info('Attempting to restart server regardless of build failure...');
        await serverManager.start(cliArgs);
      }
    }
  };

  // Perform initial build if required
  if (isProject || isPlugin || isMonorepo) {
    const modeDescription = isMonorepo ? 'monorepo' : isProject ? 'project' : 'plugin';
    console.info(`Running in ${modeDescription} mode`);

    await performInitialBuild(context);
  }

  // Start the server initially
  await serverManager.start(cliArgs);

  // Set up file watching if we're in a project, plugin, or monorepo directory
  if (isProject || isPlugin || isMonorepo) {
    // Pass the rebuildAndRestart 
}

} // namespace elizaos
