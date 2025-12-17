#include "file-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string createSecureUploadDir(const std::string& id, const std::string& type) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Additional validation beyond UUID to ensure no path traversal
        if (id.includes('..') || id.includes('/') || id.includes('\\') || id.includes('\0')) {
            throw std::runtime_error(`Invalid ${type.slice(0, -1)} ID: contains illegal characters`);
        }

        // Use CLI data directory structure consistently
        const auto baseUploadDir = type == 'agents' ? getUploadsAgentsDir() : getUploadsChannelsDir();
        const auto finalDir = path.join(baseUploadDir, id);

        // Ensure the resolved path is still within the expected directory
        const auto resolvedPath = path.resolve(finalDir);
        const auto expectedBase = path.resolve(baseUploadDir);

        // Use path.relative for more robust path traversal prevention
        const auto relativePath = path.relative(expectedBase, resolvedPath);
        if (relativePath.startsWith('..') || path.isAbsolute(relativePath)) {
            throw std::runtime_error(`Invalid ${type.slice(0, -1)} upload path: outside allowed directory`);
        }

        return resolvedPath;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string sanitizeFilename(const std::string& filename) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!filename) {
        return 'unnamed';
    }

    // Remove path separators and null bytes
    const auto sanitized = filename;
    .replace(/[/\\:*?"<>|]/g, '_')
    .replace(/\0/g, '');
    .replace(/\.+/g, '.');
    .trim();

    // Ensure filename isn't empty after sanitization
    if (!sanitized || sanitized == '.') {
        return 'unnamed';
    }

    // Limit filename length
    const auto maxLength = 255;
    if (sanitized.length > maxLength) {
        const auto ext = path.extname(sanitized);
        const auto nameWithoutExt = path.basename(sanitized, ext);
        const auto truncatedName = nameWithoutExt.substring(0, maxLength - ext.length - 1);
        return truncatedName + ext;
    }

    return sanitized;

}

} // namespace elizaos
