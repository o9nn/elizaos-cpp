#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_TESTING_TEST-HEALTH-MONITOR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_TESTING_TEST-HEALTH-MONITOR_H
#include "core.h"
#include "node:fs.h"
#include "node:path.h"
using path = _default;

class TestResult;
class TestRun;
class TestHealth;
class TestHealthMonitor;

class TestResult : public object, public std::enable_shared_from_this<TestResult> {
public:
    using std::enable_shared_from_this<TestResult>::shared_from_this;
    string name;

    double duration;

    any status;

    string error;
};

class TestRun : public object, public std::enable_shared_from_this<TestRun> {
public:
    using std::enable_shared_from_this<TestRun>::shared_from_this;
    std::shared_ptr<Date> date;

    double total;

    double passed;

    double failed;

    double skipped;

    double duration;

    array<std::shared_ptr<TestResult>> tests;
};

class TestHealth : public object, public std::enable_shared_from_this<TestHealth> {
public:
    using std::enable_shared_from_this<TestHealth>::shared_from_this;
    std::shared_ptr<Date> lastRun;

    double totalTests;

    double passedTests;

    double failedTests;

    array<string> flakyTests;

    double averageRuntime;

    array<object> slowestTests;

    array<std::shared_ptr<TestRun>> testHistory;
};

class TestHealthMonitor : public object, public std::enable_shared_from_this<TestHealthMonitor> {
public:
    using std::enable_shared_from_this<TestHealthMonitor>::shared_from_this;
    string healthDataPath;

    double maxHistorySize = 100;

    TestHealthMonitor(string dataDir = std::string(".test-health"));
    virtual void ensureDataDir();
    virtual void recordTestRun(std::shared_ptr<TestRun> results);
    virtual void updateFlakyTests(std::shared_ptr<TestHealth> health);
    virtual std::shared_ptr<TestHealth> getHealth();
    virtual void saveHealth(std::shared_ptr<TestHealth> health);
    virtual string generateReport();
    virtual object getTestTrends();
};

#endif
