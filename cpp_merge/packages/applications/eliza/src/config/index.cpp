#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-starter/src/config/index.h"

object parseArguments()
{
    try
    {
        return yargs(process->argv->slice(2))->option(std:("character"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Path to the character JSON file")}
        })->option(std:("characters"), object{
            object::pair{std:("type"), std:("string")}, 
            object::pair{std:("description"), std:("Comma separated list of paths to character JSON files")}
        })->parseSync();
    }
    catch (const any& error)
    {
        console->error(std:("Error parsing arguments:"), error);
        return object{};
    }
};


std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharacters(string charactersArg)
{
    auto characterPaths = charactersArg->split(std:(","))->map([=](auto filePath) mutable
    {
        if (path->basename(filePath) == filePath) {
            filePath = std:("../characters/") + filePath;
        }
        return path->resolve(process->cwd(), filePath->trim());
    }
    );
    auto loadedCharacters = array<any>();
    if (characterPaths->get_length() > 0) {
        for (auto& path : characterPaths)
        {
            try
            {
                auto character = JSON->parse(fs->readFileSync(path, std:("utf8")));
                validateCharacterConfig(character);
                loadedCharacters->push(character);
            }
            catch (const any& e)
            {
                console->error(std:("Error loading character from ") + path + std:(": ") + e + string_empty);
                process->exit(1);
            }
        }
    }
    return loadedCharacters;
};


any getTokenForProvider(std::shared_ptr<ModelProviderName> provider, std::shared_ptr<Character> character)
{
    switch_type __switch1603_3090 = {
        { any(ModelProviderName->OPENAI), 1 },
        { any(ModelProviderName->LLAMACLOUD), 2 },
        { any(ModelProviderName->ANTHROPIC), 3 },
        { any(ModelProviderName->REDPILL), 4 },
        { any(ModelProviderName->OPENROUTER), 5 },
        { any(ModelProviderName->GROK), 6 },
        { any(ModelProviderName->HEURIST), 7 },
        { any(ModelProviderName->GROQ), 8 }
    };
    switch (__switch1603_3090[provider])
    {
    case 1:
        return (OR((character->settings->secrets->OPENAI_API_KEY), (settings->OPENAI_API_KEY)));
    case 2:
        return (OR((OR((OR((OR((OR((OR((OR((character->settings->secrets->LLAMACLOUD_API_KEY), (settings->LLAMACLOUD_API_KEY))), (character->settings->secrets->TOGETHER_API_KEY))), (settings->TOGETHER_API_KEY))), (character->settings->secrets->XAI_API_KEY))), (settings->XAI_API_KEY))), (character->settings->secrets->OPENAI_API_KEY))), (settings->OPENAI_API_KEY)));
    case 3:
        return (OR((OR((OR((character->settings->secrets->ANTHROPIC_API_KEY), (character->settings->secrets->CLAUDE_API_KEY))), (settings->ANTHROPIC_API_KEY))), (settings->CLAUDE_API_KEY)));
    case 4:
        return (OR((character->settings->secrets->REDPILL_API_KEY), (settings->REDPILL_API_KEY)));
    case 5:
        return (OR((character->settings->secrets->OPENROUTER), (settings->OPENROUTER_API_KEY)));
    case 6:
        return OR((character->settings->secrets->GROK_API_KEY), (settings->GROK_API_KEY));
    case 7:
        return (OR((character->settings->secrets->HEURIST_API_KEY), (settings->HEURIST_API_KEY)));
    case 8:
        return OR((character->settings->secrets->GROQ_API_KEY), (settings->GROQ_API_KEY));
    }
};


