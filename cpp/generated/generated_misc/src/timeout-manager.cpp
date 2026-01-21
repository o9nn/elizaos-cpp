#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/testing/timeout-manager.h"

std::shared_ptr<TestTimeoutManager> TestTimeoutManager::instance;

std::shared_ptr<TestTimeoutManager> TestTimeoutManager::getInstance()
{
    if (!TestTimeoutManager::instance) {
        TestTimeoutManager::instance = std::make_shared<TestTimeoutManager>();
    }
    return TestTimeoutManager::instance;
}

void TestTimeoutManager::startTimeout(string testName, double duration)
{
    this->clearTimeout(testName);
    this->testStartTimes->set(testName, Date->now());
    auto timeout = setTimeout([=]() mutable
    {
        auto elapsed = Date->now() - (OR((this->testStartTimes->get(testName)), (0)));
        logger->error(std::string("Test "") + testName + std::string("" exceeded timeout of ") + duration + std::string("ms (elapsed: ") + elapsed + std::string("ms)"));
        process->exit(1);
    }
    , duration);
    this->timeouts->set(testName, timeout);
}

void TestTimeoutManager::clearTimeout(string testName)
{
    auto timeout = this->timeouts->get(testName);
    if (timeout) {
        clearTimeout(timeout);
        this->timeouts->delete(testName);
    }
    this->testStartTimes->delete(testName);
}

void TestTimeoutManager::clearAll()
{
    this->timeouts->forEach([=](auto timeout) mutable
    {
        return clearTimeout(timeout);
    }
    );
    this->timeouts->clear();
    this->testStartTimes->clear();
}

std::shared_ptr<TestTimeoutManager> testTimeout = TestTimeoutManager::getInstance();

void Main(void)
{
}

MAIN
