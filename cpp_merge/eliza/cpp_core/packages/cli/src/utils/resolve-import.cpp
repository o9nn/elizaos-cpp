#include "resolve-import.hpp"
#include <string>
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> resolveImport(const std::string& importPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return createMatchPath(config.absoluteBaseUrl, config.paths)[&](importPath, std::nullopt, () { return true, [; };
    ".ts",
    ]);

}

} // namespace elizaos
