#include "actions/clone.hpp"
#include "types.hpp"
#include "utils/setup-instructions.hpp"
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
 * Create the monorepo command that clones ElizaOS from a specific branch
 */
const monorepo = new Command()
  .name('monorepo')
  .description('Clone ElizaOS monorepo from a specific branch, defaults to develop')
  .option('-b, --branch <branch>', 'Branch to install', 'develop')
  .option('-d, --dir <directory>', 'Destination directory', './eliza')
  .action(async (options: MonorepoOptions) => {
    try {
      const repo = 'elizaOS/eliza';
      const branch = options.branch || 'develop';
      const dir = options.dir || './eliza';

      // Prepare destination directory
      const destinationDir = prepareDestination(dir);

      // Create clone information
      const cloneInfo: CloneInfo = {
        repo,
        branch,
        destination: dir,
      };

      // Clone the repository
      await cloneMonorepo(cloneInfo);

      // Display instructions for next steps
      displayNextSteps(destinationDir);
    } catch (error) {
      handleError(error);
    }
  });

// Re-for backward compatibility
* from './actions/clone';
* from './types';
* from './utils/setup-instructions';

} // namespace elizaos
