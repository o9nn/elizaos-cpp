#include "fsHelpers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> ensureDir(const std::string& dirPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    fs.mkdir(dirPath, { recursive: true });

}

std::future<void> writeToFile(const std::string& filePath, const std::string& data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    ensureDir(path.dirname(filePath));
    fs.writeFile(filePath, data, "utf-8");

}

void getRepoFilePath(const std::string& outputDir, const std::string& repoId, const std::string& dataType, const std::string& intervalType, const std::string& fileName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto safeRepoId = repoId.replace("/", "_");
    return path.join(outputDir, safeRepoId, dataType, intervalType, fileName);

}

} // namespace elizaos
