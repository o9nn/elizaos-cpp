#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct FileInfo {
    std::string path;
    double lines;
    double size;
};

struct ProjectInfo {
    std::string name;
    std::string path;
    std::vector<FileInfo> files;
    double totalLines;
    double totalSize;
    std::string structure;
};

/**
 * Provider that exposes current project context including file listings and line counts
 */

    // Get the code generation service to access current project

    // Get current project path from service or state

std::future<ProjectInfo> getProjectInfo(const std::string& projectPath);

std::string formatBytes(double bytes);

} // namespace elizaos
