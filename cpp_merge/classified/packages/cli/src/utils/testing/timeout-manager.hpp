#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_TESTING_TIMEOUT_MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_TESTING_TIMEOUT_MANAGER_H
#include "core.hpp"
// External dependency removed

class TestTimeoutManager;

class TestTimeoutManager : public object, public std::enable_shared_from_this<TestTimeoutManager> {
public:
    using std::enable_shared_from_this<TestTimeoutManager>::shared_from_this;
    static std::shared_ptr<TestTimeoutManager> instance;

    std::shared_ptr<Map<string, std::shared_ptr<NodeJS::Timeout>>> timeouts = std::make_shared<Map>();

    std::shared_ptr<Map<string, double>> testStartTimes = std::make_shared<Map>();

    static std::shared_ptr<TestTimeoutManager> getInstance();
    virtual void startTimeout(string testName, double duration = 30000);
    virtual void clearTimeout(string testName);
    virtual void clearAll();
};

extern std::shared_ptr<TestTimeoutManager> testTimeout;
#endif
