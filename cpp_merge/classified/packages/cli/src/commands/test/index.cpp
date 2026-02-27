#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/test/index.h"

any registerCommand(std::shared_ptr<Command> cli)
{
    return cli->addCommand(test);
};


any test = ((std::make_shared<Command>()))->name(std::string("test"))->description(std::string("Run tests for the current project or a specified plugin"))->argument(std::string("[path]"), std::string("Optional path to the project or plugin to test"))->addOption(((std::make_shared<Option>(std::string("-t, --type <type>"), std::string("the type of test to run"))))->choices(array<string>{ std::string("component"), std::string("e2e"), std::string("all") })->default(std::string("all")))->option(std::string("--port <port>"), std::string("The port to run e2e tests on"), validatePort)->option(std::string("--name <name>"), std::string("Filter tests by name"))->option(std::string("--skip-build"), std::string("Skip building before running tests"))->option(std::string("--skip-type-check"), std::string("Skip TypeScript validation before running tests"))->hook(std::string("preAction"), [=](auto thisCommand) mutable
{
    auto testPath = const_(thisCommand["args"])[0];
    auto projectInfo = getProjectType(testPath);
    std::async([=]() { installPluginDependencies(projectInfo); });
}
)->action([=](auto testPath, auto options) mutable
{
    logger->info(std::string("Starting tests..."));
    try
    {
        auto projectInfo = getProjectType(testPath);
        static switch_type __switch1586_2518 = {
            { any(std::string("component")), 1 },
            { any(std::string("e2e")), 2 },
            { any(std::string("all")), 3 }
        };
        switch (__switch1586_2518[options->type])
        {
        case 1:
            logger->info(std::string("Running component tests only..."));
            auto componentResult = std::async([=]() { runComponentTests(testPath, options, projectInfo); });
            if (componentResult->failed) {
                logger->error(std::string("Component tests failed."));
                process->exit(1);
            }
            logger->success(std::string("Component tests passed successfully!"));
            break;
        case 2:
            logger->info(std::string("Running e2e tests only..."));
            auto e2eResult = std::async([=]() { runE2eTests(testPath, options, projectInfo); });
            if (e2eResult->failed) {
                logger->error(std::string("E2E tests failed."));
                process->exit(1);
            }
            logger->success(std::string("E2E tests passed successfully!"));
            break;
        case 3:
        default:
            logger->info(std::string("Running all tests..."));
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
