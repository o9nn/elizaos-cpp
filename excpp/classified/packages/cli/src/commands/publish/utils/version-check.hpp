#include "...utils/bun-exec.js.hpp"
#include "..update.hpp"
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

/**
 * Check if the current CLI version is up to date
 */
std::future<std::string> checkCliVersion(); = await bunExecSimple('npm', ['view', '@elizaos/cli', 'time', '--json']);
    const timeData = JSON.parse(stdout);

    // Remove metadata entries like 'created' and 'modified'
    delete timeData.created;
    delete timeData.modified;

    // Find the most recently published version
    let latestVersion = '';
    let latestDate = new Date(0); // Start with epoch time

    for (const [version, dateString] of Object.entries(timeData)) {
      const publishDate = new Date(dateString as string);
      if (publishDate > latestDate) {
        latestDate = publishDate;
        latestVersion = version;
      }
    }

    // Compare versions
    if (latestVersion && latestVersion !== currentVersion) {
      console.warn(`CLI update available: ${currentVersion} → ${latestVersion}`);

      const update = await clack.confirm({
        message: 'Update CLI before publishing?',
        initialValue: false,
      });

      if (clack.isCancel(update)) {
        clack.cancel('Operation cancelled.');
        process.exit(0);
      }

      if (update) {
        console.info('Updating CLI...');
        // Instead of using npx (which gets blocked), directly call the update  catch (updateError) {
          console.error('Failed to update CLI:', updateError);
          // Continue with current version if update fails
        }
      }
    }

    return currentVersion;
  } catch (error) {
    console.warn('Could not check for CLI updates');
    return '0.0.0';
  }
}

} // namespace elizaos
