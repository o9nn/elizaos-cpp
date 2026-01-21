#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/test/actions/run-all-tests.h"

std::shared_ptr<Promise<void>> runAllTests(any testPath, std::shared_ptr<TestCommandOptions> options)
{
    auto projectInfo = getProjectType(testPath);
    if (!options->skipBuild) {
        auto componentResult = std::async([=]() { runComponentTests(testPath, options, projectInfo); });
        if (componentResult->failed) {
            logger->error(std::string("Component tests failed. Continuing to e2e tests..."));
        }
    }
    auto e2eResult = std::async([=]() { runE2eTests(testPath, options, projectInfo); });
    if (e2eResult->failed) {
        logger->error(std::string("E2E tests failed."));
        process->exit(1);
    }
    logger->success(std::string("All tests passed successfully!"));
    process->exit(0);
    return std::shared_ptr<Promise<void>>();
};


