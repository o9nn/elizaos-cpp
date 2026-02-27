#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_ELIZAOS_SERVICES_SRC___TESTS___CLI_INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_ELIZAOS_SERVICES_SRC___TESTS___CLI_INTEGRATION_TEST_H
#include "core.hpp"
#include "bun:test.h"
#include "child_process.hpp"
#include "fs/promises.h"
#include "os.hpp"
#include "path.hpp"
// External dependency removed

class TestEnvironment;

class TestEnvironment : public object, public std::enable_shared_from_this<TestEnvironment> {
public:
    using std::enable_shared_from_this<TestEnvironment>::shared_from_this;
    string tempDir;

    string envFile;

    std::shared_ptr<NodeJS::ProcessEnv> originalEnv;
};

std::shared_ptr<Promise<std::shared_ptr<TestEnvironment>>> createTestEnvironment(boolean withTestKeys = true);

std::shared_ptr<Promise<void>> cleanupTestEnvironment(std::shared_ptr<TestEnvironment> env);

string execCLICommand(string command, std::shared_ptr<TestEnvironment> env);

#endif
