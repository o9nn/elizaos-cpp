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
 * Convert a trajectory file to a yaml file for editing of demos.
 * You can then load the yaml file with run_replay to replay the actions in an environment
 * to get environment output.
 */

;
;
;
;

const logger = getLogger('traj2demo');

const DEMO_COMMENT = `# This is a demo file generated from trajectory file:
# {traj_path}
# You can use this demo file to replay the actions in the trajectory with run_replay.
# You can edit the content of the actions in this file to modify the replay behavior.
# NOTICE:
#         Only the actions of the assistant will be replayed.
#         You do not need to modify the observation's contents or any other fields.
#         You can add or remove actions to modify the replay behavior.`;

/**
 * Save demo data as a yaml file with proper header
 */
);

  const header = DEMO_COMMENT.replace('{traj_path}', trajPath);
  fs.writeFileSync(file, `${header}\n${content}`);
}

/**
 * Convert trajectory to action demo
 */


  const history = traj.history || [];

  const copyFields = new Set(['content', 'role', 'tool_calls', 'agent', 'message_type', 'tool_call_ids']);

  const admissibleRoles = includeUser ? new Set(['assistant', 'user', 'tool']) : new Set(['assistant']);

  const filteredHistory = history
    .filter(
      (step: Record<string, unknown>) =>
        admissibleRoles.has(step.role as string) &&
        (!step.agent || step.agent === 'main' || step.agent === 'primary') &&
        !step.is_demo,
    )
    .map((step: Record<string, unknown>) => {
      const filtered: Record<string, unknown> = {};
      for (const key of copyFields) {
        if (key in step) {
          filtered[key] = step[key];
        }
      }
      return filtered;
    });

  const outputData = {
    history: filteredHistory,
    replay_config: replayConfig,
  };

  saveDemo(outputData, outputFile, trajPath);
  logger.info(`Saved demo to ${outputFile}`);
}

/**
 * Main .demo.yaml`);

  if (fs.existsSync(outputFile) && !overwrite) {
    throw new Error(`Output file already exists: ${outputFile}. Use --overwrite to overwrite.`);
  }

  const outputFileDir = path.dirname(outputFile);
  if (!fs.existsSync(outputFileDir)) {
    fs.mkdirSync(outputFileDir, { recursive: true });
  }

  convertTrajToActionDemo(trajPath, outputFile, includeUser);
}

} // namespace elizaos
