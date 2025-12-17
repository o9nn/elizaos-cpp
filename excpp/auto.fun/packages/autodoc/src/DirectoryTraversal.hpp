#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * DirectoryTraversal class for traversing through directories and files.
 * @class DirectoryTraversal
 */
/**
 * DirectoryTraversal class for scanning directories and files based on configuration.
 *
 * @class
 */
class DirectoryTraversal {
public:
    DirectoryTraversal();
    std::string getAbsolutePath(const std::string& filePath);
    std::string getRelativePath(const std::string& filePath);
    std::vector<std::string> traverse();
    bool isExcluded(const std::string& absolutePath);
};


} // namespace elizaos
