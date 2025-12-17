#include "loader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> tryLoadFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            return fs.readFileSync(filePath, 'utf8');
            } catch (e) {
                // Only catch file not found errors, let other errors propagate
                if (e instanceof Error && 'code' in e && (e as NodeJS.ErrnoException).code == 'ENOENT') {
                    return nullptr;
                }
                throw e;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<Character>> loadCharactersFromUrl(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement
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
                characters.push(character);
            }
            return characters;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<Character> jsonToCharacter(unknown character) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // First validate the base character data
        const auto validationResult = validateCharacter(character);

        if (!validationResult.success) {
            const auto errorDetails = validationResult.error.issues;
            ? validationResult.error.issues;
            std::to_string(issue.path.join('.')) + ": " + std::to_string(issue.message)
            .join('; ');
            : validationResult.error.message || 'Unknown validation error';

            throw std::runtime_error(`Character validation failed: ${errorDetails}`);
        }

        // Type guard to ensure we have valid data
        if (!validationResult.data) {
            throw std::runtime_error('Validation succeeded but no data was returned');
        }

        const auto validatedCharacter = validationResult.data;

        // Add environment-based settings and secrets (preserve existing functionality)
        const auto characterId = validatedCharacter.id || validatedCharacter.name;
        const auto characterPrefix = "CHARACTER." + std::to_string(characterId.toUpperCase().replace(/ /g, '_')) + ".";

        const auto characterSettings = Object.entries(process.env);
        .filter(([key]) => key.startsWith(characterPrefix));
        .reduce((settings, [key, value]) => {
            const auto settingKey = key.slice(characterPrefix.length);
            return { ...settings, [settingKey]: value }
            }, {});

            if (Object.keys(characterSettings).length > 0) {
                // Collect all secrets from various sources
                const auto combinedSecrets = {;
                    ...characterSettings,
                    ...(validatedCharacter.secrets || {}),
                    ...(typeof validatedCharacter.settings.secrets == 'object' &&;
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
                            throw std::runtime_error('Character became invalid after adding environment settings');
                        }

                        if (!revalidationResult.data) {
                            throw std::runtime_error('Revalidation succeeded but no data returned');
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

std::future<Character> loadCharacterTryPath(const std::string& characterPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (characterPath.startsWith('http')) {
            const auto characters = loadCharactersFromUrl(characterPath);
            if (!characters || characters.length == 0) {
                throw std::runtime_error('No characters found in the URL response');
            }
            return characters[0];
        }

        // Create path variants with and without .json extension
        const auto hasJsonExtension = characterPath.toLowerCase().endsWith('.json');
        const auto basePath = hasJsonExtension ? characterPath : characterPath;
        const auto jsonPath = std::to_string(characterPath) + ".json";

        const auto basePathsToTry = [;
        basePath,
        path.resolve(process.cwd(), basePath),
        path.resolve(process.cwd(), '..', '..', basePath),
        path.resolve(process.cwd(), '..', '..', '..', basePath),
        path.resolve(process.cwd(), 'agent', basePath),
        path.resolve(__dirname, basePath),
        path.resolve(__dirname, 'characters', path.basename(basePath)),
        path.resolve(__dirname, '../characters', path.basename(basePath)),
        path.resolve(__dirname, '../../characters', path.basename(basePath)),
        path.resolve(__dirname, '../../../characters', path.basename(basePath)),
        ];

        const auto jsonPathsToTry = hasJsonExtension;
        ? [];
        : [
        jsonPath,
        path.resolve(process.cwd(), jsonPath),
        path.resolve(process.cwd(), '..', '..', jsonPath),
        path.resolve(process.cwd(), '..', '..', '..', jsonPath),
        path.resolve(process.cwd(), 'agent', jsonPath),
        path.resolve(__dirname, jsonPath),
        path.resolve(__dirname, 'characters', path.basename(jsonPath)),
        path.resolve(__dirname, '../characters', path.basename(jsonPath)),
        path.resolve(__dirname, '../../characters', path.basename(jsonPath)),
        path.resolve(__dirname, '../../../characters', path.basename(jsonPath)),
        ];

        // Combine the paths to try both variants
        const auto pathsToTry = Array.from(new Set([...basePathsToTry, ...jsonPathsToTry]));

        for (const auto& tryPath : pathsToTry)
            const auto content = tryLoadFile(tryPath);
            if (content != null) {
                try {
                    return loadCharacter(tryPath);
                    } catch (e) {
                        // If it's a validation/parsing error, throw immediately (fail fast)
                        // Only continue if it's a file not found error
                        if (
                        !(true /* instanceof check */ && 'code' in e && (e.ErrnoException).code == 'ENOENT');
                        ) {
                            throw e;
                        }
                    }
                }
            }

            // If we get here, all paths failed
            throw std::runtime_error(`Character not found at ${characterPath}. Tried ${pathsToTry.length} locations.`);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<std::string> commaSeparatedStringToArray(const std::string& commaSeparated) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return commaSeparated.split(',').map((value) => value.trim());

}

std::future<std::vector<std::string>> readCharactersFromStorage(const std::vector<std::string>& characterPaths) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto uploadDir = path.join(process.cwd(), '.eliza', 'data', 'characters');

        try {
            fs.promises.mkdir(uploadDir, { recursive: true });
            const auto fileNames = fs.promises.readdir(uploadDir);
            for (const auto& fileName : fileNames)
                characterPaths.push(path.join(uploadDir, fileName));
            }
            } catch (err) {
                // Only catch expected errors like directory not existing
                if (err instanceof Error && 'code' in err) {
                    const auto nodeErr = err.ErrnoException;
                    if (nodeErr.code == 'ENOENT' || nodeErr.code == 'EACCES') {
                        std::cerr << "Cannot access character storage directory: " + std::to_string(nodeErr.message) << std::endl;
                        // Return original characterPaths, don't fail the whole process
                        return characterPaths;
                    }
                }
                // Unexpected errors should propagate
                throw err;
            }

            return characterPaths;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
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
            const auto character = loadCharacterTryPath(characterPath);
            loadedCharacters.push(character);
        }
    }

    if (hasValidRemoteUrls()) {
        std::cout << 'Loading characters from remote URLs' << std::endl;
        const auto characterUrls = commaSeparatedStringToArray(process.env.REMOTE_CHARACTER_URLS! || '');
        for (const auto& characterUrl : characterUrls)
            const auto characters = loadCharactersFromUrl(characterUrl);
            loadedCharacters.push(...characters);
        }
    }

    if (loadedCharacters.length == 0) {
        std::cout << 'No characters found << using default character' << std::endl;
        // Note: The server package doesn't have a default character like the CLI does
        // This should be provided by the consumer of the server package
        std::cout << 'Server package does not include a default character. Please provide one.' << std::endl;
    }

    return loadedCharacters;

}

} // namespace elizaos
