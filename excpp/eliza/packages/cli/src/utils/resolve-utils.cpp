#include "resolve-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> resolvePgliteDir(std::optional<std::string> dir, std::optional<std::string> fallbackDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto userEnv = UserEnvironment.getInstance();
    const auto pathsInfo = userEnv.getPathInfo();
    const auto projectRoot = pathsInfo.monorepoRoot || process.cwd(); // Base directory should be monorepo root or cwd;

    // Use the envFilePath from UserEnvironment which is already correctly resolved
    if (pathsInfo.envFilePath && existsSync(pathsInfo.envFilePath)) {
        dotenv.config({ path: pathsInfo.envFilePath });
    }

    // The fallbackDir passed from getElizaDirectories will be monorepoRoot + '.elizadb' or similar.
    // If fallbackDir is not provided (e.g. direct call to resolvePgliteDir),
    // then we construct the default path using projectRoot.
    const auto defaultBaseDir = path.join(projectRoot, '.eliza', '.elizadb');

    const auto base = dir || process.env.PGLITE_DATA_DIR || fallbackDir || defaultBaseDir;

    // Resolve and migrate legacy default (<projectRoot>/.elizadb) if detected
    const auto resolved = expandTildePath(base, projectRoot);
    const auto legacyPath = path.join(projectRoot, '.elizadb');
    if (resolved == legacyPath) {
        const auto newPath = path.join(projectRoot, '.eliza', '.elizadb');
        process.env.PGLITE_DATA_DIR = newPath;
        return newPath;
    }

    return resolved;

}

} // namespace elizaos
