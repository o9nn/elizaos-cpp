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
 * Build and publish package to npm
 */
std::future<void> publishToNpm(const std::string& cwd, PackageJson packageJson, const std::string& npmUsername);`);

  // Update npmPackage field if it's a placeholder or not set
  if (!packageJson.npmPackage || packageJson.npmPackage === '${NPM_PACKAGE}') {
    packageJson.npmPackage = packageJson.name;
    console.info(`Set npmPackage to: ${packageJson.npmPackage}`);

    // Save updated package.json
    const packageJsonPath = path.join(cwd, 'package.json');
    await fs.writeFile(packageJsonPath, JSON.stringify(packageJson, null, 2), 'utf-8');
  }

  // Build the package
  console.info('Building package...');
  await execa('npm', ['run', 'build'], { cwd, stdio: 'inherit' });

  // Publish to npm with --ignore-scripts to prevent recursion
  console.info('Publishing to npm...');
  await execa('npm', ['publish', '--ignore-scripts'], { cwd, stdio: 'inherit' });

  console.log(`[√] Successfully published ${packageJson.name}@${packageJson.version} to npm`);
}

} // namespace elizaos
