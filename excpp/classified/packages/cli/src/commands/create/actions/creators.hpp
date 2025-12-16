#include ".utils.hpp"
#include "setup.hpp"
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
;
;

/**
 * Handles interactive configuration setup for projects
 * This includes database configuration, AI model setup, and Ollama fallback configuration
 */
std::future<void> handleInteractiveConfiguration(const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel);/.env`;

  // Handle PostgreSQL configuration
  if (database === 'postgres') {
    await promptAndStorePostgresUrl(envFilePath);
  }

  // Handle AI model configuration
  if (aiModel !== 'local' || embeddingModel) {
    if (aiModel !== 'local') {
      await setupAIModelConfig(aiModel, envFilePath, false);
    }
    if (embeddingModel) {
      await setupEmbeddingModelConfig(embeddingModel, envFilePath, false);
    }
  }
}

/**
 * wraps the creation process with cleanup handlers that remove the directory
 * if the user interrupts with ctrl-c during installation
 */
async ...`));
      try {
        rmSync(targetDir, { recursive: true, force: true });
        console.info('Cleanup completed.');
      } catch (error) {
        console.error(colors.red('Error during cleanup:'), error);
      }
    }
  };

  // store handler references for proper cleanup
  const sigintHandler = () => {
    process.exit(130);
  };
  const sigtermHandler = () => {
    process.exit(143);
  };

  // register cleanup on process exit (handles all termination cases)
  process.on('exit', cleanup);
  process.on('SIGINT', sigintHandler);
  process.on('SIGTERM', sigtermHandler);

  try {
    const result = await fn();

    // success - remove only our cleanup handlers
    process.removeListener('exit', cleanup);
    process.removeListener('SIGINT', sigintHandler);
    process.removeListener('SIGTERM', sigtermHandler);

    return result;
  } catch (error) {
    // remove only our cleanup handlers
    process.removeListener('exit', cleanup);
    process.removeListener('SIGINT', sigintHandler);
    process.removeListener('SIGTERM', sigtermHandler);

    // cleanup on error - if the directory didn't exist before and exists now
    if (!directoryExistedBefore && existsSync(targetDir)) {
      try {
        console.info(colors.red(`\nCleaning up due to error...`));
        rmSync(targetDir, { recursive: true, force: true });
      } catch (cleanupError) {
        // ignore cleanup errors
      }
    }
    throw error;
  }
}

/**
 * Creates a new plugin with the specified name and configuration.
 */
std::future<void> createPlugin(const std::string& pluginName, const std::string& targetDir, string = 'full' pluginType, auto isNonInteractive = false);

  const processedName = nameResult.processedName!;
  // Add prefix to ensure plugin directory name follows convention
  const pluginDirName = processedName.startsWith('plugin-')
    ? processedName
    : `plugin-${processedName}`;

  // Show warning if the final name differs from what the user entered
  if (pluginDirName !== pluginName) {
    console.warn(
      `\nWarning: changing "${pluginName}" to "${pluginDirName}" to conform to plugin naming conventions\n`
    );
  }

  const pluginTargetDir = join(targetDir, pluginDirName);

  // Validate target directory
  const dirResult = await validateTargetDirectory(pluginTargetDir);
  if (!dirResult.isValid) {
    throw new Error(dirResult.error || 'Invalid target directory');
  }

  if (!isNonInteractive) {
    const displayDir = getDisplayDirectory(targetDir);
    const confirmCreate = await clack.confirm({
      message: `Create plugin "${pluginDirName}" in ${displayDir}?`,
    });

    if (clack.isCancel(confirmCreate) || !confirmCreate) {
      clack.cancel('Plugin creation cancelled.');
      process.exit(0);
    }
  }

  await withCleanupOnInterrupt(pluginTargetDir, pluginDirName, async () => {
    // Map plugin type to template name
    const templateName = pluginType === 'quick' ? 'plugin-quick' : 'plugin';

    await runTasks([
      createTask('Copying plugin template', () =>
        copyTemplateUtil(templateName as 'plugin' | 'plugin-quick', pluginTargetDir)
      ),
      createTask('Installing dependencies', () => installDependenciesWithSpinner(pluginTargetDir)),
    ]);

    console.info(`\n${colors.green('✓')} Plugin "${pluginDirName}" created successfully!`);
    console.info(`\nNext steps:`);
    console.info(`  cd ${pluginDirName}`);
    console.info(`  bun run build   # Build the plugin`);
    console.info(`\n  Common commands:`);
    console.info(`  elizaos dev    # Start development mode with hot reloading`);
    console.info(`  elizaos start  # Start in production mode`);
    console.info(`\n${colors.yellow('⚠️')}  Security reminder:`);
    console.info(`  - Check .gitignore is present before committing`);
    console.info(`  - Never commit .env files or API keys`);
    console.info(`  - Add sensitive files to .gitignore if needed\n`);
  });
}

/**
 * Creates a new agent character file with the specified name.
 */
std::future<void> createAgent(const std::string& agentName, const std::string& targetDir, auto isNonInteractive = false);.json`);

  // Check if agent file already exists
  try {
    await fs.access(agentFilePath);
    throw new Error(`Agent file ${agentFilePath} already exists`);
  } catch (error: any) {
    if (error.code !== 'ENOENT') {
      throw error;
    }
    // File doesn't exist, which is what we want
  }

  if (!isNonInteractive) {
    const displayDir = getDisplayDirectory(targetDir);
    const confirmCreate = await clack.confirm({
      message: `Create agent "${agentName}" in ${displayDir}?`,
    });

    if (clack.isCancel(confirmCreate) || !confirmCreate) {
      clack.cancel('Agent creation cancelled.');
      process.exit(0);
    }
  }

  // Create agent character based on Eliza template
  const agentCharacter = {
    ...getElizaCharacter(),
    name: agentName,
    bio: [
      `${agentName} is a helpful AI assistant created to provide assistance and engage in meaningful conversations.`,
      `${agentName} is knowledgeable, creative, and always eager to help users with their questions and tasks.`,
    ],
  };

  await fs.writeFile(agentFilePath, JSON.stringify(agentCharacter, null, 2));

  // Always show success message and usage instructions - this is critical information
  // that users need regardless of interactive/non-interactive mode
  console.info(`\n${colors.green('✓')} Agent "${agentName}" created successfully!`);
  console.info(`Agent character created successfully at: ${agentFilePath}`);
  console.info(`\nTo use this agent:`);
  console.info(`  1. Start ElizaOS server with this character:`);
  console.info(`     elizaos start --character ${agentFilePath}`);
  console.info(`\n  OR if a server is already running:`);
  console.info(`     elizaos agent start --path ${agentFilePath}`);
}

/**
 * Creates a new TEE project with the specified name and configuration.
 */
std::future<void> createTEEProject(const std::string& projectName, const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive = false);

  if (!isNonInteractive) {
    const displayDir = getDisplayDirectory(targetDir);
    const confirmCreate = await clack.confirm({
      message: `Create TEE project "${projectName}" in ${displayDir}?`,
    });

    if (clack.isCancel(confirmCreate) || !confirmCreate) {
      clack.cancel('TEE project creation cancelled.');
      process.exit(0);
    }
  }

  await withCleanupOnInterrupt(teeTargetDir, projectName, async () => {
    // Create project directory first
    await fs.mkdir(teeTargetDir, { recursive: true });

    // Handle interactive configuration before spinner tasks
    if (!isNonInteractive) {
      await handleInteractiveConfiguration(teeTargetDir, database, aiModel, embeddingModel);
    }

    await runTasks([
      createTask('Copying TEE template', () =>
        copyTemplateUtil('project-tee-starter', teeTargetDir)
      ),
      createTask('Setting up project environment', () =>
        setupProjectEnvironment(teeTargetDir, database, aiModel, embeddingModel, true)
      ),
      createTask('Installing dependencies', () => installDependenciesWithSpinner(teeTargetDir)),
      createTask('Building project', () => buildProjectWithSpinner(teeTargetDir, false)),
    ]);

    console.info(`\n${colors.green('✓')} TEE project "${projectName}" created successfully!`);
    console.info(`\nNext steps:`);
    console.info(`  cd ${projectName}`);
    console.info(`\n  Common commands:`);
    console.info(`  elizaos dev    # Start development mode with hot reloading`);
    console.info(`  elizaos start  # Start in production mode\n`);
  });
}

/**
 * Creates a new regular project with the specified name and configuration.
 */
std::future<void> createProject(const std::string& projectName, const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive = false);

  if (!isNonInteractive) {
    const displayDir = getDisplayDirectory(targetDir);
    const displayProjectName = projectName === '.' ? 'project' : `project "${projectName}"`;
    const confirmCreate = await clack.confirm({
      message: `Create ${displayProjectName} in ${displayDir}?`,
    });

    if (clack.isCancel(confirmCreate) || !confirmCreate) {
      clack.cancel('Project creation cancelled.');
      process.exit(0);
    }
  }

  // only use cleanup wrapper for new directories, not current directory
  const createFn = async () => {
    // Create project directory first if it's not current directory
    if (projectName !== '.') {
      await fs.mkdir(projectTargetDir, { recursive: true });
    }

    // Handle interactive configuration before spinner tasks
    if (!isNonInteractive) {
      await handleInteractiveConfiguration(projectTargetDir, database, aiModel, embeddingModel);
    }

    await runTasks([
      createTask('Copying project template', () =>
        copyTemplateUtil('project-starter', projectTargetDir)
      ),
      createTask('Setting up project environment', () =>
        setupProjectEnvironment(projectTargetDir, database, aiModel, embeddingModel, true)
      ),
      createTask('Installing dependencies', () => installDependenciesWithSpinner(projectTargetDir)),
      createTask('Building project', () => buildProjectWithSpinner(projectTargetDir, false)),
    ]);

    const displayName = projectName === '.' ? 'Project' : `Project "${projectName}"`;
    console.info(`\n${colors.green('✓')} ${displayName} initialized successfully!`);
    console.info(`\nNext steps:`);
    if (projectName !== '.') {
      console.info(`  cd ${projectName}`);
    }
    console.info(`\n  Common commands:`);
    console.info(`  elizaos dev    # Start development mode with hot reloading`);
    console.info(`  elizaos start  # Start in production mode\n`);
  };

  if (projectName === '.') {
    // for current directory, no cleanup needed
    await createFn();
  } else {
    // for new directory, use cleanup wrapper
    await withCleanupOnInterrupt(projectTargetDir, projectName, createFn);
  }
}

} // namespace elizaos
