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
 * Calculate statistics from .traj files
 */

;
;
;

const logger = getLogger('quick-stats', '📊');

/**
 * Calculate statistics from .traj files
 */
std::future<std::string> quickStats(string = '.' directory);`);
    return 'No .traj files found.';
  }

  // Extract api_calls from each file
  const apiCalls: number[] = [];
  const filesByExitStatus = new Map<string, string[]>();

  for (const filePath of trajFiles) {
    try {
      const data = JSON.parse(fs.readFileSync(filePath, 'utf-8'));

      // Extract the api_calls value
      if (data.info?.model_stats?.api_calls !== undefined) {
        apiCalls.push(data.info.model_stats.api_calls);
      }

      if (data.info?.exit_status) {
        const status = data.info.exit_status;
        if (!filesByExitStatus.has(status)) {
          filesByExitStatus.set(status, []);
        }
        filesByExitStatus.get(status)!.push(filePath);
      }
    } catch (error) {
      logger.error(`Error processing ${filePath}: ${error}`);
    }
  }

  // Sort exit statuses by count (highest to lowest)
  const sortedStatuses = Array.from(filesByExitStatus.entries()).sort((a, b) => b[1].length - a[1].length);

  // If we have no exit statuses and no api_calls, return error
  if (sortedStatuses.length === 0 && apiCalls.length === 0) {
    logger.warn('No valid api_calls data found in the .traj files');
    return 'No valid api_calls data found in the .traj files.';
  }

  // Log statistics
  logger.info('Exit statuses:');
  for (const [status, files] of sortedStatuses) {
    logger.info(`${status}: ${files.length}`);
  }

  if (apiCalls.length > 0) {
    const averageApiCalls = apiCalls.reduce((a, b) => a + b, 0) / apiCalls.length;
    logger.info(`Avg api calls: ${averageApiCalls}`);
  }

  // Format result
  const result: string[] = [];
  for (const [status, files] of sortedStatuses) {
    result.push(`\n## \`${status}\` - ${files.length} trajectories`);
    // Extract unique subdirectories instead of full paths
    const subdirs = new Set(files.map((file) => path.dirname(file)));
    result.push(Array.from(subdirs).join(' '));
  }

  return result.join('\n');
}

/**
 * Recursively find all .traj files in a directory
 */
 else if (file.endsWith('.traj')) {
        results.push(filePath);
      }
    }
  }

  walk(directory);
  return results;
}

} // namespace elizaos
