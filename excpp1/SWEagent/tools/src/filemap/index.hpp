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
 * Filemap tool
 * Print Python file contents, skipping lengthy function bodies
 * Converted from tools/filemap/bin/filemap
 */

struct ElidedRange {
    double start;
    double end;
};

void createFilemap(const std::string& filePath);

// CLI if run directly


} // namespace elizaos
