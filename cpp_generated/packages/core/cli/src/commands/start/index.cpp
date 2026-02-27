#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/start/index.h"

any start = ((std::make_shared<Command>()))->name(std::string("start"))->description(std::string("Start the Eliza agent server"))->option(std::string("-c, --configure"), std::string("Reconfigure services and AI models"))->option(std::string("-p, --port <port>"), std::string("Port to listen on"), validatePort)->option(std::string("--character <paths...>"), std::string("Character file(s) to use"))->hook(std::string("preAction"), [=]() mutable
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
                    logger->error(std::string("Character file not found: ") + resolvedPath + string_empty);
                    throw any(std::make_shared<Error>(std::string("Character file not found: ") + resolvedPath + string_empty));
                }
                try
                {
                    auto character = std::async([=]() { loadCharacterTryPath(resolvedPath); });
                    if (character) {
                        characters->push(character);
                        logger->info(std::string("Successfully loaded character: ") + character->name + string_empty);
                    } else {
                        logger->error(std::string("Failed to load character from ") + resolvedPath + std::string(": Invalid or empty character file"));
                        throw any(std::make_shared<Error>(std::string("Invalid character file: ") + resolvedPath + string_empty));
                    }
                }
                catch (const any& e)
                {
                    logger->error(std::string("Failed to load character from ") + resolvedPath + std::string(":"), e);
                    throw any(std::make_shared<Error>(std::string("Invalid character file: ") + resolvedPath + string_empty));
                }
            }
        } else {
            try
            {
                auto cwd = process->cwd();
                auto dirInfo = detectDirectoryType(cwd);
                if (AND((dirInfo->hasPackageJson), (dirInfo->type != std::string("non-elizaos-dir")))) {
                    logger->info(std::string("No character files specified, attempting to load project agents..."));
                    auto project = std::async([=]() { loadProject(cwd); });
                    if (AND((project->agents), (project->agents->length > 0))) {
                        logger->info(std::string("Found ") + project->agents->length + std::string(" agent(s) in project configuration"));
                        projectAgents = project->agents;
                        for (auto& agent : project->agents)
                        {
                            if (agent->character) {
                                logger->info(std::string("Loaded character: ") + agent->character->name + string_empty);
                            }
                        }
                    }
                }
            }
            catch (const any& e)
            {
                logger->debug(std::string("Failed to load project agents, will use default character:"), e);
            }
        }
        std::async([=]() { startAgents(utils::assign(object{
            , 
            object::pair{std::string("characters"), std::string("characters")}, 
            object::pair{std::string("projectAgents"), std::string("projectAgents")}
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
