#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * File utilities
 * Converted from sweagent/utils/files.py
 */

/**
 * Load a file (JSON or YAML)
 */
std::any loadFile(const std::string& filepath);

} // namespace elizaos
