#pragma once
#include <functional>
#include <future>
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
 * Attempts to load a file from the given file path.
 *
 * @param {string} filePath - The path to the file to load.
 * @returns {string | null} The contents of the file as a string, or null if an error occurred.
 * @throws {Error} If an error occurs while loading the file.
 */
std::string tryLoadFile(const std::string& filePath);

/**
 * Load characters from a specified URL and return them as an array of Character objects.
 * @param {string} url - The URL from which to load character data.
 * @returns {Promise<Character[]>} - A promise that resolves with an array of Character objects.
 */

/**
 * Converts a JSON object representing a character into a validated Character object with additional settings and secrets.
 *
 * @param {unknown} character - The input data representing a character.
 * @returns {Promise<Character>} - A Promise that resolves to a validated Character object.
 * @throws {Error} If character validation fails.
 */
std::future<Character> jsonToCharacter(unknown character);

/**
 * Loads a character from the specified file path with safe JSON parsing and validation.
 *
 * @param {string} filePath - The path to the character file.
 * @returns {Promise<Character>} A Promise that resolves to the validated Character object.
 * @throws {Error} If the character file is not found, has invalid JSON, or fails validation.
 */
std::future<Character> loadCharacter(const std::string& filePath);

/**
 * Handles errors when loading a character from a specific path.
 *
 * @param {string} path - The path from which the character is being loaded.
 * @returns {never}
 */
never handleCharacterLoadError(const std::string& path, unknown error);

/**
 * Asynchronously loads a character from the specified path while handling any potential errors.
 *
 * @param {string} path - The path to load the character from.
 * @returns {Promise<Character>} A promise that resolves to the loaded character.
 */
std::future<Character> safeLoadCharacter(const std::string& path);

/**
 * Asynchronously loads a character from the specified path.
 * If the path is a URL, it loads the character from the URL.
 * If the path is a local file path, it tries multiple possible locations and
 * loads the character from the first valid location found.
 *
 * @param {string} characterPath - The path to load the character from.
 * @returns {Promise<Character>} A Promise that resolves to the loaded character.
 */
std::future<Character> loadCharacterTryPath(const std::string& characterPath);

/**
 * Converts a comma-separated string to an array of strings.
 *
 * @param {string} commaSeparated - The input comma-separated string.
 * @returns {string[]} An array of strings after splitting the input string by commas and trimming each value.
 */
std::vector<std::string> commaSeparatedStringToArray(const std::string& commaSeparated);

/**
 * Asynchronously reads character files from the storage directory and pushes their paths to the characterPaths array.
 * @param {string[]} characterPaths - An array of paths where the character files will be stored.
 * @returns {Promise<string[]>} - A promise that resolves with an updated array of characterPaths.
 */

/**
 * Load characters from local paths or remote URLs based on configuration.
 * @param charactersArg - A comma-separated list of local file paths or remote URLs to load characters from.
 * @returns A promise that resolves to an array of loaded characters.
 */

} // namespace elizaos
