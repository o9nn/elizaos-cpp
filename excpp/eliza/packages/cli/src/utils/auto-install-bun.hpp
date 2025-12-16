#include "elizaos/core.hpp"
#include "emoji-handler.hpp"
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

/**
 * Automatically install Bun if it's not available
 * @returns {Promise<boolean>} - Whether Bun is now available
 */
std::future<bool> autoInstallBun(););
    return true;
  } catch {
    // Bun is not available, attempt auto-installation
  }

  logger.info(`${emoji.rocket('Bun is required for ElizaOS CLI. Installing automatically...')}`);

  try {
    if (platform === 'win32') {
      // Windows installation
      logger.info('Installing Bun on Windows...');
      await execa('powershell', ['-c', 'irm bun.sh/install.ps1 | iex'], {
        stdio: 'inherit',
      });
    } else {
      // Linux/macOS installation
      logger.info('Installing Bun on Linux/macOS...');

      // Use shell to execute the curl | bash command directly
      await execa('sh', ['-c', 'curl -fsSL https://bun.sh/install | bash'], {
        stdio: 'inherit',
      });
    }

    // Add bun to PATH for current session
    const bunPath =
      platform === 'win32'
        ? `${process.env.USERPROFILE}\\.bun\\bin`
        : `${process.env.HOME}/.bun/bin`;

    if (bunPath && !process.env.PATH?.includes(bunPath)) {
      process.env.PATH = `${bunPath}${platform === 'win32' ? ';' : ':'}${process.env.PATH}`;
    }

    // Verify installation worked
    await execa('bun', ['--version'], { stdio: 'ignore' });
    logger.success(`${emoji.success('Bun installed successfully!')}`);

    return true;
  } catch (error) {
    logger.error(`${emoji.error('Failed to automatically install Bun:')}`);
    logger.error(`Error: ${error instanceof Error ? error.message : String(error)}`);

    // Fall back to manual installation instructions
    logger.info(`\n${emoji.info('Please install Bun manually:')}`);
    if (platform === 'win32') {
      logger.info('   Windows: powershell -c "irm bun.sh/install.ps1 | iex"');
    } else {
      logger.info('   Linux/macOS: curl -fsSL https://bun.sh/install | bash');
      if (platform === 'darwin') {
        logger.info('   macOS (Homebrew): brew install bun');
      }
    }
    logger.info(`\n${emoji.link('More options: https://bun.sh/docs/installation')}`);
    logger.info(
      `\n${emoji.tip('After installation, restart your terminal or source your shell profile')}`
    );

    return false;
  }
}

/**
 * Check if auto-installation should be attempted
 * @returns {boolean} - Whether to attempt auto-installation
 */


  // Don't auto-install if explicitly disabled
  if (process.env.ELIZA_NO_AUTO_INSTALL === 'true') {
    return false;
  }

  // Check for --no-auto-install flag (backup check)
  if (process.argv.includes('--no-auto-install')) {
    return false;
  }

  return true;
}

} // namespace elizaos
