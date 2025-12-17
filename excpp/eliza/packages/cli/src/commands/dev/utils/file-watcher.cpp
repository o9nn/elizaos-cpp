#include "file-watcher.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<std::string> findTsFiles(const std::string& dir, const std::string& watchDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::vector<std::string> results = [];

    try {
        const auto entries = readdirSync(dir, { withFileTypes: true });

        for (const auto& entry : entries)
            const auto fullPath = path.join(dir, entry.name);
            if (
            entry.isDirectory() &&;
            !entry.name.startsWith('.') &&;
            entry.name != 'node_modules' &&;
            entry.name != 'dist';
            ) {
                results = results.concat(findTsFiles(fullPath, watchDir));
                } else if (;
                entry.isFile() &&;
                (entry.name.endsWith('.ts') ||;
                entry.name.endsWith('.js') ||;
                entry.name.endsWith('.tsx') ||;
                entry.name.endsWith('.jsx'));
                ) {
                    results.push(path.relative(watchDir, fullPath));
                }
            }
            } catch (error) {
                // Ignore errors for directories we can't read
            }

            return results;

}

} // namespace elizaos
