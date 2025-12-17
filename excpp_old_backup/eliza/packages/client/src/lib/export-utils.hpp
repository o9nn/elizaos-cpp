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


struct ExportResult {
    bool success;
    std::optional<std::string> filename;
    std::optional<std::string> error;
};

struct ToastFunction {
};

/**
 * Sanitizes a filename by replacing non-alphanumeric characters with dashes
 * and cleaning up multiple consecutive dashes
 */
std::string sanitizeFilename(const std::string& name);

/**
 * Converts an agent to character JSON data, excluding sensitive information
 */
std::unordered_map<std::string, std::any> agentToCharacterData(Agent agent);

/**
 * Generates a filename for the exported character
 */
std::string generateExportFilename(const std::string& agentName);

/**
 * Creates and triggers a download of a JSON file
 */
void downloadJsonFile(const std::any& data, const std::string& filename);

/**
 * Exports an agent's character data as a JSON file download
 * @param agent - The agent containing the character data to * @param toast - Optional toast function for notifications
 * @returns Export result with success status and filename or error
 */
ExportResult exportCharacterAsJson(Agent agent, std::optional<ToastFunction> toast);

} // namespace elizaos
