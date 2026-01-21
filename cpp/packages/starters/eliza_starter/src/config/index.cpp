#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void parseArguments() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    character?: string;
    characters?: string;

}

std::future<std::vector<Character>> loadCharacters(const std::string& charactersArg) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto characterPaths = charactersArg.split(",").map((filePath) => {;
        if (path.basename(filePath) == filePath) {
            filePath = "../characters/" + filePath;
        }
        return path.resolve(process.cwd(), filePath.trim());
        });

        const auto loadedCharacters = [];

        if (characterPaths.length > 0) {
            for (const auto& path : characterPaths)
                try {
                    const auto character = /* JSON.parse */ fs.readFileSync(path, "utf8");

                    validateCharacterConfig(character);

                    loadedCharacters.push_back(character);
                    } catch (e) {
                        std::cerr << "Error loading character from " + path + ": " + e << std::endl;
                        // don't continue to load if a specified file is not found
                        process.exit(1);
                    }
                }
            }

            return loadedCharacters;

}

void getTokenForProvider(ModelProviderName provider, Character character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (provider) {
        case ModelProviderName.OPENAI:
        return (;
        character.settings.secrets.OPENAI_API_KEY || settings.OPENAI_API_KEY;
        );
        case ModelProviderName.LLAMACLOUD:
        return (;
        character.settings.secrets.LLAMACLOUD_API_KEY ||;
        settings.LLAMACLOUD_API_KEY ||;
        character.settings.secrets.TOGETHER_API_KEY ||;
        settings.TOGETHER_API_KEY ||;
        character.settings.secrets.XAI_API_KEY ||;
        settings.XAI_API_KEY ||;
        character.settings.secrets.OPENAI_API_KEY ||;
        settings.OPENAI_API_KEY;
        );
        case ModelProviderName.ANTHROPIC:
        return (;
        character.settings.secrets.ANTHROPIC_API_KEY ||;
        character.settings.secrets.CLAUDE_API_KEY ||;
        settings.ANTHROPIC_API_KEY ||;
        settings.CLAUDE_API_KEY;
        );
        case ModelProviderName.REDPILL:
        return (;
        character.settings.secrets.REDPILL_API_KEY || settings.REDPILL_API_KEY;
        );
        case ModelProviderName.OPENROUTER:
        return (;
        character.settings.secrets.OPENROUTER || settings.OPENROUTER_API_KEY;
        );
        case ModelProviderName.GROK:
        return character.settings.secrets.GROK_API_KEY || settings.GROK_API_KEY;
        case ModelProviderName.HEURIST:
        return (;
        character.settings.secrets.HEURIST_API_KEY || settings.HEURIST_API_KEY;
        );
        case ModelProviderName.GROQ:
        return character.settings.secrets.GROQ_API_KEY || settings.GROQ_API_KEY;
    }

}

} // namespace elizaos
