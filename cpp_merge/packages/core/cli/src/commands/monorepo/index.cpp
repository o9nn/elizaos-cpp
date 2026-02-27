#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/monorepo/index.h"

any monorepo = ((std::make_shared<Command>()))->name(std:("monorepo"))->description(std:("Clone ElizaOS monorepo from a specific branch, defaults to develop"))->option(std:("-b, --branch <branch>"), std:("Branch to install"), std:("develop"))->option(std:("-d, --dir <directory>"), std:("Destination directory"), std:("./eliza"))->action([=](auto options) mutable
{
    try
    {
        auto repo = std:("elizaOS/eliza");
        auto branch = OR((options->branch), (std:("develop")));
        auto dir = OR((options->dir), (std:("./eliza")));
        auto destinationDir = prepareDestination(dir);
        auto cloneInfo = object{
            object::pair{std:("repo"), std:("repo")}, 
            object::pair{std:("branch"), std:("branch")}, 
            object::pair{std:("destination"), dir}
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
