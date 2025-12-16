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

#!/usr/bin/env node
process.env.NODE_OPTIONS = '--no-deprecation';
process.env.NODE_NO_WARNINGS = '1';

;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
import path, { dirname } from 'node:path';
;
;

process.on('SIGINT', () => process.exit(0));
process.on('SIGTERM', () => process.exit(0));

/**
 * Asynchronous 
 */
async );
  }

  // Check for --no-auto-install flag early (before command parsing)
  if (process.argv.includes('--no-auto-install')) {
    process.env.ELIZA_NO_AUTO_INSTALL = 'true';
  }

  // For ESM modules we need to use import.meta.url instead of __dirname
  const __filename = fileURLToPath(import.meta.url);
  const __dirname = dirname(__filename);

  // Find package.json relative to the current file
  const packageJsonPath = path.resolve(__dirname, '../package.json');

  // Add a simple check in case the path is incorrect
  let version = '0.0.0'; // Fallback version
  if (!existsSync(packageJsonPath)) {
  } else {
    const packageJson = JSON.parse(readFileSync(packageJsonPath, 'utf-8'));
    version = packageJson.version;
  }

  // Check for built-in flags that exit early (before preAction hook runs)
  const args = process.argv.slice(2);
  const isUpdateCommand = args.includes('update');
  const willShowBanner = args.length === 0;

  // Show update notification for all commands except:
  // - when banner will show (it handles its own notification)
  // - when running update command
  if (!willShowBanner && !isUpdateCommand) {
    const currentVersion = getVersion();
    await checkAndShowUpdateNotification(currentVersion);
  }

  const program = new Command()
    .name('elizaos')
    .version(version, '-v, --version', 'output the version number')
    .option('--no-emoji', 'Disable emoji output')
    .option('--no-auto-install', 'Disable automatic Bun installation');

  // Add global options but hide them from global help
  // They will still be passed to all commands for backward compatibility
  // Note: Removed --remote-url global option as it conflicts with subcommand options

  program
    .addCommand(create)
    .addCommand(monorepo)
    .addCommand(plugins)
    .addCommand(agent)
    .addCommand(tee)
    .addCommand(start)
    .addCommand(update)
    .addCommand(test)
    .addCommand(env)
    .addCommand(dev)
    .addCommand(publish);

  // if no args are passed, display the banner (it will handle its own update check)
  if (process.argv.length === 2) {
    await displayBanner(false); // Let banner handle update check and show enhanced notification
  }

  await program.parseAsync();
}

main().catch((error) => {
  logger.error('An error occurred:', error);
  process.exit(1);
});

} // namespace elizaos
