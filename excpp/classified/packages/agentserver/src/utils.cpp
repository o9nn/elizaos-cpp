#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string expandTildePath(const std::string& filepath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!filepath || typeof filepath != 'string') {
        return filepath;
    }

    if (filepath.startsWith('~')) {
        return path.join(process.cwd(), filepath.slice(1));
    }

    return filepath;

}

std::string resolvePgliteDir(std::optional<std::string> dir, std::optional<std::string> fallbackDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // If explicit directory provided and not empty, use it
    if (dir && dir.trim()) {
        return expandTildePath(dir);
    }

    // Try environment variable
    const auto envFile = '.env';
    if (fs.existsSync(envFile)) {
        dotenv.config({ path: envFile });
    }

    const auto envDir = process.env.PGLITE_DIR;
    if (envDir) {
        return expandTildePath(envDir);
    }

    // Use fallback directory if provided
    if (fallbackDir) {
        return expandTildePath(fallbackDir);
    }

    // Default fallback
    return expandTildePath('~/eliza/data');

}

} // namespace elizaos
