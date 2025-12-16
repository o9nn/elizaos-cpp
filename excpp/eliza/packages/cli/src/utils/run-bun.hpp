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
std::future<void> runBunCommand(const std::vector<std::string>& args, const std::string& cwd);
    console.info('✅ Using CI-optimized flags for faster installation...');
  }

  try {
    await execa('bun', finalArgs, { cwd, stdio: 'inherit' });
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
      await execa('bun', args, { cwd, stdio: 'inherit' });
    } else {
      throw error;
    }
  }
}

} // namespace elizaos
