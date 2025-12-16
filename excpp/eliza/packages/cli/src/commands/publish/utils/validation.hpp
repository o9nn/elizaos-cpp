#include ".types.hpp"
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
 * Validate plugin requirements
 */
std::future<void> validatePluginRequirements(const std::string& cwd, PackageJson packageJson);

  // Check if description is still the default generated one (warning)
  const pluginDirName = path.basename(cwd);
  const expectedDefaultDesc = `ElizaOS plugin for ${pluginDirName.replace('plugin-', '')}`;
  if (
    packageJson.description === expectedDefaultDesc ||
    packageJson.description === '${PLUGINDESCRIPTION}'
  ) {
    warnings.push(
      'Description appears to be the default generated description. Consider writing a custom description.'
    );
  }

  // Check for required images (warnings)
  const imagesDir = path.join(cwd, 'images');
  const logoPath = path.join(imagesDir, 'logo.jpg');
  const bannerPath = path.join(imagesDir, 'banner.jpg');

  try {
    await fs.access(logoPath);
  } catch {
    warnings.push('Missing required logo.jpg in images/ directory (400x400px, max 500KB).');
  }

  try {
    await fs.access(bannerPath);
  } catch {
    warnings.push('Missing required banner.jpg in images/ directory (1280x640px, max 1MB).');
  }

  // Handle hard errors (must be fixed)
  if (errors.length > 0) {
    console.error('Plugin validation failed:');
    errors.forEach((error) => console.error(`  - ${error}`));
    console.error('\nPlease fix these issues and try publishing again.');
    process.exit(1);
  }

  // Handle warnings (can be bypassed)
  if (warnings.length > 0) {
    console.warn('Plugin validation warnings:');
    warnings.forEach((warning) => console.warn(`  - ${warning}`));
    console.warn('Your plugin may get rejected if you submit without addressing these issues.');

    const proceed = await clack.confirm({
      message: 'Do you wish to continue anyway?',
      initialValue: false,
    });

    if (clack.isCancel(proceed)) {
      clack.cancel('Operation cancelled.');
      process.exit(0);
    }

    if (!proceed) {
      console.info('Publishing cancelled. Please address the warnings and try again.');
      process.exit(0);
    }
  }
}

/**
 * Check if user is a maintainer for the package
 */


  return packageJson.maintainers.includes(username);
}

/**
 * Display appropriate registry publication messaging based on options and user status
 */
,
  userIsMaintainer: boolean,
  registryPrUrl?: string
): void {
  // Early returns for clear flow control
  if (opts.skipRegistry) {
    console.info('Registry publication skipped as requested with --skip-registry flag');
    return;
  }

  if (opts.npm) {
    // NPM-only publishing with registry enabled
    console.warn('NPM publishing currently does not update the registry.');
    console.info('To include this package in the registry:');
    console.info(`1. Fork the registry repository at https://github.com/elizaos/registry`);
    console.info('2. Add your package metadata');
    console.info('3. Submit a pull request to the main repository');
    return;
  }

  // GitHub + registry publishing
  if (userIsMaintainer) {
    // For GitHub publishing, PR is already created by publishToGitHub
    if (!registryPrUrl) {
      console.info('Registry publication completed during GitHub publishing process.');
    }
  } else {
    // For non-maintainers, show instructions for registry inclusion
    console.info("Package published, but you're not a maintainer of this package.");
    console.info('To include this package in the registry, please:');
    console.info(`1. Fork the registry repository at https://github.com/elizaos/registry`);
    console.info('2. Add your package metadata');
    console.info('3. Submit a pull request to the main repository');
  }
}

} // namespace elizaos
