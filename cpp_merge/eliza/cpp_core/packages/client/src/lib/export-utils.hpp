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


struct ExportResult {
    bool success;
    std::optional<std:> filename;
    std::optional<std:> error;
};

struct ToastFunction {
    std: description;
};

/**
 * Sanitizes a filename by replacing non-alphanumeric characters with dashes
 * and cleaning up multiple consecutive dashes
 */
std: sanitizeFilename(const std:& name);

/**
 * Converts an agent to character JSON data, excluding sensitive information
 */
std::unordered_map<std:, std:> agentToCharacterData(Agent agent);

/**
 * Generates a filename for the exported character
 */
std: generateExportFilename(const std:& agentName);

/**
 * Creates and triggers a download of a JSON file
 */
void downloadJsonFile(const std:& data, const std:& filename);

/**
 * Exports an agent's character data as a JSON file download
 * @param agent - The agent containing the character data to * @param toast - Optional toast std::function for notifications
 * @returns Export result with success status and filename or error
 */
ExportResult exportCharacterAsJson(Agent agent, std::optional<ToastFunction> toast);

} // namespace elizaos
