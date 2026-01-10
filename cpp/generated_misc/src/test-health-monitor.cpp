#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/testing/test-health-monitor.h"

TestHealthMonitor::TestHealthMonitor(string dataDir) {
    this->healthDataPath = path->join(dataDir, std::string("health.json"));
    this->ensureDataDir();
}

void TestHealthMonitor::ensureDataDir()
{
    auto dir = path->dirname(this->healthDataPath);
    if (!existsSync(dir)) {
        mkdirSync(dir, object{
            object::pair{std::string("recursive"), true}
        });
    }
}

void TestHealthMonitor::recordTestRun(std::shared_ptr<TestRun> results)
{
    auto health = this->getHealth();
    health->lastRun = std::make_shared<Date>();
    health->totalTests = results->total;
    health->passedTests = results->passed;
    health->failedTests = results->failed;
    health->averageRuntime = results->duration;
    health->slowestTests = results->tests->filter([=](auto t) mutable
    {
        return t->duration > 1000;
    }
    )->sort([=](auto a, auto b) mutable
    {
        return b->duration - a->duration;
    }
    )->slice(0, 10)->map([=](auto t) mutable
    {
        return (object{
            object::pair{std::string("name"), t->name}, 
            object::pair{std::string("duration"), t->duration}
        });
    }
    );
    health->testHistory->push(results);
    if (health->testHistory->get_length() > this->maxHistorySize) {
        health->testHistory = health->testHistory->slice(-this->maxHistorySize);
    }
    this->updateFlakyTests(health);
    this->saveHealth(health);
}

void TestHealthMonitor::updateFlakyTests(std::shared_ptr<TestHealth> health)
{
    auto testResults = std::make_shared<Map<string, array<boolean>>>();
    auto recentRuns = health->testHistory->slice(-10);
    for (auto& run : recentRuns)
    {
        for (auto& test : run->tests)
        {
            if (!testResults->has(test->name)) {
                testResults->set(test->name, array<any>());
            }
            testResults->get(test->name)->push(test->status == std::string("passed"));
        }
    }
    auto flakyTests = array<string>();
    auto& __array2425_2862 = testResults;
    for (auto __indx2425_2862 = 0_N; __indx2425_2862 < __array2425_2862->get_length(); __indx2425_2862++)
    {
        auto& [testName, results] = const_(__array2425_2862)[__indx2425_2862];
        {
            if (results["length"] >= 3) {
                auto hasPassed = results["some"]([=](auto r) mutable
                {
                    return r;
                }
                );
                auto hasFailed = results["some"]([=](auto r) mutable
                {
                    return !r;
                }
                );
                if (AND((hasPassed), (hasFailed))) {
                    auto failureRate = results["filter"]([=](auto r) mutable
                    {
                        return !r;
                    }
                    )["length"] / results["length"];
                    if (AND((failureRate > 0.1), (failureRate < 0.9))) {
                        flakyTests->push(testName);
                    }
                }
            }
        }
    }
    health->flakyTests = flakyTests;
}

std::shared_ptr<TestHealth> TestHealthMonitor::getHealth()
{
    if (existsSync(this->healthDataPath)) {
        auto data = JSON->parse(readFileSync(this->healthDataPath, std::string("utf-8")));
        data["lastRun"] = std::make_shared<Date>(data["lastRun"]);
        data["testHistory"] = OR((data["testHistory"]), (array<any>()));
        data["testHistory"]["forEach"]([=](auto run) mutable
        {
            run["date"] = std::make_shared<Date>(run["date"]);
        }
        );
        return data;
    }
    return object{
        object::pair{std::string("lastRun"), std::make_shared<Date>()}, 
        object::pair{std::string("totalTests"), 0}, 
        object::pair{std::string("passedTests"), 0}, 
        object::pair{std::string("failedTests"), 0}, 
        object::pair{std::string("flakyTests"), array<any>()}, 
        object::pair{std::string("averageRuntime"), 0}, 
        object::pair{std::string("slowestTests"), array<any>()}, 
        object::pair{std::string("testHistory"), array<any>()}
    };
}

void TestHealthMonitor::saveHealth(std::shared_ptr<TestHealth> health)
{
    writeFileSync(this->healthDataPath, JSON->stringify(health, nullptr, 2));
}

string TestHealthMonitor::generateReport()
{
    auto health = this->getHealth();
    auto recentRuns = health->testHistory->slice(-5);
    auto avgRecentDuration = (recentRuns->get_length() > 0) ? any(recentRuns->reduce([=](auto sum, auto run) mutable
    {
        return sum + run->duration;
    }
    , 0) / recentRuns->get_length()) : any(0);
    auto successRate = (health->totalTests > 0) ? any(((health->passedTests / health->totalTests) * 100)->toFixed(2)) : any(std::string("0"));
    return std::string("\
# Test Health Report\
\
Last Run: ") + health->lastRun->toISOString() + std::string("\
Total Tests: ") + health->totalTests + std::string("\
Passed: ") + health->passedTests + std::string("\
Failed: ") + health->failedTests + std::string("\
Success Rate: ") + successRate + std::string("%\
Average Runtime: ") + health->averageRuntime + std::string("ms\
Recent Average: ") + avgRecentDuration->toFixed(0) + std::string("ms\
\
## Slowest Tests\
") + health->slowestTests->map([=](auto t) mutable
    {
        return std::string("- ") + t["name"] + std::string(": ") + t["duration"] + std::string("ms");
    }
    )->join(std::string("\
")) + std::string("\
\
## Flaky Tests (") + health->flakyTests->get_length() + std::string(")\
") + (health->flakyTests->get_length() > 0) ? any(health->flakyTests->map([=](auto t) mutable
    {
        return std::string("- ") + t + string_empty;
    }
    )->join(std::string("\
"))) : any(std::string("None detected")) + std::string("\
\
## Test History (Last 5 Runs)\
") + recentRuns->map([=](auto run) mutable
    {
        return std::string("- ") + run->date->toISOString() + std::string(": ") + run->passed + std::string("/") + run->total + std::string(" passed (") + run->duration + std::string("ms)");
    }
    )->join(std::string("\
")) + std::string("\
    ")->trim();
}

object TestHealthMonitor::getTestTrends()
{
    auto health = this->getHealth();
    auto history = health->testHistory->slice(-30);
    return object{
        object::pair{std::string("successRateHistory"), history->map([=](auto run) mutable
        {
            return (run->total > 0) ? any((run->passed / run->total) * 100) : any(0);
        }
        )}, 
        object::pair{std::string("durationHistory"), history->map([=](auto run) mutable
        {
            return run->duration;
        }
        )}, 
        object::pair{std::string("dates"), history->map([=](auto run) mutable
        {
            return run->date;
        }
        )}
    };
}

