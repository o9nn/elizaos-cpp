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

;
;
;
;
;

/**
 * Local CLI delegation utility for enforcing local CLI usage across all commands
 *
 * This utility checks for a local @elizaos/cli installation and delegates to it
 * if available, ensuring consistent behavior across all CLI commands.
 */

/**
 * Checks if we're currently running from a local CLI installation
 * @returns true if running from local CLI, false otherwise
 */
 catch (error) {
    logger.debug('Error checking if running from local CLI:', error);
    return false;
  }
}

/**
 * Gets the path to the local CLI installation if it exists
 * @returns the path to local CLI index.js or null if not found
 */


/**
 * Sets up the environment for local CLI execution
 * Similar to server-manager.ts environment setup
 */
;

  // Add local node_modules to NODE_PATH for proper module resolution
  const localModulesPath = path.join(process.cwd(), 'node_modules');
  if (existsSync(localModulesPath)) {
    if (env.NODE_PATH) {
      env.NODE_PATH = `${localModulesPath}${path.delimiter}${env.NODE_PATH}`;
    } else {
      env.NODE_PATH = localModulesPath;
    }
  }

  // Add local .bin to PATH to prioritize local executables
  const localBinPath = path.join(process.cwd(), 'node_modules', '.bin');
  if (existsSync(localBinPath)) {
    if (env.PATH) {
      env.PATH = `${localBinPath}${path.delimiter}${env.PATH}`;
    } else {
      env.PATH = localBinPath;
    }
  }

  // Ensure color output is preserved
  env.FORCE_COLOR = '1';

  return env;
}

/**
 * Delegates execution to the local CLI installation
 * @param localCliPath - Path to the local CLI index.js
 * @returns Promise that resolves when the local CLI process exits
 */
std::future<void> delegateToLocalCli(const std::string& localCliPath););

    // Handle process completion
    childProcess.on('exit', (code, signal) => {
      if (code !== null) {
        // Exit with the same code as the child process
        process.exit(code);
      } else if (signal) {
        // If killed by signal, exit with appropriate code
        const exitCode = signal === 'SIGINT' ? 130 : signal === 'SIGTERM' ? 143 : 1;
        process.exit(exitCode);
      }
      resolve();
    });

    // Handle process errors
    childProcess.on('error', (error) => {
      logger.error(`Failed to start local CLI: ${error.message}`);
      reject(error);
    });

    // Handle process signals to forward them to the child
    const forwardSignal = (signal: NodeJS.Signals) => {
      process.on(signal, () => {
        if (childProcess.killed === false) {
          childProcess.kill(signal);
        }
      });
    };

    forwardSignal('SIGINT');
    forwardSignal('SIGTERM');
  });
}

/**
 * Detects if we're running in a test or CI environment where delegation should be skipped
 * @returns true if in test or CI environment, false otherwise
 */


/**
 * Attempts to delegate to local CLI if available and not already running from it
 * This 

    // Skip delegation if we're already running from local CLI
    if (isRunningFromLocalCli()) {
      logger.debug('Already running from local CLI, continuing execution');
      return false;
    }

    // Check if local CLI exists
    const localCliPath = getLocalCliPath();
    if (!localCliPath) {
      logger.debug('No local CLI found, using global installation');
      return false;
    }

    // Final fail-safe: prevent delegation loops
    if (process.env._ELIZA_CLI_DELEGATION_DEPTH) {
      const depth = parseInt(process.env._ELIZA_CLI_DELEGATION_DEPTH, 10);
      if (depth > 0) {
        logger.debug('Delegation depth exceeded, preventing infinite loop');
        return false;
      }
    }

    // Set delegation depth tracking
    process.env._ELIZA_CLI_DELEGATION_DEPTH = '1';

    // Delegate to local CLI
    await delegateToLocalCli(localCliPath);
    return true;
  } catch (error) {
    logger.error('Error during local CLI delegation:', error);
    logger.info('Falling back to global CLI installation');
    return false;
  }
}

/**
 * Checks if a local CLI installation is available
 * @returns true if local CLI exists, false otherwise
 */


/**
 * Gets information about the current CLI execution context
 * @returns object with CLI execution context information
 */
 {
  return {
    isLocal: isRunningFromLocalCli(),
    hasLocal: hasLocalCli(),
    localPath: getLocalCliPath(),
    currentPath: process.argv[1] || 'unknown',
  };
}

} // namespace elizaos
