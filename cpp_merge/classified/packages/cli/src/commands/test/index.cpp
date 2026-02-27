#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/test/index.h"

any registerCommand(std::shared_ptr<Command> cli)
{
    return cli->addCommand(test);
};


any test = ((std::make_shared<Command>()))->name(std:("test"))->description(std:("Run tests for the current project or a specified plugin"))->argument(std:("[path]"), std:("Optional path to the project or plugin to test"))->addOption(((std::make_shared<Option>(std:("-t, --type <type>"), std:("the type of test to run"))))->choices(array<string>{ std:("component"), std:("e2e"), std:("all") })->default(std:("all")))->option(std:("--port <port>"), std:("The port to run e2e tests on"), validatePort)->option(std:("--name <name>"), std:("Filter tests by name"))->option(std:("--skip-build"), std:("Skip building before running tests"))->option(std:("--skip-type-check"), std:("Skip TypeScript validation before running tests"))->hook(std:("preAction"), [=](auto thisCommand) mutable
{
    auto testPath = const_(thisCommand["args"])[0];
    auto projectInfo = getProjectType(testPath);
    std::async([=]() { installPluginDependencies(projectInfo); });
}
)->action([=](auto testPath, auto options) mutable
{
    logger->info(std:("Starting tests..."));
    try
    {
        auto projectInfo = getProjectType(testPath);
        static switch_type __switch1586_2518 = {
            { any(std:("component")), 1 },
            { any(std:("e2e")), 2 },
            { any(std:("all")), 3 }
        };
        switch (__switch1586_2518[options->type])
        {
        case 1:
            logger->info(std:("Running component tests only..."));
            auto componentResult = std::async([=]() { runComponentTests(testPath, options, projectInfo); });
            if (componentResult->failed) {
                logger->error(std:("Component tests failed."));
                process->exit(1);
            }
            logger->success(std:("Component tests passed successfully!"));
            break;
        case 2:
            logger->info(std:("Running e2e tests only..."));
            auto e2eResult = std::async([=]() { runE2eTests(testPath, options, projectInfo); });
            if (e2eResult->failed) {
                logger->error(std:("E2E tests failed."));
                process->exit(1);
            }
            logger->success(std:("E2E tests passed successfully!"));
            break;
        case 3:
        default:
            logger->info(std:("Running all tests..."));
            std::async([=]() { runAllTests(testPath, options); });
            break;
        }
        process->exit(0);
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
