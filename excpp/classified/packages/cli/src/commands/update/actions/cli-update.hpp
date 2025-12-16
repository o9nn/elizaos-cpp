#include ".types.hpp"
#include ".utils/version-utils.hpp"
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
 * Update CLI to latest version
 *
 * Handles CLI updates with automatic migration from npm to bun when appropriate, and supports both global and local installation scenarios.
 */
std::future<bool> performCliUpdate(GlobalUpdateOptions = {} options);
      latestVersion = fetchedVersion;
    } else {
      latestVersion = targetVersion;
    }

    const { needsUpdate } = checkVersionNeedsUpdate(currentVersion, latestVersion);
    if (!needsUpdate) {
      console.log(`CLI is already at the latest version (${currentVersion}) [✓]`);
      return true;
    }

    console.log(`Updating CLI from ${currentVersion} to ${latestVersion}...`);

    // Check if CLI is installed via npm and migrate to bun (unless skipped)
    if (!options.skipBunMigration) {
      const npmInstallation = await isCliInstalledViaNpm();
      if (npmInstallation) {
        logger.info('Detected npm installation, migrating to bun...');
        try {
          await migrateCliToBun(latestVersion);
          console.log(`CLI updated successfully to version ${latestVersion} [✓]`);
          return true;
        } catch (migrationError) {
          logger.warn('Migration to bun failed, falling back to npm update...');
          logger.debug(
            'Migration error:',
            migrationError instanceof Error ? migrationError.message : String(migrationError)
          );
          // Fallback to npm installation since bun failed
          try {
            await bunExecInherit('npm', ['install', '-g', `@elizaos/cli@${latestVersion}`]);
            console.log(`CLI updated successfully to version ${latestVersion} [✓]`);
            return true;
          } catch (npmError) {
            throw new Error(
              `Both bun migration and npm fallback failed. Bun: ${migrationError instanceof Error ? migrationError.message : String(migrationError)}, npm: ${npmError instanceof Error ? npmError.message : String(npmError)}`
            );
          }
        }
      }
    }

    // Standard bun installation (no npm installation detected or migration skipped)
    try {
      await bunExecInherit('bun', ['add', '-g', `@elizaos/cli@${latestVersion}`]);
      console.log(`CLI updated successfully to version ${latestVersion} [✓]`);
      return true;
    } catch (bunError) {
      console.error('Bun installation not found. Please install bun first:');
      console.error('  curl -fsSL https://bun.sh/install | bash');
      console.error('  # or');
      console.error('  npm install -g bun');
      logger.debug('Bun error:', bunError instanceof Error ? bunError.message : String(bunError));
      return false;
    }
  } catch (error) {
    console.error(`CLI update failed: ${error instanceof Error ? error.message : String(error)}`);
    return false;
  }
}

} // namespace elizaos
