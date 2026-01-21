#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_TESTS_TAURI-INTEGRATION-RUNNER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_TESTS_TAURI-INTEGRATION-RUNNER_H
#include "core.h"
#include "child_process.h"
#include "fs.h"
using fs = promises;
#include "path.h"
using path = _default;

class TestResult;
class TauriIntegrationRunner;

class TestResult : public object, public std::enable_shared_from_this<TestResult> {
public:
    using std::enable_shared_from_this<TestResult>::shared_from_this;
    string name;

    boolean passed;

    string error;

    double duration;
};

class TauriIntegrationRunner : public object, public std::enable_shared_from_this<TauriIntegrationRunner> {
public:
    using std::enable_shared_from_this<TauriIntegrationRunner>::shared_from_this;
    array<std::shared_ptr<TestResult>> results = array<std::shared_ptr<TestResult>>();

    any serverProcess = nullptr;

    any tauriProcess = nullptr;

    virtual std::shared_ptr<Promise<void>> runAll();
    virtual std::shared_ptr<Promise<void>> buildTauriApp();
    virtual std::shared_ptr<Promise<void>> startElizaServer();
    virtual std::shared_ptr<Promise<void>> startTauriApp();
    virtual std::shared_ptr<Promise<void>> runIntegrationTests();
    virtual std::shared_ptr<Promise<void>> runTest(string name, std::function<std::shared_ptr<Promise<void>>()> testFn);
    virtual std::shared_ptr<Promise<void>> generateReport();
    virtual std::shared_ptr<Promise<void>> cleanup();
};

#endif
