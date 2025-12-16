#include ".utils/files.hpp"
#include ".utils/log.hpp"
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

/**
 * Remove unfinished trajectories
 */

;
;
;
;

const logger = getLogger('remove_unfinished');

struct TrajectoryFile {
    std::optional<{> info;
    std::optional<string | null> submission;
};


/**
 * Remove unfinished trajectories
 */
std::future<void> removeUnfinished(const std::string& baseDir, boolean = true dryRun);

    if (!dirName.includes('__')) {
      continue;
    }

    const trajs = fs
      .readdirSync(directory)
      .filter((file) => file.endsWith('.traj'))
      .map((file) => path.join(directory, file));

    if (trajs.length === 0) {
      logger.info(`No trajectories found in ${directory}`);
      continue;
    }

    if (trajs.length > 1) {
      logger.warn(`Found multiple trajectories in ${directory}. Skipping.`);
      continue;
    }

    try {
      const traj = loadFile(trajs[0]);

      if (!traj || typeof traj !== 'object') {
        logger.warn(`Invalid trajectory format in ${trajs[0]}. Adding to remove list.`);
        toRemove.push(directory);
        continue;
      }

      const submission = (traj as TrajectoryFile).info?.submission || null;

      if (submission === null) {
        logger.warn(`No submission found in ${directory}. Adding to remove list.`);
        toRemove.push(directory);
        continue;
      }
    } catch (error) {
      logger.warn(`Error loading trajectory ${trajs[0]}: ${error}. Adding to remove list.`);
      toRemove.push(directory);
      continue;
    }
  }

  if (dryRun) {
    logger.info(`Would remove ${toRemove.length} unfinished trajectories.`);
    for (const directory of toRemove) {
      logger.info(directory);
    }
  } else {
    for (const directory of toRemove) {
      logger.info(`Removing ${directory}`);
      fs.rmSync(directory, { recursive: true, force: true });
    }
  }
}

} // namespace elizaos
