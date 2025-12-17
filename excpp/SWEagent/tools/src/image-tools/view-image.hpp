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
 * View image tool
 * Displays an image file as a base64-encoded markdown image
 * Converted from tools/image_tools/bin/view_image
 */

std::string getMimeType(const std::string& filename);

void viewImage(const std::string& imagePath);

// CLI if run directly
// CLI setup
void setupCLI();

// Run CLI if called directly or from bin script


} // namespace elizaos
