#include "server.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

TrajectoryContent addProblemStatement(TrajectoryContent content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto problemStatement = '';

    for (const auto& item : content.history || [])
        if (item.role == 'user') {
            problemStatement = item.content;
            break;
        }
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

TrajectoryContent appendExit(TrajectoryContent content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto exitStatus = content.info.exitStatus;

    if (!exitStatus) {
        return content;
    }

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
            } else if (exitStatus == 'exit_cost' || exitStatus == 'exit_context') {
                const auto observation = exitStatus == 'exit_cost' ? 'Exit due to cost limit' : 'Exit due to context limit';

                content.trajectory.push({
                    thought: 'Exit',
                    action: 'Exit',
                    response: '',
                    observation,
                    messages: [],
                    });
                    } else if (exitStatus == 'exit_error') {
                        content.trajectory.push({
                            thought: 'Exit',
                            action: 'Exit due to error',
                            response: '',
                            observation: 'Exit due to error',
                            messages: [],
                            });
                            } else if (exitStatus == 'exit_format') {
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

TrajectoryContent addModelStats(TrajectoryContent content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto modelStats = content.info.modelStats;

    if (modelStats) {
        const auto statsText = `Model API Usage:;
    Instance Cost: $${modelStats.instanceCost.toFixed(4) || 0}
    Total Cost: $${modelStats.totalCost.toFixed(4) || 0}
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

std::optional<TrajectoryContent> getTrajectory(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto fileContent = fs.readFileSync(filePath, 'utf-8');
        auto content: TrajectoryContent;

        // Try parsing as JSON first, then YAML
        try {
            content = JSON.parse(fileContent);
            } catch {
                content = yaml.load(fileContent);
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
                    std::cerr << "Error reading trajectory file " + std::to_string(filePath) + ":" << error << std::endl;
                    return nullptr;
                }

}

void startInspectorServer(std::optional<std::any> options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto app = express();
    const auto port = options.port || 8000;
    const auto trajectoryDir = options.trajectoryDir || './trajectories';
    const auto staticDir = options.staticDir || path.join(__dirname, '../../sweagent/inspector');

    // Ensure trajectory directory exists
    if (!fs.existsSync(trajectoryDir)) {
        fs.mkdirSync(trajectoryDir, { recursive: true });
    }

    // Serve static files (HTML, CSS, JS)
    app.use(express.static(staticDir));

    // API endpoint to list trajectory files
    app.get('/api/trajectories', (_req, res) => {
        try {
            const auto files = fs;
            .readdirSync(trajectoryDir);
            .filter((file) => file.endsWith('.traj') || file.endsWith('.yaml') || file.endsWith('.json'));
            .map((file) => ({
                name: file,
                path: path.join(trajectoryDir, file),
                modified: fs.statSync(path.join(trajectoryDir, file)).mtime,
                }));
                .sort((a, b) => b.modified.getTime() - a.modified.getTime());

                res.json(files);
                } catch (error) {
                    std::cerr << 'Error listing trajectories:' << error << std::endl;
                    res.status(500).json({ error: 'Failed to list trajectories' });
                }
                });

                // API endpoint to get a specific trajectory
                app.get('/api/trajectory/:filename', (req, res) => {
                    const auto filename = req.params.filename;
                    const auto filePath = path.join(trajectoryDir, filename);

                    if (!fs.existsSync(filePath)) {
                        return res.status(404).json({ error: 'Trajectory not found' });
                    }

                    const auto trajectory = getTrajectory(filePath);

                    if (!trajectory) {
                        return res.status(500).json({ error: 'Failed to parse trajectory' });
                    }

                    return res.json(trajectory);
                    });

                    // API endpoint to get trajectory statistics
                    app.get('/api/stats', (_req, res) => {
                        try {
                            const auto files = fs;
                            .readdirSync(trajectoryDir);
                            .filter((file) => file.endsWith('.traj') || file.endsWith('.yaml') || file.endsWith('.json'));

                            const auto stats = {;
                                totalTrajectories: files.length,
                                recentTrajectories: files.slice(0, 10).map((file) => {
                                    const auto trajectory = getTrajectory(path.join(trajectoryDir, file));
                                    return {
                                        file,
                                        steps: trajectory.trajectory.length || 0,
                                        exitStatus: trajectory.info.exitStatus || 'unknown',
                                        cost: trajectory.info.modelStats.instanceCost || 0,
                                        };
                                        }),
                                        };

                                        res.json(stats);
                                        } catch (error) {
                                            std::cerr << 'Error computing statistics:' << error << std::endl;
                                            res.status(500).json({ error: 'Failed to compute statistics' });
                                        }
                                        });

                                        // Start the server
                                        app.listen(port, () => {
                                            std::cout << "Inspector server running at http://localhost:" + std::to_string(port) << std::endl;
                                            std::cout << "Serving trajectories from: " + std::to_string(trajectoryDir) << std::endl;
                                            std::cout << "Static files from: " + std::to_string(staticDir) << std::endl;
                                            });

}

} // namespace elizaos
