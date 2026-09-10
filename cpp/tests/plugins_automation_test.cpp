// plugins_automation_test.cpp - deterministic lifecycle and owned executor tests.
#include <gtest/gtest.h>
#include "elizaos/plugins_automation.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <process.h>
#define elizaos_getpid _getpid
#else
#include <unistd.h>
#define elizaos_getpid getpid
#endif

using namespace elizaos;
namespace fs = std::filesystem;

namespace {

class TestPlugin : public Plugin {
public:
    TestPlugin(std::string name, std::vector<std::string> dependencies = {})
        : name_(std::move(name)), dependencies_(std::move(dependencies)) {}

    bool initialize(const PluginMetadata&) override {
        ++initializeCalls;
        if (throwInitialize) throw std::runtime_error("initialize boom");
        if (!initializeResult) return false;
        status_.store(PluginStatus::LOADED);
        return true;
    }
    bool activate() override {
        ++activateCalls;
        if (throwActivate) throw std::runtime_error("activate boom");
        if (!activateResult) return false;
        status_.store(PluginStatus::ACTIVE);
        return true;
    }
    bool deactivate() override {
        ++deactivateCalls;
        if (throwDeactivate) throw std::runtime_error("deactivate boom");
        if (!deactivateResult) return false;
        status_.store(PluginStatus::INACTIVE);
        return true;
    }
    bool shutdown() override {
        ++shutdownCalls;
        if (throwShutdown) throw std::runtime_error("shutdown boom");
        if (!shutdownResult) return false;
        status_.store(PluginStatus::UNKNOWN);
        return true;
    }
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version; }
    PluginStatus getStatus() const override { return status_.load(); }
    std::vector<std::string> getDependencies() const override { return dependencies_; }
    bool healthCheck() const override { return healthy && Plugin::healthCheck(); }

    std::string version = "1.0.0";
    bool initializeResult = true;
    bool activateResult = true;
    bool deactivateResult = true;
    bool shutdownResult = true;
    bool healthy = true;
    bool throwInitialize = false;
    bool throwActivate = false;
    bool throwDeactivate = false;
    bool throwShutdown = false;
    std::atomic<int> initializeCalls{0};
    std::atomic<int> activateCalls{0};
    std::atomic<int> deactivateCalls{0};
    std::atomic<int> shutdownCalls{0};

private:
    std::string name_;
    std::vector<std::string> dependencies_;
    std::atomic<PluginStatus> status_{PluginStatus::UNKNOWN};
};

PluginMetadata metadata(const std::string& name,
                        std::vector<std::string> dependencies = {}) {
    PluginMetadata value(name, "1.0.0", "test plugin");
    value.apiVersion = "1.0";
    value.dependencies = std::move(dependencies);
    return value;
}

class ScopedTempDirectory {
public:
    explicit ScopedTempDirectory(const std::string& name) {
        path_ = fs::temp_directory_path() /
            (name + "_" + std::to_string(elizaos_getpid()) + "_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        fs::create_directories(path_);
    }
    ~ScopedTempDirectory() { std::error_code error; fs::remove_all(path_, error); }
    const fs::path& path() const { return path_; }
private:
    fs::path path_;
};

std::string readFile(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    std::ostringstream content;
    content << input.rdbuf();
    return content.str();
}

} // namespace

TEST(PluginMetadata, ConvenienceConstructorAndApiVersion) {
    PluginMetadata value("foo", "1.0.0", "desc");
    EXPECT_EQ(value.name, "foo");
    EXPECT_EQ(value.version, "1.0.0");
    EXPECT_EQ(value.apiVersion, "1.0");
}

TEST(PluginRegistry, ValidatedLifecycleAndHealthAgree) {
    PluginRegistry registry;
    auto plugin = std::make_shared<TestPlugin>("plugin");
    ASSERT_TRUE(registry.registerPlugin(plugin, metadata("plugin")));
    EXPECT_EQ(registry.getPluginStatus("plugin"), PluginStatus::LOADED);
    EXPECT_TRUE(registry.checkPluginHealth("plugin"));
    EXPECT_FALSE(registry.activatePlugin("missing"));
    ASSERT_TRUE(registry.activatePlugin("plugin"));
    EXPECT_EQ(registry.getPluginStatus("plugin"), PluginStatus::ACTIVE);
    EXPECT_EQ(registry.getActivePlugins(), std::vector<std::string>{"plugin"});
    EXPECT_TRUE(registry.checkPluginHealth("plugin"));
    ASSERT_TRUE(registry.deactivatePlugin("plugin"));
    EXPECT_EQ(registry.getPluginStatus("plugin"), PluginStatus::INACTIVE);
    EXPECT_TRUE(registry.unregisterPlugin("plugin"));
    EXPECT_EQ(registry.getPluginStatus("plugin"), PluginStatus::UNKNOWN);
}

TEST(PluginRegistry, RejectsMetadataIdentityApiAndDependencyDefects) {
    PluginRegistry registry;
    EXPECT_FALSE(registry.registerPlugin(nullptr, metadata("null")));
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>("actual"), metadata("other")));
    auto invalidApi = metadata("api"); invalidApi.apiVersion = "2.0";
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>("api"), invalidApi));
    auto invalidVersion = metadata("version"); invalidVersion.version = "latest";
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>("version"), invalidVersion));
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>("dependent", std::vector<std::string>{"missing"}),
                                         metadata("dependent", {"missing"})));
}

TEST(PluginRegistry, DuplicateIdsAndDependentsAreProtected) {
    PluginRegistry registry;
    auto base = std::make_shared<TestPlugin>("base");
    ASSERT_TRUE(registry.registerPlugin(base, metadata("base")));
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>("base"), metadata("base")));
    auto dependent = std::make_shared<TestPlugin>("dependent", std::vector<std::string>{"base"});
    ASSERT_TRUE(registry.registerPlugin(dependent, metadata("dependent", {"base"})));
    EXPECT_FALSE(registry.unregisterPlugin("base"));
    EXPECT_FALSE(registry.activatePlugin("dependent"));
    ASSERT_TRUE(registry.activatePlugin("base"));
    EXPECT_TRUE(registry.resolveDependencies("dependent"));
    EXPECT_TRUE(registry.activatePlugin("dependent"));
}

TEST(PluginRegistry, HookFailuresRollbackRegistryState) {
    PluginRegistry registry;
    auto initFailure = std::make_shared<TestPlugin>("init-failure");
    initFailure->initializeResult = false;
    EXPECT_FALSE(registry.registerPlugin(initFailure, metadata("init-failure")));
    EXPECT_EQ(registry.getPlugin("init-failure"), nullptr);

    auto plugin = std::make_shared<TestPlugin>("rollback");
    ASSERT_TRUE(registry.registerPlugin(plugin, metadata("rollback")));
    plugin->activateResult = false;
    EXPECT_FALSE(registry.activatePlugin("rollback"));
    EXPECT_EQ(registry.getPluginStatus("rollback"), PluginStatus::LOADED);
    plugin->activateResult = true;
    ASSERT_TRUE(registry.activatePlugin("rollback"));
    plugin->deactivateResult = false;
    EXPECT_FALSE(registry.deactivatePlugin("rollback"));
    EXPECT_EQ(registry.getPluginStatus("rollback"), PluginStatus::ACTIVE);
    plugin->deactivateResult = true;
    plugin->shutdownResult = false;
    EXPECT_FALSE(registry.unregisterPlugin("rollback"));
    EXPECT_EQ(registry.getPluginStatus("rollback"), PluginStatus::ACTIVE);
    EXPECT_NE(registry.getPlugin("rollback"), nullptr);
}

TEST(PluginRegistry, ThrowingHooksRollbackToTruthfulStates) {
    PluginRegistry registry;
    auto plugin = std::make_shared<TestPlugin>("throwing-hooks");
    ASSERT_TRUE(registry.registerPlugin(plugin, metadata("throwing-hooks")));
    plugin->throwActivate = true;
    EXPECT_FALSE(registry.activatePlugin("throwing-hooks"));
    EXPECT_EQ(registry.getPluginStatus("throwing-hooks"), PluginStatus::LOADED);
    plugin->throwActivate = false;
    ASSERT_TRUE(registry.activatePlugin("throwing-hooks"));
    plugin->throwDeactivate = true;
    EXPECT_FALSE(registry.deactivatePlugin("throwing-hooks"));
    EXPECT_EQ(registry.getPluginStatus("throwing-hooks"), PluginStatus::ACTIVE);
    plugin->throwDeactivate = false;
    plugin->throwShutdown = true;
    EXPECT_FALSE(registry.unregisterPlugin("throwing-hooks"));
    EXPECT_EQ(registry.getPluginStatus("throwing-hooks"), PluginStatus::ACTIVE);
}

TEST(PluginRegistry, ConcurrentRegistryAccessIsSynchronized) {
    PluginRegistry registry;
    auto plugin = std::make_shared<TestPlugin>("concurrent");
    ASSERT_TRUE(registry.registerPlugin(plugin, metadata("concurrent")));
    std::atomic<bool> stop{false};
    std::vector<std::thread> readers;
    for (int i = 0; i < 4; ++i) readers.emplace_back([&] {
        while (!stop.load()) {
            (void)registry.getPluginNames();
            (void)registry.getPluginStatus("concurrent");
            (void)registry.getActivePlugins();
            (void)registry.checkPluginHealth("concurrent");
        }
    });
    for (int i = 0; i < 100; ++i) {
        ASSERT_TRUE(registry.activatePlugin("concurrent"));
        ASSERT_TRUE(registry.deactivatePlugin("concurrent"));
    }
    stop.store(true);
    for (auto& reader : readers) reader.join();
    EXPECT_EQ(registry.getPluginStatus("concurrent"), PluginStatus::INACTIVE);
}

TEST(PluginTester, RepeatedExecutionAndExceptionPropagation) {
    PluginTester tester;
    std::atomic<int> executions{0};
    tester.addTestCase("alpha::passes", [&] { ++executions; return true; });
    tester.addTestCase("alpha::throws", []() -> bool { throw std::runtime_error("boom"); });
    for (int i = 0; i < 20; ++i) {
        const auto results = tester.runTests("alpha");
        ASSERT_EQ(results.size(), 2u);
        EXPECT_TRUE(results[0].passed);
        EXPECT_FALSE(results[1].passed);
        EXPECT_NE(results[1].message.find("boom"), std::string::npos);
    }
    EXPECT_EQ(executions.load(), 20);
}

TEST(PluginTester, CooperativeTimeoutReturnsPromptlyAndDestructorJoins) {
    const auto started = std::chrono::steady_clock::now();
    std::atomic<bool> observedStop{false};
    {
        PluginTester tester;
        tester.setTimeout(0);
        tester.addTestCase("slow::cooperative", [&](const PluginTester::StopToken& token) {
            while (!token.stopRequested()) std::this_thread::sleep_for(std::chrono::milliseconds(1));
            observedStop.store(true);
            return false;
        });
        const auto result = tester.runTests("slow").front();
        EXPECT_TRUE(result.timedOut);
        EXPECT_FALSE(result.passed);
    }
    EXPECT_TRUE(observedStop.load());
    EXPECT_LT(std::chrono::steady_clock::now() - started, std::chrono::seconds(1));
}

TEST(PluginTester, NonCooperativeTimeoutOwnsWorkUntilTeardown) {
    std::atomic<bool> completed{false};
    const auto started = std::chrono::steady_clock::now();
    {
        PluginTester tester;
        tester.setTimeout(0);
        tester.addTestCase("slow::legacy", [&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            completed.store(true);
            return true;
        });
        const auto result = tester.runTests("slow").front();
        EXPECT_TRUE(result.timedOut);
        EXPECT_FALSE(completed.load());
    }
    EXPECT_TRUE(completed.load());
    EXPECT_GE(std::chrono::steady_clock::now() - started, std::chrono::milliseconds(35));
}

TEST(PluginTester, BoundedExecutorRejectsAdmissionWhileTimedOutWorkRuns) {
    PluginTester tester;
    tester.setTimeout(0);
    tester.setMaxPendingTests(1);
    tester.addTestCase("suite::a-slow", [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        return true;
    });
    tester.addTestCase("suite::b-next", [] { return true; });
    const auto results = tester.runTests("suite");
    ASSERT_EQ(results.size(), 2u);
    EXPECT_TRUE(results[0].timedOut);
    EXPECT_TRUE(results[1].admissionRejected);
}

TEST(CIPipeline, ExecutesAndReportsRealCommandResults) {
    CIPipeline pipeline;
    pipeline.setBuildCommand("cmake -E echo build-ok");
    ASSERT_TRUE(pipeline.buildPlugin("sample-plugin").get());
    EXPECT_TRUE(pipeline.getStatus("sample-plugin").success);
    pipeline.setTestCommand("cmake -E false");
    EXPECT_FALSE(pipeline.testPlugin("broken-plugin").get());
    EXPECT_FALSE(pipeline.getStatus("broken-plugin").error.empty());
}

TEST(WorkflowAutomation, GeneratesValidatesAndPackagesPlugin) {
    ScopedTempDirectory temp("elizaos_plugin_workflow");
    WorkflowAutomation workflow;
    const fs::path plugin = temp.path() / "sample_plugin";
    ASSERT_TRUE(workflow.createPluginTemplate("sample_plugin", plugin.string()));
    EXPECT_TRUE(workflow.validatePluginStructure(plugin.string()));
    ASSERT_TRUE(workflow.generatePluginDocs(plugin.string()));
    EXPECT_NE(readFile(plugin / "README.md").find("CMakeLists.txt"), std::string::npos);
    const fs::path packaged = temp.path() / "packaged";
    EXPECT_TRUE(workflow.packagePlugin(plugin.string(), packaged.string()));
    EXPECT_TRUE(fs::is_regular_file(packaged / "PACKAGE_MANIFEST.txt"));
}
