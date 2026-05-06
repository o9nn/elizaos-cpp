#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/monorepo/index.h"

any monorepo = ((std::make_shared<Command>()))->name(std::string("monorepo"))->description(std::string("Clone ElizaOS monorepo from a specific branch, defaults to develop"))->option(std::string("-b, --branch <branch>"), std::string("Branch to install"), std::string("develop"))->option(std::string("-d, --dir <directory>"), std::string("Destination directory"), std::string("./eliza"))->action([=](auto options) mutable
{
    try
    {
        auto repo = std::string("elizaOS/eliza");
        auto branch = OR((options->branch), (std::string("develop")));
        auto dir = OR((options->dir), (std::string("./eliza")));
        auto destinationDir = prepareDestination(dir);
        auto cloneInfo = object{
            object::pair{std::string("repo"), std::string("repo")}, 
            object::pair{std::string("branch"), std::string("branch")}, 
            object::pair{std::string("destination"), dir}
        };
        std::async([=]() { cloneMonorepo(cloneInfo); });
        displayNextSteps(destinationDir);
    }
    catch (const any& error)
    {
        handleError(error);
    }
}
);

void Main(void)
{
}

MAIN
