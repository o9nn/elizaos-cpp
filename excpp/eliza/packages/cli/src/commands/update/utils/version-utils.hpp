#include ".types.hpp"
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
;

// Constants
const SPECIAL_VERSION_TAGS = ['latest', 'next', 'canary', 'rc', 'dev', 'nightly', 'alpha'];
const ELIZAOS_ORG = '@elizaos';
const FALLBACK_VERSION = '0.0.0';

/**
 * Get current CLI version using UserEnvironment
 */
std::future<std::string> getVersion(); catch (error) {
    logger.error('Error getting CLI version:', error);
    return FALLBACK_VERSION;
  }
}

/**
 * Check if version string is a workspace reference
 */
const isWorkspaceVersion = (version: string): boolean =>
  version === 'workspace:*' || version === 'workspace' || version.startsWith('workspace:');

/**
 * Check if version is a special tag
 */
const isSpecialVersionTag = (version: string): boolean =>
  SPECIAL_VERSION_TAGS.includes(version);

/**
 * Version comparison helper
 */
;
    }

    if (!semver.valid(cleanCurrent) && !semver.validRange(cleanCurrent)) {
      return { needsUpdate: false, error: 'Invalid semver format' };
    }

    const versionToCompare = semver.validRange(cleanCurrent)
      ? semver.minVersion(cleanCurrent)?.version || cleanCurrent
      : cleanCurrent;

    return { needsUpdate: semver.lt(versionToCompare, targetVersion) };
  } catch (error) {
    return { needsUpdate: false, error: error instanceof Error ? error.message : String(error) };
  }
}

/**
 * Check for major version update
 */


    const currentMajor = semver.major(cleanCurrent);
    const targetMajor = semver.major(targetVersion);
    return targetMajor > currentMajor;
  } catch {
    return false;
  }
}

/**
 * Fetch latest package version from npm registry
 */
std::future<string | null> fetchLatestVersion(const std::string& packageName); = await execa('npm', ['view', packageName, 'version'], {
      env: { NODE_ENV: 'production' },
    });
    const version = stdout.trim();
    logger.debug(`Latest version of ${packageName} from npm: ${version}`);
    return version;
  } catch (error) {
    logger.error(
      `Failed to fetch version for ${packageName}: ${error instanceof Error ? error.message : String(error)}`
    );
    return null;
  }
}

} // namespace elizaos
