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
;
;
;

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);



  try {
    return fs.readFileSync(filePath, "utf8");
  } catch (e) {
    return null;
  }
}



std::future<std::vector<Character>> loadCharacters(const std::string& characterPath);: File not found`,
    );
    process.exit(1);
  }

  try {
    const character = JSON.parse(content);
    validateCharacterConfig(character);

    if (isAllStrings(character.plugins)) {
      elizaLogger.info("Plugins are: ", character.plugins);
      const importedPlugins = await Promise.all(
        character.plugins.map(async (plugin: any) => {
          const importedPlugin = await import(plugin);
          return importedPlugin.default;
        }),
      );
      character.plugins = importedPlugins;
    }

    loadedCharacters.push(character);
    elizaLogger.info(`Successfully loaded character from: ${characterPath}`);
  } catch (e) {
    elizaLogger.error(`Error parsing character from ${characterPath}: ${e}`);
    process.exit(1);
  }

  if (loadedCharacters.length === 0) {
    elizaLogger.info("No characters found, using default character");
    loadedCharacters.push(defaultCharacter);
  }

  return loadedCharacters;
}

} // namespace elizaos
