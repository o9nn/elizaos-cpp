#include "actions/dev-server.hpp"
#include "types.hpp"
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
 * Create a command that runs start in watch mode with auto-restart
 */
const dev = new Command()
  .name('dev')
  .description(
    'Start the project or plugin in development mode with auto-rebuild, detailed logging, and file change detection'
  )
  .option('-c, --configure', 'Reconfigure services and AI models (skips using saved configuration)')
  .option('--character [paths...]', 'Character file(s) to use - accepts paths or URLs')
  .option('-b, --build', 'Build the project before starting')
  .addOption(
    new Option('-p, --port <port>', 'Port to listen on (default: 3000)').argParser(validatePort)
  )
  .action(async (options: DevOptions) => {
    try {
      await startDevMode(options);
    } catch (error) {
      handleError(error);
    }
  });

// Re-for backward compatibility
* from './actions/dev-server';
* from './types';
* from './utils/build-utils';
* from './utils/file-watcher';
* from './utils/server-manager';

} // namespace elizaos
