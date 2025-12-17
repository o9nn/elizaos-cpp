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
 * Class representing a TypeScript file identifier.
 */
/**
 * Check if the given file is a TypeScript file based on its extension.
 * * @param { string } file - The file to check.
 * @returns { boolean } Returns true if the file is a TypeScript file (.ts or .tsx), otherwise false.
 */
/**
 * Retrieves an array of TypeScript files from the specified directory.
 * * @param { string } directory - The directory path to search for TypeScript files.
 * @returns {string[]} - An array of TypeScript files found in the directory.
 */
class TypeScriptFileIdentifier {
public:
    bool isTypeScriptFile(const std::string& file);
    std::vector<std::string> getTypeScriptFiles(const std::string& directory);
};


} // namespace elizaos
