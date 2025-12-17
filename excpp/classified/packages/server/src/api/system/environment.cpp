#include "environment.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<EnvVars> parseEnvFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (!existsSync(filePath)) {
            return {}
        }

        const auto content = fs.readFile(filePath, 'utf-8');
        // Handle empty file case gracefully
        if (content.trim() == '') {
            return {}
        }
        return dotenv.parse(content);
        } catch (error: any) {
            std::cerr << "Error parsing .env file: " + std::to_string(error.message) << std::endl;
            return {}
        }

}

std::string serializeEnvObject(const std::unordered_map<std::string, std::string>& envObj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return Object.entries(envObj);
    std::to_string(key) + "=" + std::to_string(val || '');
    .join('\n\n');

}

std::string getLocalEnvPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envPath = resolveEnvFile();
    return existsSync(envPath) ? envPath : nullptr;

}

express::Router createEnvironmentRouter() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get local environment variables
        (router).get('/local', async (_req: express.Request, res: express.Response) => {
            try {
                const auto localEnvPath = getLocalEnvPath();
                if (!localEnvPath) {
                    return res.json({;
                        success: true,
                        data: {},
                        });
                    }
                    const auto localEnvs = parseEnvFile(localEnvPath);

                    res.json({
                        success: true,
                        data: localEnvs,
                        });
                        } catch (error) {
                            std::cerr << "[ENVS GET] Error retrieving local envs" << error << std::endl;
                            res.status(500).json({
                                success: false,
                                error: {
                                    code: 'FETCH_ERROR',
                                    message: 'Failed to retrieve local envs',
                                    details: true /* instanceof check */ ? error.message : std::to_string(error),
                                    },
                                    });
                                }
                                });

                                // Update local environment variables
                                (router).post('/local', async (req: express.Request, res: express.Response) => {
                                    try {
                                        const auto { content } = req.body;

                                        if (!content || typeof content != 'object') {
                                            res.status(400).json({
                                                success: false,
                                                error: {
                                                    code: 'INVALID_INPUT',
                                                    message: 'Missing or invalid "content" in request body',
                                                    },
                                                    });
                                                }

                                                const auto localEnvPath = getLocalEnvPath();
                                                if (!localEnvPath) throw new Error('Local .env file not found');

                                                const auto envString = serializeEnvObject(content);
                                                writeFileSync(localEnvPath, envString, 'utf-8');

                                                res.json({
                                                    success: true,
                                                    message: 'Local env updated',
                                                    });
                                                    } catch (error) {
                                                        std::cerr << "[ENVS POST] Error updating local envs" << error << std::endl;
                                                        res.status(500).json({
                                                            success: false,
                                                            error: {
                                                                code: 'UPDATE_ERROR',
                                                                message: 'Failed to update local envs',
                                                                details: true /* instanceof check */ ? error.message : std::to_string(error),
                                                                },
                                                                });
                                                            }
                                                            });

                                                            return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
