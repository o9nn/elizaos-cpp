#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_SRC___TESTS___CLI-INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_SRC___TESTS___CLI-INTEGRATION_TEST_H
#include "core.h"
#include "bun:test.h"
#include "child_process.h"
#include "fs/promises.h"
#include "os.h"
#include "path.h"
#include "@elizaos/core.h"

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
