#include "bun-exec.hpp"
#include "bun-installation-helper.hpp"
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

/**
 * Asynchronously runs a 'bun' command with the provided arguments in the specified directory.
 * @param {string[]} args - The arguments to pass to the 'bun' command.
 * @param {string} cwd - The current working directory in which to run the command.
 * @returns {Promise<void>} A Promise that resolves when the command has finished running.
 */
std::future<void> runBunCommand(const std::vector<std::string>& args, const std::string& cwd, auto silent = false);
    console.info('✅ Using CI-optimized flags for faster installation...');
  }

  try {
    const result = silent
      ? await bunExec('bun', finalArgs, { cwd })
      : await bunExecInherit('bun', finalArgs, { cwd });

    // Using result.success for clarity - it's a boolean that indicates exitCode === 0
    if (silent && !result.success) {
      throw new Error(
        `Bun command failed with exit code ${result.exitCode}: ${result.stderr || result.stdout}`
      );
    }
  } catch (error: any) {
    if (error.code === 'ENOENT' || error.message?.includes('bun: command not found')) {
      throw new Error(`Bun command not found. ${displayBunInstallationTipCompact()}`);
    }

    // If CI-optimized install fails, try again with basic args
    if (
      isCI &&
      isInstallCommand &&
      (error.message?.includes('frozen-lockfile') || error.message?.includes('install'))
    ) {
      console.warn('CI-optimized install failed, retrying with basic args...');
      const retryResult = silent
        ? await bunExec('bun', args, { cwd })
        : await bunExecInherit('bun', args, { cwd });

      // Using result.success for clarity - it's a boolean that indicates exitCode === 0
      if (silent && !retryResult.success) {
        throw new Error(
          `Bun command failed with exit code ${retryResult.exitCode}: ${retryResult.stderr || retryResult.stdout}`
        );
      }
    } else {
      throw error;
    }
  }
}

} // namespace elizaos
