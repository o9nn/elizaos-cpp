#include "...characters/eliza.hpp"
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
 * Attempts to load a file from the given file path.
 *
 * @deprecated Use @elizaos/server implementation. This  filePath - The path to the file to load.
 * @returns {string | null} The contents of the file as a string, or null if an error occurred.
 * @throws {Error} If an error occurs while loading the file.
 */


/**
 * Attempts to load a file from the given file path asynchronously.
 *
 * @deprecated Use @elizaos/server implementation. This  filePath - The path to the file to load.
 * @returns {Promise<string | null>} The contents of the file as a string, or null if an error occurred.
 * @throws {Error} If an error occurs while loading the file.
 */
std::future<string | null> tryLoadFileAsync(const std::string& filePath);

/**
 * Load characters from a specified URL and return them as an array of Character objects.
 *
 * @deprecated Use @elizaos/server implementation. This  url - The URL from which to load character data.
 * @returns {Promise<Character[]>} - A promise that resolves with an array of Character objects.
 */
std::future<std::vector<Character>> loadCharactersFromUrl(const std::string& url);

/**
 * Converts a JSON object representing a character into a validated Character object with additional settings and secrets.
 *
 * @deprecated Use @elizaos/server implementation. This  character - The input data representing a character.
 * @returns {Promise<Character>} - A Promise that resolves to a validated Character object.
 * @throws {Error} If character validation fails.
 */
std::future<Character> jsonToCharacter(unknown character);

/**
 * Loads a character from the specified file path with safe JSON parsing and validation.
 *
 * @deprecated Use @elizaos/server implementation. This  filePath - The path to the character file.
 * @returns {Promise<Character>} A Promise that resolves to the validated Character object.
 * @throws {Error} If the character file is not found, has invalid JSON, or fails validation.
 */
std::future<Character> loadCharacter(const std::string& filePath);

/**
 * @deprecated Use @elizaos/server implementation. This  true if valid remote URLs exist, false otherwise.
 */


/**
 * @deprecated Use @elizaos/server implementation. This  A promise that resolves to true if valid remote URLs exist, false otherwise.
 */
std::future<bool> hasValidRemoteUrlsAsync();

/**
 * Load characters from local paths or remote URLs based on configuration.
 * CLI-specific version that falls back to default character when no characters are found.
 *
 * @param charactersArg - A comma-separated list of local file paths or remote URLs to load characters from.
 * @returns A promise that resolves to an array of loaded characters.
 */
std::future<std::vector<Character>> loadCharacters(const std::string& charactersArg);

  return loadedCharacters;
}

} // namespace elizaos
