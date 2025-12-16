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
 * Inspector server for viewing agent trajectories
 * Converted from sweagent/inspector/server.py
 */

;
;
;
;
;

const logger = getLogger('inspector-server');

struct TrajectoryStep {
    std::string thought;
    std::string action;
    std::string response;
    std::string observation;
    std::optional<std::vector<{ role: string; content: string }>> messages;
};


struct ModelStats {
    std::optional<double> instanceCost;
    std::optional<double> totalCost;
    std::optional<double> instanceInputTokens;
    std::optional<double> instanceOutputTokens;
    std::optional<double> instanceCallCount;
};


struct TrajectoryContent {
    std::vector<{ role: string; content: string }> history;
    std::vector<TrajectoryStep> trajectory;
    { info;
    std::optional<std::string> exitStatus;
    std::optional<std::string> submission;
    std::optional<ModelStats> modelStats;
    std::optional<std::string> environment;
};


/**
 * Add problem statement to the trajectory
 * The problem statement is the first 'user' message in the history
 */

  }

  if (problemStatement) {
    content.trajectory.unshift({
      thought: '',
      action: '',
      response: '',
      observation: problemStatement,
      messages: [{ role: 'system', content: 'Problem Statement placeholder' }],
    });
  }

  return content;
}

/**
 * Append exit status to the trajectory
 */


  if (exitStatus.startsWith('submitted')) {
    if (content.info.submission) {
      content.trajectory.push({
        thought: 'Submitting solution',
        action: 'Model Submission',
        response: 'Submitting solution',
        observation: content.info.submission,
        messages: [],
      });
    }
  } else if (exitStatus === 'exit_cost' || exitStatus === 'exit_context') {
    const observation = exitStatus === 'exit_cost' ? 'Exit due to cost limit' : 'Exit due to context limit';

    content.trajectory.push({
      thought: 'Exit',
      action: 'Exit',
      response: '',
      observation,
      messages: [],
    });
  } else if (exitStatus === 'exit_error') {
    content.trajectory.push({
      thought: 'Exit',
      action: 'Exit due to error',
      response: '',
      observation: 'Exit due to error',
      messages: [],
    });
  } else if (exitStatus === 'exit_format') {
    content.trajectory.push({
      thought: 'Exit',
      action: 'Exit due to format error',
      response: '',
      observation: 'Exit due to format error',
      messages: [],
    });
  }

  return content;
}

/**
 * Add model statistics to the trajectory
 */

Total Cost: $${modelStats.totalCost?.toFixed(4) || 0}
Input Tokens: ${modelStats.instanceInputTokens || 0}
Output Tokens: ${modelStats.instanceOutputTokens || 0}
API Calls: ${modelStats.instanceCallCount || 0}`;

    content.trajectory.push({
      thought: '',
      action: 'Model Stats',
      response: '',
      observation: statsText,
      messages: [],
    });
  }

  return content;
}

/**
 * Parse and augment trajectory file
 */
 catch {
      content = yaml.load(fileContent) as TrajectoryContent;
    }

    // Augment the trajectory
    content = addProblemStatement(content);
    content = appendExit(content);
    content = addModelStats(content);

    // Add environment information if available
    if (content.environment) {
      content.trajectory.push({
        thought: '',
        action: 'Environment',
        response: '',
        observation: content.environment,
        messages: [],
      });
    }

    return content;
  } catch (error) {
    logger.error(`Error reading trajectory file ${filePath}:`, error);
    return null;
  }
}

/**
 * Create and start the inspector server
 */
): void {
  const app = express();
  const port = options.port || 8000;
  const trajectoryDir = options.trajectoryDir || './trajectories';
  const staticDir = options.staticDir || path.join(__dirname, '../../sweagent/inspector');

  // Ensure trajectory directory exists
  if (!fs.existsSync(trajectoryDir)) {
    fs.mkdirSync(trajectoryDir, { recursive: true });
  }

  // Serve static files (HTML, CSS, JS)
  app.use(express.static(staticDir));

  // API endpoint to list trajectory files
  app.get('/api/trajectories', (_req, res) => {
    try {
      const files = fs
        .readdirSync(trajectoryDir)
        .filter((file) => file.endsWith('.traj') || file.endsWith('.yaml') || file.endsWith('.json'))
        .map((file) => ({
          name: file,
          path: path.join(trajectoryDir, file),
          modified: fs.statSync(path.join(trajectoryDir, file)).mtime,
        }))
        .sort((a, b) => b.modified.getTime() - a.modified.getTime());

      res.json(files);
    } catch (error) {
      logger.error('Error listing trajectories:', error);
      res.status(500).json({ error: 'Failed to list trajectories' });
    }
  });

  // API endpoint to get a specific trajectory
  app.get('/api/trajectory/:filename', (req, res) => {
    const filename = req.params.filename;
    const filePath = path.join(trajectoryDir, filename);

    if (!fs.existsSync(filePath)) {
      return res.status(404).json({ error: 'Trajectory not found' });
    }

    const trajectory = getTrajectory(filePath);

    if (!trajectory) {
      return res.status(500).json({ error: 'Failed to parse trajectory' });
    }

    return res.json(trajectory);
  });

  // API endpoint to get trajectory statistics
  app.get('/api/stats', (_req, res) => {
    try {
      const files = fs
        .readdirSync(trajectoryDir)
        .filter((file) => file.endsWith('.traj') || file.endsWith('.yaml') || file.endsWith('.json'));

      const stats = {
        totalTrajectories: files.length,
        recentTrajectories: files.slice(0, 10).map((file) => {
          const trajectory = getTrajectory(path.join(trajectoryDir, file));
          return {
            file,
            steps: trajectory?.trajectory.length || 0,
            exitStatus: trajectory?.info.exitStatus || 'unknown',
            cost: trajectory?.info.modelStats?.instanceCost || 0,
          };
        }),
      };

      res.json(stats);
    } catch (error) {
      logger.error('Error computing statistics:', error);
      res.status(500).json({ error: 'Failed to compute statistics' });
    }
  });

  // Start the server
  app.listen(port, () => {
    logger.info(`Inspector server running at http://localhost:${port}`);
    logger.info(`Serving trajectories from: ${trajectoryDir}`);
    logger.info(`Static files from: ${staticDir}`);
  });
}

/**
 * CLI entry point
 */
if (require.main === module) {
  const args = process.argv.slice(2);
  const port = parseInt(args.find((arg) => arg.startsWith('--port='))?.split('=')[1] || '8000');
  const trajectoryDir = args.find((arg) => arg.startsWith('--dir='))?.split('=')[1] || './trajectories';

  startInspectorServer({ port, trajectoryDir });
}

} // namespace elizaos
