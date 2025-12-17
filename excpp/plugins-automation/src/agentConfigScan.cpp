#include "agentConfigScan.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> walkDir(const std::string& dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto items = fs.readdir(dir);

    for (const auto& item : items)
        const auto fullPath = path.join(dir, item);
        const auto stat = fs.stat(fullPath);

        if (stat.isDirectory()) {
            // Skip node_modules and other common directories
            if (
            !["node_modules", ".git", "dist", "build", ".next"].includes(item);
            ) {
                walkDir(fullPath);
            }
            } else if (extensions.some((ext) => item.endsWith(ext))) {
                files.push(fullPath);
            }
        }

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto scanner = new AgentConfigScanner();
    scanner.run();

}

} // namespace elizaos
