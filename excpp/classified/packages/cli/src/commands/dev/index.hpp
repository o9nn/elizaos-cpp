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
    new Option('-p, --port <port>', 'Port to listen on (default: 3000)').argParser((value) => {
      const port = Number.parseInt(value, 10);
      if (Number.isNaN(port) || port < 0 || port > 65535) {
        throw new Error('Port must be a number between 0 and 65535');
      }
      return port;
    })
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
