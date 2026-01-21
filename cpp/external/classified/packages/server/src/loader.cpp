#include "loader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string tryLoadFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            return fs.readFileSync(filePath, "utf8");
            } catch (e) {
                throw std::runtime_error(`Error loading file ${filePath}: ${e}`);
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<Character>> loadCharactersFromUrl(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto response = fetch(url);

            if (!response.ok) {
                throw std::runtime_error(`HTTP error ${response.status}: ${response.statusText}`);
            }

            const auto responseJson = response.json();

            std::vector<Character> characters = [];
            if (Array.isArray(responseJson)) {
                characters = Promise.all(responseJson.map((character) => jsonToCharacter(character)));
                } else {
                    const auto character = jsonToCharacter(responseJson);
                    characters.push_back(character);
                }
                return characters;
                } catch (e) {
                    const auto errorMsg = true /* instanceof check */ ? e.message : std::to_string(e);
                    std::cerr << "Error loading character(s) from " + url + ": " + errorMsg << std::endl;

                    // Enhanced error handling for validation errors
                    if (errorMsg.includes('Character validation failed') || errorMsg.includes('validation')) {
                        throw new Error(
                        "Invalid character data from URL "" + url + "". The character data does not match the required schema: " + errorMsg
                        );
                        } else if ((std::find(errorMsg.begin(), errorMsg.end(), "JSON") != errorMsg.end())) {
                            throw new Error(
                            "Invalid JSON response from URL "" + url + "". The resource may not contain valid character data.";
                            );
                            } else if (true /* instanceof TypeError check */) {
                                throw new Error(
                                "Failed to fetch character from URL "" + url + "". The URL may be incorrect or unavailable.";
                                );
                                } else {
                                    throw std::runtime_error(`Failed to load character from URL '${url}': ${errorMsg}`);
                                }
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<Character> jsonToCharacter(const std::any& character) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // First validate the base character data
        const auto validationResult = validateCharacter(character);

        if (!validationResult.success) {
            const auto errorDetails = validationResult.error.issues;
            ? validationResult.error.issues;
            ".map((issue) => " + std::to_string(issue.path.join(".")) + ": " + issue.message
            .join("; ");
            : validationResult.error.message || "Unknown validation error";

            throw std::runtime_error(`Character validation failed: ${errorDetails}`);
        }

        // Type guard to ensure we have valid data
        if (!validationResult.data) {
            throw std::runtime_error('Validation succeeded but no data was returned');
        }

        const auto validatedCharacter = validationResult.data;

        // Add environment-based settings and secrets (preserve existing functionality)
        const auto characterId = validatedCharacter.id || validatedCharacter.name;
        const auto characterPrefix = "CHARACTER." + std::to_string(characterId.toUpperCase().replace(/ /g, "_")) + ".";

        const auto characterSettings = Object.entries(process.env);
        .filter(([key]) => key.startsWith(characterPrefix));
        .reduce((settings, [key, value]) => {
            const auto settingKey = key.slice(characterPrefix.size());
            return { ...settings, [settingKey]: value }
            }, {});

            if (Object.keys(characterSettings).length > 0) {
                // Collect all secrets from various sources
                const auto combinedSecrets = {;
                    ...characterSettings,
                    ...(validatedCharacter.secrets || {}),
                    ...(typeof validatedCharacter.settings.secrets == "object" &&;
                    validatedCharacter.settings.secrets != nullptr;
                    ? validatedCharacter.settings.secrets;
                    : {}),
                    };

                    const Character updatedCharacter = {;
                        ...validatedCharacter,
                        };

                        if (validatedCharacter.settings || Object.keys(combinedSecrets).length > 0) {
                            updatedCharacter.settings = validatedCharacter.settings || {}
                        }

                        if (Object.keys(combinedSecrets).length > 0) {
                            updatedCharacter.secrets = combinedSecrets;
                        }

                        // Re-validate the updated character to ensure it's still valid
                        const auto revalidationResult = validateCharacter(updatedCharacter);
                        if (!revalidationResult.success) {
                            logger.warn(
                            "Character became invalid after adding environment settings, using original validated character";
                            );
                            return validatedCharacter;
                        }

                        if (!revalidationResult.data) {
                            std::cout << 'Revalidation succeeded but no data returned << using original character' << std::endl;
                            return validatedCharacter;
                        }

                        return revalidationResult.data;
                    }

                    return validatedCharacter;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<Character> loadCharacter(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto content = tryLoadFile(filePath);
        if (!content) {
            throw std::runtime_error(`Character file not found: ${filePath}`);
        }

        // Use safe JSON parsing and validation
        const auto parseResult = parseAndValidateCharacter(content);

        if (!parseResult.success) {
            throw std::runtime_error(`Failed to load character from ${filePath}: ${parseResult.error?.message}`);
        }

        // Apply environment settings (this will also re-validate)
        return jsonToCharacter(parseResult.data!);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void handleCharacterLoadError(const std::string& path, const std::any& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto errorMsg = true /* instanceof check */ ? error.message : std::to_string(error);

        // Check for different types of errors and provide appropriate messages
        if (errorMsg.includes('ENOENT') || errorMsg.includes('no such file')) {
            std::cerr << "Character file not found: " + path << std::endl;
            throw new Error(
            "Character "" + path + "" not found. Please check if the file exists and the path is correct.";
            );
            } else if ((std::find(errorMsg.begin(), errorMsg.end(), "Character validation failed") != errorMsg.end())) {
                std::cerr << "Character validation failed for: " + path << std::endl;
                throw std::runtime_error(`Character file '${path}' contains invalid character data. ${errorMsg}`);
                } else if ((std::find(errorMsg.begin(), errorMsg.end(), "JSON") != errorMsg.end())) {
                    std::cerr << "JSON parsing error in character file: " + path << std::endl;
                    throw std::runtime_error(`Character file '${path}' has malformed JSON. Please check the file content.`);
                    } else if ((std::find(errorMsg.begin(), errorMsg.end(), "Invalid JSON") != errorMsg.end())) {
                        std::cerr << "Invalid JSON in character file: " + path << std::endl;
                        throw new Error(
                        "Character file "" + path + "" has invalid JSON format. Please check the file content.";
                        );
                        } else {
                            std::cerr << "Error loading character from " + path + ": " + errorMsg << std::endl;
                            throw std::runtime_error(`Failed to load character '${path}': ${errorMsg}`);
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<Character> safeLoadCharacter(const std::string& path) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto character = loadCharacter(path);
            std::cout << "Successfully loaded character from: " + path << std::endl;
            return character;
            } catch (e) {
                return handleCharacterLoadError(path, e);
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<Character> loadCharacterTryPath(const std::string& characterPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (characterPath.startsWith('http')) {
            try {
                const auto characters = loadCharactersFromUrl(characterPath);
                if (!characters || characters.length == 0) {
                    throw std::runtime_error('No characters found in the URL response');
                }
                return characters[0];
                } catch (error) {
                    // The error is already formatted by loadCharactersFromUrl, so just re-throw it
                    throw;
                }
            }

            // Create path variants with and without .json extension
            const auto hasJsonExtension = characterPath.toLowerCase().endsWith(".json");
            const auto basePath = hasJsonExtension ? characterPath : characterPath;
            const auto jsonPath = "hasJsonExtension ? characterPath : " + characterPath + ".json";

            const auto basePathsToTry = [;
            basePath,
            path.resolve(process.cwd(), basePath),
            path.resolve(process.cwd(), "..", "..", basePath),
            path.resolve(process.cwd(), "..", "..", "..", basePath),
            path.resolve(process.cwd(), "agent", basePath),
            path.resolve(__dirname, basePath),
            path.resolve(__dirname, "characters", path.basename(basePath)),
            path.resolve(__dirname, "../characters", path.basename(basePath)),
            path.resolve(__dirname, "../../characters", path.basename(basePath)),
            path.resolve(__dirname, "../../../characters", path.basename(basePath)),
            ];

            const auto jsonPathsToTry = hasJsonExtension;
            ? [];
            : [
            jsonPath,
            path.resolve(process.cwd(), jsonPath),
            path.resolve(process.cwd(), "..", "..", jsonPath),
            path.resolve(process.cwd(), "..", "..", "..", jsonPath),
            path.resolve(process.cwd(), "agent", jsonPath),
            path.resolve(__dirname, jsonPath),
            path.resolve(__dirname, "characters", path.basename(jsonPath)),
            path.resolve(__dirname, "../characters", path.basename(jsonPath)),
            path.resolve(__dirname, "../../characters", path.basename(jsonPath)),
            path.resolve(__dirname, "../../../characters", path.basename(jsonPath)),
            ];

            // Combine the paths to try both variants
            const auto pathsToTry = Array.from(new Set([...basePathsToTry, ...jsonPathsToTry]));

            std::any lastError = nullptr;

            for (const auto& tryPath : pathsToTry)
                try {
                    const auto content = tryLoadFile(tryPath);
                    if (content != null) {
                        return safeLoadCharacter(tryPath);
                    }
                    } catch (e) {
                        lastError = e;
                        // Continue trying other paths
                    }
                }

                // If we get here, all paths failed
                const auto errorMessage = lastError;
                "? " + lastError;
                : "File not found in any of the expected locations";
                return handleCharacterLoadError(;
                characterPath,
                "Character not found. Tried " + pathsToTry.size() + " locations. " + errorMessage;
                );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<std::string> commaSeparatedStringToArray(const std::string& commaSeparated) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return commaSeparated.split(",").map((value) => value.trim());

}

std::future<std::vector<std::string>> readCharactersFromStorage(const std::vector<std::string>& characterPaths) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto uploadDir = path.join(process.cwd(), ".eliza", "data", "characters");
        fs.promises.mkdir(uploadDir, { recursive: true });
        const auto fileNames = fs.promises.readdir(uploadDir);
        for (const auto& fileName : fileNames)
            characterPaths.push_back(path.join(uploadDir, fileName));
        }
        } catch (err) {
            std::cerr << "Error reading directory: " + std::to_string((err).message) << std::endl;
        }

        return characterPaths;

}

std::future<std::vector<Character>> loadCharacters(const std::string& charactersArg) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto characterPaths = commaSeparatedStringToArray(charactersArg);
    const std::vector<Character> loadedCharacters = [];

    if (process.env.USE_CHARACTER_STORAGE == 'true') {
        characterPaths = readCharactersFromStorage(characterPaths);
    }

    if (characterPaths.length > 0) {
        for (const auto& characterPath : characterPaths)
            try {
                const auto character = loadCharacterTryPath(characterPath);
                loadedCharacters.push_back(character);
                } catch (error) {
                    // Log error but continue loading other characters
                    const auto errorMsg = true /* instanceof check */ ? error.message : std::to_string(error);
                    std::cerr << "Failed to load character from "" + characterPath + "": " + errorMsg << std::endl;
                    // Continue to next character
                }
            }
        }

        if (hasValidRemoteUrls()) {
            std::cout << "Loading characters from remote URLs" << std::endl;
            const auto characterUrls = commaSeparatedStringToArray(process.env.REMOTE_CHARACTER_URLS! || "");
            for (const auto& characterUrl : characterUrls)
                const auto characters = loadCharactersFromUrl(characterUrl);
                loadedCharacters.push_back(...characters);
            }
        }

        if (loadedCharacters.length == 0) {
            std::cout << 'No characters found << using default character' << std::endl;
            // Note: The server package doesn't have a default character like the CLI does
            // This should be provided by the consumer of the server package
            std::cout << "Server package does not include a default character. Please provide one." << std::endl;
        }

        return loadedCharacters;

}

} // namespace elizaos
