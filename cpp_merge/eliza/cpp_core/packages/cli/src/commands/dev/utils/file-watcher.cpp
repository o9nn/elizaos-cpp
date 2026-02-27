#include "file-watcher.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<std::string> findTsFiles(const std::string& dir, const std::string& watchDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::vector<std::string> results = [];

    try {
        const auto entries = readdirSync(dir, Config{withFileTypes = true});

        for (const auto& entry : entries)
            const auto fullPath = path.join(dir, entry.name);
            if (
            entry.isDirectory() &&;
            !entry.name.substr(0, ".") &&;
            entry.name != "node_modules" &&;
            entry.name != "dist";
            ) {
                results = results.concat(findTsFiles(fullPath, watchDir));
                } else if (;
                entry.isFile() &&;
                (entry.name.rfind(".ts") ||;
                entry.name.rfind(".js") ||;
                entry.name.rfind(".tsx") ||;
                entry.name.rfind(".jsx"));
                ) {
                    results.push_back(path.relative(watchDir, fullPath));
                }
            }
            } catch (error) {
                // Ignore errors for directories we can't read
            }

            return results;

}

} // namespace elizaos
