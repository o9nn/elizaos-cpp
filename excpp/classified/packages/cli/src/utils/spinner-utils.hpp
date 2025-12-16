#include "bun-exec.js.hpp"
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

/**
 * Check if quiet mode is enabled
 */


struct SpinnerCommandOptions {
    std::optional<std::string> cwd;
    std::string spinnerText;
    std::string successText;
    std::optional<std::string> errorText;
    std::optional<bool> showOutputOnError;
};


struct CommandResult {
    bool success;
    std::optional<std::string> stdout;
    std::optional<std::string> stderr;
    std::optional<Error> error;
};


/**
 * Core ;
    } else {
      const errorMessage = options.errorText || `Command failed with exit code ${result.exitCode}`;
      spinner.stop(colors.red(errorMessage));

      // Show captured output on error for debugging
      if (options.showOutputOnError !== false) {
        if (result.stdout) {
          console.error(colors.dim('stdout:'), result.stdout);
        }
        if (result.stderr) {
          console.error(colors.dim('stderr:'), result.stderr);
        }
      }

      return {
        success: false,
        stdout: result.stdout,
        stderr: result.stderr,
        error: new Error(`${errorMessage}: ${result.stderr || result.stdout}`),
      };
    }
  } catch (error) {
    const errorMessage = options.errorText || 'Command execution failed';
    spinner.stop(colors.red(errorMessage));

    return {
      success: false,
      error: error as Error,
    };
  }
}

/**
 * Run a bun command with spinner
 */
std::future<CommandResult> runBunWithSpinner(const std::vector<std::string>& args, const std::string& cwd, const std::optional<SpinnerCommandOptions>& options););
}

/**
 * Install dependencies with spinner
 */
std::future<void> installDependenciesWithSpinner(const std::string& targetDir);

  const result = await runBunWithSpinner(['install'], targetDir, {
    spinnerText: 'Installing dependencies...',
    successText: colors.green('✓ Dependencies installed successfully'),
    errorText: 'Failed to install dependencies',
  });

  if (!result.success) {
    throw result.error || new Error('Dependency installation failed');
  }
}

/**
 * Build project with spinner
 */
std::future<void> buildProjectWithSpinner(const std::string& targetDir, auto isPlugin = false);

  const projectType = isPlugin ? 'plugin' : 'project';

  const result = await runBunWithSpinner(['run', 'build'], targetDir, {
    spinnerText: `Building ${projectType}...`,
    successText: colors.green(
      `✓ ${projectType.charAt(0).toUpperCase() + projectType.slice(1)} built successfully`
    ),
    errorText: `Failed to build ${projectType}`,
  });

  if (!result.success) {
    throw result.error || new Error(`${projectType} build failed`);
  }
}

/**
 * Install plugin with spinner (non-critical, warns on failure)
 */
std::future<void> installPluginWithSpinner(const std::string& pluginName, const std::string& targetDir, auto purpose = '');` : '';
  const packageName = `@elizaos/plugin-${pluginName}`;

  // Skip in test environments
  if (process.env.CI === 'true' || process.env.ELIZA_TEST_MODE === 'true') {
    return;
  }

  const spinner = clack.spinner();
  spinner.start(`Installing ${packageName}${purposeText}...`);

  try {
    const result = await bunExec('bun', ['add', packageName], {
      cwd: targetDir,
    });

    if (result.success) {
      spinner.stop(colors.green(`✓ ${packageName} installed successfully`));
    } else {
      // Log warning but don't throw - plugin installation is non-critical
      spinner.stop(colors.yellow(`⚠ Failed to install ${packageName} (optional)`));

      // Log to debug for troubleshooting
      logger.debug(`Plugin installation failed: ${packageName}`, {
        exitCode: result.exitCode,
        stderr: result.stderr,
        stdout: result.stdout,
      });
    }
  } catch (error) {
    // Log warning but don't throw - plugin installation is non-critical
    spinner.stop(colors.yellow(`⚠ Failed to install ${packageName} (optional)`));

    logger.debug(`Plugin installation error: ${packageName}`, error);
  }
}

/**
 * Create a task for use with clack.tasks()
 */
 completed`;
    },
  };
}

/**
 * Run multiple tasks with clack.tasks()
 */
async >
): Promise<void> {
  await clack.tasks(tasks);
}

} // namespace elizaos
