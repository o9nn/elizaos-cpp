#pragma once
#include <algorithm>
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
 * Find file tool
 * Find all files with a given name or pattern in a directory
 * Converted from tools/search/bin/find_file
 */

void findFile(const std::string& fileName, std::string dir = "./");

// CLI if run directly
// CLI setup
void setupCLI();

// Run CLI if called directly or from bin script


} // namespace elizaos
