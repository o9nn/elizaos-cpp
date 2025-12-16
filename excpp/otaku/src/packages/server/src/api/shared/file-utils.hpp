#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;

/**
 * Safely constructs and validates upload directory paths to prevent path traversal attacks
 */
 ID: contains illegal characters`);
  }

  // Use CLI data directory structure consistently
  const baseUploadDir = type === 'agents' ? getUploadsAgentsDir() : getUploadsChannelsDir();
  const finalDir = path.join(baseUploadDir, id);

  // Ensure the resolved path is still within the expected directory
  const resolvedPath = path.resolve(finalDir);
  const expectedBase = path.resolve(baseUploadDir);

  // Use path.relative for more robust path traversal prevention
  const relativePath = path.relative(expectedBase, resolvedPath);
  if (relativePath.startsWith('..') || path.isAbsolute(relativePath)) {
    throw new Error(`Invalid ${type.slice(0, -1)} upload path: outside allowed directory`);
  }

  return resolvedPath;
}

/**
 * Sanitizes a filename by removing dangerous characters and normalizing it
 */


  // Remove path separators and null bytes
  const sanitized = filename
    .replace(/[/\\:*?"<>|]/g, '_')
    .replace(/\0/g, '')
    .replace(/\.+/g, '.')
    .trim();

  // Ensure filename isn't empty after sanitization
  if (!sanitized || sanitized === '.') {
    return 'unnamed';
  }

  // Limit filename length
  const maxLength = 255;
  if (sanitized.length > maxLength) {
    const ext = path.extname(sanitized);
    const nameWithoutExt = path.basename(sanitized, ext);
    const truncatedName = nameWithoutExt.substring(0, maxLength - ext.length - 1);
    return truncatedName + ext;
  }

  return sanitized;
}

/**
 * Safely cleans up a file by removing it from the filesystem
 */
const cleanupFile = (filePath: string) => {
  try {
    if (filePath && fs.existsSync(filePath)) {
      // Additional path validation
      const normalizedPath = path.normalize(filePath);
      fs.unlinkSync(normalizedPath);
      logger.debug(`[FILE] Successfully cleaned up file: ${normalizedPath}`);
    }
  } catch (error) {
    logger.error(
      `Error cleaning up file ${filePath}:`,
      error instanceof Error ? error.message : String(error)
    );
  }
};

/**
 * Cleans up multiple multer files
 */
const cleanupFiles = (files: Express.Multer.File[]) => {
  if (files) {
    files.forEach((file) => {
      // For multer memory storage, no temp files to clean up
      // This  in memory, no cleanup needed`);
    });
  }
};

/**
 * Cleans up a multer file (no-op for memory storage)
 */
const cleanupUploadedFile = (file: Express.Multer.File) => {
  // For multer memory storage, no temp files to clean up
  logger.debug(`[FILE] Multer file ${file.originalname} in memory, no cleanup needed`);
};

} // namespace elizaos
