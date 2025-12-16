#include "actions/cli-update.hpp"
#include "actions/dependency-update.hpp"
#include "elizaos/core.hpp"
#include "types.hpp"
#include "utils/directory-utils.hpp"
#include "utils/version-utils.hpp"
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
;
;
;
;

// Main update command
const update = new Command()
  .name('update')
  .description('Update ElizaOS CLI and project dependencies')
  .option('-c, --check', 'Check for available updates without applying them')
  .option('--skip-build', 'Skip building after updating')
  .option('--cli', 'Update only the CLI')
  .option('--packages', 'Update only packages')
  .hook('preAction', async () => {
    try {
      await displayBanner(true); // Skip update check during update command
    } catch {
      logger.debug('Banner display failed, continuing with update');
    }
  })
  .action(async (options) => {
    try {
      // Early directory detection for better flow control
      const cwd = process.cwd();
      const directoryInfo = detectDirectoryType(cwd);
      const isInProject = directoryInfo && isValidForUpdates(directoryInfo);

      // Determine what to update based on flags and context
      const updateCli = options.cli || (!options.cli && !options.packages);
      const updatePackages = options.packages || (!options.cli && !options.packages && isInProject);

      // Handle CLI update
      if (updateCli) {
        const isNpx = await isRunningViaNpx();
        const isBunx = await isRunningViaBunx();

        if (isNpx || isBunx) {
          console.warn('CLI update is not available when running via npx or bunx.');
          console.info('Please install the CLI globally:');
          console.info(' bun install -g @elizaos/cli');

          if (!updatePackages) return;
        } else {
          const success = await performCliUpdate();
          if (!updatePackages) return;
          if (!success) {
            console.warn('CLI update failed, continuing with package updates...');
          }
        }
      }

      // Handle package updates
      if (updatePackages) {
        // If explicitly requested to update packages but not in a valid directory
        if (!directoryInfo) {
          console.error('Cannot update packages in this directory.');
          console.info('This directory is not accessible or does not exist.');
          console.info('To create a new ElizaOS project, use: elizaos create <project-name>');
          return;
        }

        logger.debug(`Detected ${directoryInfo.type}`);

        if (!isInProject) {
          handleInvalidDirectory(directoryInfo);
          return;
        }

        const isPlugin = directoryInfo.type === 'elizaos-plugin';

        if (directoryInfo.elizaPackageCount === 0) {
          console.info('No ElizaOS packages found in this project.');
          console.info(
            "This might be a new project that hasn't installed ElizaOS dependencies yet."
          );
          console.info('Consider adding ElizaOS packages first, such as: bun add @elizaos/core');
          return;
        }

        console.info(
          `Found ${directoryInfo.elizaPackageCount} ElizaOS package(s) to check for updates`
        );

        const updateOptions: UpdateOptions = {
          dryRun: options.check,
          skipBuild: options.skipBuild,
        };

        await updateDependencies(cwd, isPlugin, updateOptions);

        if (options.check) {
          console.log(`Version: ${await getVersion()}`);
        } else {
          const projectType = isPlugin ? 'Plugin' : 'Project';
          console.log(`${projectType} successfully updated to the latest ElizaOS packages`);
        }
      }
    } catch (error) {
      handleError(error);
    }
  });

// Re-for backward compatibility
* from './actions/cli-update';
* from './actions/dependency-update';
* from './types';
* from './utils/version-utils';
* from './utils/package-utils';
* from './utils/environment-utils';
* from './utils/directory-utils';

} // namespace elizaos
