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



// UUID validation schema

// Message content schema matching the Content interface

// MessageExample schema

// DirectoryItem schema

// Knowledge item can be a string, object with path, or DirectoryItem

// TemplateType schema - can be string or function (we'll validate as string for JSON)

// Style configuration schema

// Settings schema - flexible object

// Secrets schema

// Main Character schema

// Validation result type
struct CharacterValidationResult {
    bool success;
    std::optional<Character> data;
    std::optional<{> error;
    std::string message;
    std::optional<std::vector<z.ZodIssue>> issues;
};

/**
 * Safely validates character data using Zod schema
 * @param data - Raw character data to validate
 * @returns Validation result with success flag and either data or error
 */
CharacterValidationResult validateCharacter(unknown data);

/**
 * Safely parses JSON string and validates as character
 * @param jsonString - JSON string to parse and validate
 * @returns Validation result with success flag and either data or error
 */
CharacterValidationResult parseAndValidateCharacter(const std::string& jsonString);

/**
 * Type guard to check if data is a valid Character
 * @param data - Data to check
 * @returns True if data is a valid Character
 */

} // namespace elizaos
