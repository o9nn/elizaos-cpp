#include "elizaos/core.hpp"
#include "spinner-utils.hpp"
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

// Define __dirname for ES modules
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

/**
 * Copy a directory recursively
 */
/**
 * Asynchronously copies the contents of a directory from a source path to a destination path, excluding specified files and directories.
 * If the destination directory does not exist, it will be created.
 *
 * @param {string} src - The path to the source directory.
 * @param {string} dest - The path to the destination directory.
 * @param {string[]} [exclude=[]] - An array of file and directory names to exclude from the copy operation.
 * @returns {Promise<void>} A Promise that resolves when the copy operation is complete.
 */
async );

  // Read source directory
  const entries = await fs.readdir(resolvedSrc, { withFileTypes: true });

  // Separate files and directories for different processing strategies
  const files: typeof entries = [];
  const directories: typeof entries = [];

  for (const entry of entries) {
    // Skip excluded directories/files
    if (exclude.includes(entry.name)) {
      continue;
    }

    // Skip node_modules, .git directories and other build artifacts
    if (
      entry.name === 'node_modules' ||
      entry.name === '.git' ||
      entry.name === 'cache' ||
      entry.name === 'data' ||
      entry.name === 'generatedImages' ||
      entry.name === '.turbo'
    ) {
      continue;
    }

    if (entry.isDirectory()) {
      directories.push(entry);
    } else {
      files.push(entry);
    }
  }

  // Process files in parallel (up to 10 concurrent operations)
  const MAX_CONCURRENT_FILES = 10;
  const filePromises: Promise<void>[] = [];

  for (let i = 0; i < files.length; i += MAX_CONCURRENT_FILES) {
    const batch = files.slice(i, i + MAX_CONCURRENT_FILES);
    const batchPromises = batch.map(async (entry) => {
      const srcPath = path.join(resolvedSrc, entry.name);
      const destPath = path.join(resolvedDest, entry.name);
      await fs.copyFile(srcPath, destPath);
    });
    filePromises.push(...batchPromises);
  }

  // Wait for all file copies to complete
  await Promise.all(filePromises);

  // Process directories sequentially to avoid too much recursion depth
  // but still get benefits from parallel file copying within each directory
  for (const entry of directories) {
    const srcPath = path.join(resolvedSrc, entry.name);
    const destPath = path.join(resolvedDest, entry.name);
    await copyDir(srcPath, destPath, exclude);
  }
}

/**
 * Map template types to actual package names
 */

}

/**
 * Copy a project or plugin template to target directory
 */
async 
  }

  if (!templateDir) {
    throw new Error(
      `Template '${packageName}' not found. Searched in:\n${possibleTemplatePaths.join('\n')}`
    );
  }

  logger.debug(`Copying ${templateType} template from ${templateDir} to ${targetDir}`);

  // Copy template files as-is
  await copyDir(templateDir, targetDir);

  // For plugin templates, replace hardcoded "plugin-starter" strings in source files
  if (templateType === 'plugin' || templateType === 'plugin-quick') {
    const pluginNameFromPath = path.basename(targetDir);
    await replacePluginNameInFiles(targetDir, pluginNameFromPath);
  }

  // Update package.json with dependency versions only (leave placeholders intact)
  const packageJsonPath = path.join(targetDir, 'package.json');

  try {
    // Get the CLI package version for dependency updates
    const cliPackageJsonPath = path.resolve(
      path.dirname(require.resolve('@elizaos/cli/package.json')),
      'package.json'
    );

    const cliPackageJson = JSON.parse(await fs.readFile(cliPackageJsonPath, 'utf8'));
    const cliPackageVersion = cliPackageJson.version;

    const packageJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));

    // Remove private field from template package.json since templates should be usable by users
    if (packageJson.private) {
      delete packageJson.private;
      logger.debug('Removed private field from template package.json');
    }

    // Only update dependency versions - leave everything else unchanged
    if (packageJson.dependencies) {
      for (const depName of Object.keys(packageJson.dependencies)) {
        if (depName.startsWith('@elizaos/')) {
          if (!isQuietMode()) {
            logger.info(`Setting ${depName} to use version ${cliPackageVersion}`);
          }
          packageJson.dependencies[depName] = 'latest';
        }
      }
    }

    if (packageJson.devDependencies) {
      for (const depName of Object.keys(packageJson.devDependencies)) {
        if (depName.startsWith('@elizaos/')) {
          if (!isQuietMode()) {
            logger.info(`Setting dev dependency ${depName} to use version ${cliPackageVersion}`);
          }
          packageJson.devDependencies[depName] = 'latest';
        }
      }
    }

    // Update the package name to use the actual name provided by the user
    const projectNameFromPath = path.basename(targetDir);

    if (packageJson.name !== projectNameFromPath) {
      packageJson.name = projectNameFromPath;
      if (!isQuietMode()) {
        logger.info(`Setting package name to ${projectNameFromPath}`);
      }
    }

    // Write the updated package.json (dependency versions and plugin name changed)
    await fs.writeFile(packageJsonPath, JSON.stringify(packageJson, null, 2));
    logger.debug('Updated package.json with latest dependency versions');
  } catch (error) {
    logger.error(`Error updating package.json: ${error}`);
  }

  logger.debug(`${templateType} template copied successfully`);
}

/**
 * Replace hardcoded "plugin-starter" or "plugin-quick-starter" strings in source files with the actual plugin name
 */
std::future<void> replacePluginNameInFiles(const std::string& targetDir, const std::string& pluginName);`);
      }
    } catch (error) {
      logger.warn(
        `Could not update ${filePath}: ${error instanceof Error ? error.message : String(error)}`
      );
    }
  });

  await Promise.all(promises);
}

} // namespace elizaos
