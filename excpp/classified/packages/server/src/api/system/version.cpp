#include "version.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

VersionInfo getVersionInfo() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto timestamp = new Date().toISOString();

    try {
        return {
            version: packageJson.version,
            source: 'server',
            timestamp,
            environment: process.env.NODE_ENV || 'development',
            uptime: process.uptime(),
            };
            } catch (error) {
                std::cerr << 'Error getting version info:' << error << std::endl;

                return {
                    version: 'unknown',
                    source: 'server',
                    timestamp,
                    environment: process.env.NODE_ENV || 'development',
                    uptime: process.uptime(),
                    error: 'Failed to retrieve version information',
                    };
                }

}

express::Router createVersionRouter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // GET /api/system/version - Returns version information
    router.get('/', (_, res) => {
        const auto versionInfo = getVersionInfo();
        const auto statusCode = versionInfo.error ? 500 : 200;

        res.status(statusCode).json(versionInfo);
        });

        return router;

}

} // namespace elizaos
