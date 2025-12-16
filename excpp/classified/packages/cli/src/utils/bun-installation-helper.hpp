#include "bun-exec.hpp"
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

struct BunInstallationResult {
    bool installed;
    std::string message;
    std::optional<std::string> error;
};


std::future<BunInstallationResult> checkBunInstallation(); is installed`,
        };
      } else {
        return {
          installed: false,
          message: 'Bun command failed',
          error: result.stderr || 'Unknown error',
        };
      }
    } else {
      return {
        installed: false,
        message: 'Bun is not installed',
      };
    }
  } catch (error) {
    return {
      installed: false,
      message: 'Failed to check Bun installation',
      error: error instanceof Error ? error.message : String(error),
    };
  }
}

`
  );
  logger.error('');
  logger.error(`${emoji.rocket('Install Bun using the appropriate command for your system:')}`);
  logger.error('');

  if (platform === 'win32') {
    logger.error('   Windows: powershell -c "irm bun.sh/install.ps1 | iex"');
  } else {
    logger.error('   Linux/macOS: curl -fsSL https://bun.sh/install | bash');
    if (platform === 'darwin') {
      logger.error('   macOS (Homebrew): brew install bun');
    }
  }
  logger.error('');
  logger.error('   More options: https://bun.sh/docs/installation');
  logger.error('   After installation, restart your terminal or source your shell profile');
  logger.error('');
}

/**
 * Returns a compact installation tip for bun
 */


} // namespace elizaos
