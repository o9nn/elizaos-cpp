#include "index.hpp"

any start = ((std::make_shared<Command>()))->name(std:("start"))->description(std:("Start the Eliza agent server"))->option(std:("-c, --configure"), std:("Reconfigure services and AI models"))->option(std:("-p, --port <port>"), std:("Port to listen on"), validatePort)->option(std:("--character <paths...>"), std:("Character file(s) to use"))->hook(std:("preAction"), [=]() mutable
{
    std::async([=]() { displayBanner(); });
}
)->action([=](auto options) mutable
{
    try
    {
        std::async([=]() { loadEnvConfig(); });
        auto characters = array<std::shared_ptr<Character>>();
        auto projectAgents = array<std::shared_ptr<ProjectAgent>>();
        if (AND((options->character), (options->character->get_length() > 0))) {
            for (auto& charPath : options->character)
            {
                auto resolvedPath = path->resolve(charPath);
                if (!fs->existsSync(resolvedPath)) {
                    logger->error(std:("Character file not found: ") + resolvedPath + string_empty);
                    throw any(std::make_shared<Error>(std:("Character file not found: ") + resolvedPath + string_empty));
                }
                try
                {
                    auto character = std::async([=]() { loadCharacterTryPath(resolvedPath); });
                    if (character) {
                        characters->push(character);
                        logger->info(std:("Successfully loaded character: ") + character->name + string_empty);
                    } else {
                        logger->error(std:("Failed to load character from ") + resolvedPath + std:(": Invalid or empty character file"));
                        throw any(std::make_shared<Error>(std:("Invalid character file: ") + resolvedPath + string_empty));
                    }
                }
                catch (const any& e)
                {
                    logger->error(std:("Failed to load character from ") + resolvedPath + std:(":"), e);
                    throw any(std::make_shared<Error>(std:("Invalid character file: ") + resolvedPath + string_empty));
                }
            }
        } else {
            try
            {
                auto cwd = process->cwd();
                auto dirInfo = detectDirectoryType(cwd);
                if (AND((dirInfo->hasPackageJson), (dirInfo->type != std:("non-elizaos-dir")))) {
                    logger->info(std:("No character files specified, attempting to load project agents..."));
                    auto project = std::async([=]() { loadProject(cwd); });
                    if (AND((project->agents), (project->agents->length > 0))) {
                        logger->info(std:("Found ") + project->agents->length + std:(" agent(s) in project configuration"));
                        projectAgents = project->agents;
                        for (auto& agent : project->agents)
                        {
                            if (agent->character) {
                                logger->info(std:("Loaded character: ") + agent->character->name + string_empty);
                            }
                        }
                    }
                }
            }
            catch (const any& e)
            {
                logger->debug(std:("Failed to load project agents, will use default character:"), e);
            }
        }
        std::async([=]() { startAgents(utils::assign(object{
            , 
            object::pair{std:("characters"), std:("characters")}, 
            object::pair{std:("projectAgents"), std:("projectAgents")}
        }, options)); });
    }
    catch (const any& e)
    {
        handleError(e);
        process->exit(1);
    }
}
);

void Main(void)
{
}

MAIN
