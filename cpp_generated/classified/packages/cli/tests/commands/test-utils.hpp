#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_TESTS_COMMANDS_TEST-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_TESTS_COMMANDS_TEST-UTILS_H
#include "core.h"
#include "fs/promises.h"
#include "path.h"
#include "os.h"
#include "fs.h"
#include "../test-timeouts.h"
#include "../../src/utils/bun-exec.h"
#include "../utils/bun-test-helpers.h"

class TestContext;
class TestProcessManager;

std::shared_ptr<Promise<string>> execShellCommand(string command, object options = object{});

class TestContext : public object, public std::enable_shared_from_this<TestContext> {
public:
    using std::enable_shared_from_this<TestContext>::shared_from_this;
    string testTmpDir;

    string originalCwd;
};

std::shared_ptr<Promise<std::shared_ptr<TestContext>>> setupTestEnvironment();

std::shared_ptr<Promise<void>> cleanupTestEnvironment(std::shared_ptr<TestContext> context);

void safeChangeDirectory(string targetDir);

std::shared_ptr<Promise<void>> createTestProject(string projectName);

void expectHelpOutput(string output, string command, array<string> expectedOptions = array<string>());

std::shared_ptr<Promise<void>> createTestPluginStructure(string pluginName);

std::shared_ptr<Promise<void>> createTestAgent(string agentName);

extern object assertions;
std::shared_ptr<Promise<void>> waitForServerReady(double port, double maxWaitTime = TEST_TIMEOUTS["SERVER_STARTUP"], string endpoint = std::string("/api/agents"));

std::shared_ptr<Promise<void>> killProcessOnPort(double port);

extern object crossPlatform;
any getPlatformOptions(any baseOptions = object{});

class TestProcessManager : public object, public std::enable_shared_from_this<TestProcessManager> {
public:
    using std::enable_shared_from_this<TestProcessManager>::shared_from_this;
    std::shared_ptr<Set<any>> processes = std::make_shared<Set>();

    virtual any spawn(string command, array<string> args, any options = object{});
    virtual std::shared_ptr<Promise<void>> terminateProcess(any process);
    virtual std::shared_ptr<Promise<void>> cleanup();
    virtual double getActiveCount();
};

#endif
