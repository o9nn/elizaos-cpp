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
 * Search directory tool
 * Search for a term in all files within a directory
 * Converted from tools/search/bin/search_dir
 */

void searchDir(const std::string& searchTerm, std::string dir = "./");

// CLI if run directly
// CLI setup
void setupCLI();

// Run CLI if called directly or from bin script


} // namespace elizaos
