#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "service.ts.hpp"
#include "types.ts.hpp"
#include "utils.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Get the knowledge path from environment or default to ./docs
 */
std::string getKnowledgePath();

/**
 * Load documents from the knowledge path
 */

  // Get all files recursively

      // Skip hidden files and directories

      // Determine content type

      // Skip unsupported file types

      // Read file

      // Check if file is binary using the same logic as the service

      // For text files, read as UTF-8 string directly
      // For binary files, convert to base64

      // Create knowledge options

      // Process the document

/**
 * Recursively get all files in a directory
 */
std::vector<std::string> getAllFiles(const std::string& dirPath, std::vector<std::string> files = {});

/**
 * Get content type based on file extension
 */
std::string getContentType(const std::string& extension);

} // namespace elizaos
