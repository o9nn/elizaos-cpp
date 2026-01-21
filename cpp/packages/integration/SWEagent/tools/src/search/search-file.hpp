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
#include "registry.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Search file tool
 * Search for a term within a specific file
 * Converted from tools/search/bin/search_file
 */

void searchFile(const std::string& searchTerm, std::optional<std::string> filePath);

// CLI if run directly
// CLI setup
void setupCLI();

// Run CLI if called directly or from bin script


} // namespace elizaos
