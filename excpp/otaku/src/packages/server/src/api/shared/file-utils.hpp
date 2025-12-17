#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Safely constructs and validates upload directory paths to prevent path traversal attacks
 */
std::string createSecureUploadDir(const std::string& id, const std::string& type);

/**
 * Sanitizes a filename by removing dangerous characters and normalizing it
 */
std::string sanitizeFilename(const std::string& filename);

/**
 * Safely cleans up a file by removing it from the filesystem
 */
      // Additional path validation

/**
 * Cleans up multiple multer files
 */
      // For multer memory storage, no temp files to clean up
      // This function is kept for compatibility

/**
 * Cleans up a multer file (no-op for memory storage)
 */
  // For multer memory storage, no temp files to clean up

} // namespace elizaos
