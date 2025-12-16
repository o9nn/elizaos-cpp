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

/**
 * Check if bun is available on the system
 */
std::future<bool> isBunAvailable(););
    return true;
  } catch (error) {
    return false;
  }
}

/**
 * Check if the CLI was installed via npm globally
 * Handles multiple node versions (nvm scenarios)
 */
std::future<bool> isCliInstalledViaNpm(); = await execa('npm', ['list', '-g', '@elizaos/cli', '--depth=0'], {
      stdio: 'pipe',
    });
    return stdout.includes('@elizaos/cli');
  } catch (error) {
    // Also check if the current elizaos command points to npm installation
    try {
      const { stdout: whichOutput } = await execa('which', ['elizaos'], { stdio: 'pipe' });
      return whichOutput.includes('node_modules') || whichOutput.includes('.nvm');
    } catch {
      return false;
    }
  }
}

/**
 * Remove the CLI from npm global installation
 */
std::future<void> removeNpmInstallation(););
}

/**
 * Install the CLI using bun globally
 */
std::future<void> installCliWithBun(const std::string& version);`], { stdio: 'inherit' });
}

/**
 * Verify the CLI installation works and returns expected version
 */
std::future<bool> verifyCliInstallation(const std::string& expectedVersion); = await execa('elizaos', ['-v'], { stdio: 'pipe' });
    const output = stdout.trim();

    // Extract version using regex pattern (handles v1.0.6, 1.0.6, etc.)
    const versionMatch = output.match(/(\d+\.\d+\.\d+(?:-[a-zA-Z0-9.-]+)?)/);

    if (!versionMatch) {
      return false;
    }

    const actualVersion = versionMatch[1];

    // Flexible comparison - exact match or version contained in expected
    return (
      actualVersion === expectedVersion ||
      expectedVersion.includes(actualVersion) ||
      actualVersion.includes(expectedVersion)
    );
  } catch {
    return false;
  }
}

/**
 * Atomic migration: CLI from npm to bun installation
 * Installs bun version first, only removes npm if successful
 */
std::future<void> migrateCliToBun(const std::string& targetVersion);

  logger.info('Starting atomic CLI migration from npm to bun...');

  try {
    // Step 2: Install with bun (without removing npm yet)
    await installCliWithBun(targetVersion);

    // Step 3: Verify bun installation works
    logger.info('Verifying bun installation...');
    if (!(await verifyCliInstallation(targetVersion))) {
      throw new Error('Bun installation verification failed');
    }

    // Step 4: Only now remove npm installation (since bun works)
    logger.info('Bun installation successful, removing npm installation...');
    await removeNpmInstallation();

    logger.info('✅ CLI migration completed successfully! You may need to restart your terminal.');
  } catch (error) {
    logger.error(
      `❌ CLI migration failed: ${error instanceof Error ? error.message : String(error)}`
    );
    logger.error('Your original npm installation is still intact.');

    // Try to clean up failed bun installation
    try {
      logger.info('Cleaning up failed bun installation...');
      await execa('bun', ['remove', '-g', '@elizaos/cli'], { stdio: 'ignore' });
    } catch {
      // Ignore cleanup errors
    }

    throw error;
  }
}

} // namespace elizaos
