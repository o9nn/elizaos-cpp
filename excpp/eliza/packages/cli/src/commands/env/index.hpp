#include "actions/edit.hpp"
#include "actions/interactive.hpp"
#include "actions/list.hpp"
#include "actions/reset.hpp"
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
;
;
;

// Create command for managing environment variables
const env = new Command()
  .name('env')
  .description('Manage environment variables and secrets');

// List subcommand
env
  .command('list')
  .description('List all environment variables')
  .option('--system', 'List only system information')
  .option('--local', 'List only local environment variables')
  .action(async (options: ListEnvOptions) => {
    try {
      await handleListCommand(options);
    } catch (error) {
      handleError(error);
    }
  });

// Edit local subcommand
env
  .command('edit-local')
  .description('Edit local environment variables')
  .option('-y, --yes', 'Automatically confirm prompts')
  .action(async (options: EditEnvOptions) => {
    try {
      await editEnvVars(options);
    } catch (error) {
      handleError(error);
    }
  });

// Reset subcommand
env
  .command('reset')
  .description(
    'Reset environment variables and clean up database/cache files (interactive selection)'
  )
  .option('-y, --yes', 'Automatically reset using default selections')
  .action(async (options: ResetEnvOptions) => {
    try {
      await resetEnv(options);
    } catch (error) {
      handleError(error);
    }
  });

// Interactive mode
env
  .command('interactive')
  .description('Interactive environment variable management')
  .option('-y, --yes', 'Automatically confirm prompts')
  .action(async (options: InteractiveEnvOptions) => {
    try {
      await showMainMenu(options);
    } catch (error) {
      handleError(error);
    }
  });

// Default command (show help if no subcommand provided)
env.action(() => {
  // Show available subcommands
  console.log(colors.bold('\nEliza Environment Variable Manager'));
  console.log('\nAvailable commands:');
  console.log('  list                  List all environment variables');
  console.log('  edit-local            Edit local environment variables');
  console.log(
    '  reset                 Reset environment variables and clean up database/cache files (interactive selection)'
  );
  console.log('  interactive           Start interactive environment variable manager');
  console.log('\nYou can also edit environment variables in the web UI:');
  console.log('  http://localhost:3000/settings');
});

// Re-for backward compatibility
* from './actions/edit';
* from './actions/interactive';
* from './actions/list';
* from './actions/reset';
* from './types';
* from './utils/directory-operations';
* from './utils/file-operations';
* from './utils/validation';

} // namespace elizaos
