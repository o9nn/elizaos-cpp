// plugins_automation_test.cpp - End-to-end tests for plugin automation.
#include <gtest/gtest.h>

#include "elizaos/plugins_automation.hpp"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>

using namespace elizaos;
namespace fs = std::filesystem;
using json = nlohmann::json;

namespace {

class TemporaryDirectory {
public:
    TemporaryDirectory()
        : path_(fs::temp_directory_path() /
                ("elizaos_plugins_automation_" + std::to_string(counter_.fetch_add(1)))) {
        std::error_code ec;
        fs::remove_all(path_, ec);
        fs::create_directories(path_);
    }

    ~TemporaryDirectory() {
        std::error_code ec;
        fs::remove_all(path_, ec);
    }

    const fs::path& path() const { return path_; }

private:
    fs::path path_;
    static std::atomic<unsigned long long> counter_;
};

std::atomic<unsigned long long> TemporaryDirectory::counter_{0};

std::string readFile(const fs::path& path) {
    std::ifstream stream(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

void writeFile(const fs::path& path, const std::string& content) {
    fs::create_directories(path.parent_path());
    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    ASSERT_TRUE(stream.good());
    stream << content;
    ASSERT_TRUE(stream.good());
}

class TestPlugin : public Plugin {
public:
    TestPlugin(std::string name, bool initializeResult = true, bool activateResult = true)
        : name_(std::move(name)), initializeResult_(initializeResult),
          activateResult_(activateResult) {}

    bool initialize(const PluginMetadata& metadata) override {
        ++initializeCalls;
        observedMetadata = metadata;
        status_ = initializeResult_ ? PluginStatus::LOADED : PluginStatus::FAILED;
        return initializeResult_;
    }
    bool activate() override {
        ++activateCalls;
        status_ = activateResult_ ? PluginStatus::ACTIVE : PluginStatus::FAILED;
        return activateResult_;
    }
    bool deactivate() override {
        ++deactivateCalls;
        status_ = PluginStatus::INACTIVE;
        return true;
    }
    bool shutdown() override {
        ++shutdownCalls;
        status_ = PluginStatus::UNKNOWN;
        return true;
    }
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return "1.0.0"; }
    PluginStatus getStatus() const override { return status_; }
    std::vector<std::string> getDependencies() const override { return dependencies_; }

    std::string name_;
    bool initializeResult_;
    bool activateResult_;
    PluginStatus status_{PluginStatus::UNKNOWN};
    std::vector<std::string> dependencies_;
    PluginMetadata observedMetadata;
    int initializeCalls{0};
    int activateCalls{0};
    int deactivateCalls{0};
    int shutdownCalls{0};
};

}  // namespace

TEST(PluginMetadata, ConvenienceConstructorPreservesFields) {
    PluginMetadata metadata("foo", "1.0", "description");
    EXPECT_EQ(metadata.name, "foo");
    EXPECT_EQ(metadata.version, "1.0");
    EXPECT_EQ(metadata.description, "description");
}

TEST(PluginRegistry, RegistrationInitializesPluginAndRejectsInvalidEntries) {
    PluginRegistry registry;
    auto plugin = std::make_shared<TestPlugin>("p1");
    PluginMetadata metadata("p1", "1.0", "test plugin");

    ASSERT_TRUE(registry.registerPlugin(plugin, metadata));
    EXPECT_EQ(plugin->initializeCalls, 1);
    EXPECT_EQ(plugin->observedMetadata.name, "p1");
    EXPECT_EQ(registry.getPlugin("p1"), plugin);
    EXPECT_EQ(registry.getPluginStatus("p1"), PluginStatus::LOADED);

    EXPECT_FALSE(registry.registerPlugin(plugin, metadata));
    EXPECT_FALSE(registry.registerPlugin(nullptr, metadata));
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>("bad"),
                                         PluginMetadata("", "1.0", "")));
    auto failing = std::make_shared<TestPlugin>("failing", false);
    EXPECT_FALSE(registry.registerPlugin(failing, PluginMetadata("failing", "1.0", "")));
    EXPECT_EQ(registry.getPlugin("failing"), nullptr);
}

TEST(PluginRegistry, DependencyResolutionActivatesInTopologicalOrder) {
    PluginRegistry registry;
    auto base = std::make_shared<TestPlugin>("base");
    auto child = std::make_shared<TestPlugin>("child");
    auto leaf = std::make_shared<TestPlugin>("leaf");

    ASSERT_TRUE(registry.registerPlugin(base, PluginMetadata("base", "1.0", "")));
    PluginMetadata childMetadata("child", "1.0", "");
    childMetadata.dependencies = {"base"};
    ASSERT_TRUE(registry.registerPlugin(child, childMetadata));
    PluginMetadata leafMetadata("leaf", "1.0", "");
    leafMetadata.dependencies = {"child"};
    ASSERT_TRUE(registry.registerPlugin(leaf, leafMetadata));

    EXPECT_EQ(registry.getDependencyChain("leaf"),
              (std::vector<std::string>{"base", "child"}));
    ASSERT_TRUE(registry.resolveDependencies("leaf"));
    EXPECT_EQ(base->getStatus(), PluginStatus::ACTIVE);
    EXPECT_EQ(child->getStatus(), PluginStatus::ACTIVE);
    EXPECT_EQ(leaf->getStatus(), PluginStatus::ACTIVE);
    EXPECT_EQ(registry.getActivePlugins(),
              (std::vector<std::string>{"base", "child", "leaf"}));

    EXPECT_FALSE(registry.unregisterPlugin("base"));
    EXPECT_TRUE(registry.unregisterPlugin("leaf"));
    EXPECT_TRUE(registry.unregisterPlugin("child"));
    EXPECT_TRUE(registry.unregisterPlugin("base"));
    EXPECT_EQ(base->deactivateCalls, 1);
    EXPECT_EQ(base->shutdownCalls, 1);
}

TEST(PluginRegistry, MissingDependencyAndActivationFailureAreReported) {
    PluginRegistry registry;
    PluginMetadata orphanMetadata("orphan", "1.0", "");
    orphanMetadata.dependencies = {"missing"};
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>("orphan"), orphanMetadata));

    auto failing = std::make_shared<TestPlugin>("failing", true, false);
    ASSERT_TRUE(registry.registerPlugin(failing, PluginMetadata("failing", "1.0", "")));
    EXPECT_FALSE(registry.resolveDependencies("failing"));
    EXPECT_EQ(registry.getFailedPlugins(), (std::vector<std::string>{"failing"}));
    EXPECT_FALSE(registry.resolveDependencies("unknown"));
}

TEST(CIPipeline, ExecutesBuildTestAndDeployCommandsAndRecordsStatuses) {
    TemporaryDirectory temp;
    const fs::path pluginPath = temp.path() / "plugin";
    const fs::path deployPath = temp.path() / "deploy";
    fs::create_directories(pluginPath);

    CIPipeline pipeline;
    pipeline.setBuildCommand("printf built > build.marker");
    pipeline.setTestCommand("test -f build.marker && printf tested");
    pipeline.setDeployCommand("mkdir -p {target} && cp -R {plugin} {target}/");

    ASSERT_TRUE(pipeline.buildPlugin(pluginPath.string()).get());
    EXPECT_EQ(readFile(pluginPath / "build.marker"), "built");
    auto status = pipeline.getStatus(pluginPath.string());
    EXPECT_EQ(status.stage, "build");
    EXPECT_FALSE(status.inProgress);
    EXPECT_TRUE(status.success);

    ASSERT_TRUE(pipeline.testPlugin(pluginPath.string()).get());
    status = pipeline.getStatus(pluginPath.string());
    EXPECT_EQ(status.stage, "test");
    EXPECT_TRUE(status.success);
    EXPECT_NE(status.output.find("tested"), std::string::npos);

    ASSERT_TRUE(pipeline.deployPlugin(pluginPath.string(), deployPath.string()).get());
    EXPECT_TRUE(fs::is_regular_file(deployPath / "plugin" / "build.marker"));
    status = pipeline.getStatus(pluginPath.string());
    EXPECT_EQ(status.stage, "deploy");
    EXPECT_TRUE(status.success);
    EXPECT_EQ(pipeline.getAllStatuses().size(), 1u);
}

TEST(CIPipeline, InvalidPathsAndFailingCommandsProduceTruthfulFailure) {
    TemporaryDirectory temp;
    CIPipeline pipeline;
    EXPECT_FALSE(pipeline.buildPlugin((temp.path() / "missing").string()).get());
    auto status = pipeline.getStatus((temp.path() / "missing").string());
    EXPECT_FALSE(status.success);
    EXPECT_NE(status.error.find("does not exist"), std::string::npos);

    pipeline.setBuildCommand("sh -c 'echo deliberate-failure; exit 7'");
    ASSERT_TRUE(fs::create_directories(temp.path() / "plugin"));
    EXPECT_FALSE(pipeline.buildPlugin((temp.path() / "plugin").string()).get());
    status = pipeline.getStatus((temp.path() / "plugin").string());
    EXPECT_FALSE(status.success);
    EXPECT_NE(status.error.find("status 7"), std::string::npos);
    EXPECT_NE(status.output.find("deliberate-failure"), std::string::npos);
    EXPECT_FALSE(pipeline.testPlugin("").get());
    EXPECT_FALSE(pipeline.deployPlugin("plugin", "").get());
}

TEST(PluginTester, RunsSelectedTestsAndCapturesFalseAndExceptions) {
    PluginTester tester;
    tester.addTestCase("alpha::passes", [] { return true; });
    tester.addTestCase("alpha::fails", [] { return false; });
    tester.addTestCase("beta::throws", []() -> bool { throw std::runtime_error("boom"); });

    const auto alpha = tester.runTests("alpha");
    ASSERT_EQ(alpha.size(), 2u);
    EXPECT_EQ(alpha[0].testName, "alpha::fails");
    EXPECT_FALSE(alpha[0].passed);
    EXPECT_EQ(alpha[1].testName, "alpha::passes");
    EXPECT_TRUE(alpha[1].passed);

    const auto all = tester.runAllTests();
    ASSERT_EQ(all.size(), 3u);
    EXPECT_FALSE(all[2].passed);
    EXPECT_NE(all[2].message.find("boom"), std::string::npos);

    tester.removeTestCase("alpha::fails");
    EXPECT_EQ(tester.runTests("alpha").size(), 1u);
}

TEST(PluginTester, EnforcesTimeoutWithoutBlockingTheCaller) {
    PluginTester tester;
    tester.setTimeout(1);
    tester.addTestCase("slow", [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(1250));
        return true;
    });

    const auto started = std::chrono::steady_clock::now();
    const auto results = tester.runAllTests();
    const auto elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - started).count();
    ASSERT_EQ(results.size(), 1u);
    EXPECT_FALSE(results.front().passed);
    EXPECT_NE(results.front().message.find("timed out"), std::string::npos);
    EXPECT_LT(elapsed, 1.2);
}

TEST(WorkflowAutomation, GeneratesValidBuiltinPluginDocsInterfacesTestsAndArchive) {
    TemporaryDirectory temp;
    WorkflowAutomation workflow;

    ASSERT_TRUE(workflow.createPluginTemplate("demo-plugin", temp.path().string()));
    const fs::path pluginPath = temp.path() / "demo-plugin";
    EXPECT_TRUE(workflow.validatePluginStructure(pluginPath.string()));
    ASSERT_TRUE(workflow.generatePluginDocs(pluginPath.string()));
    EXPECT_NE(readFile(pluginPath / "README.md").find("demo-plugin"), std::string::npos);

    ASSERT_TRUE(workflow.generateInterface("DemoService", (pluginPath / "include").string()));
    EXPECT_TRUE(fs::is_regular_file(pluginPath / "include" / "DemoService.hpp"));
    ASSERT_TRUE(workflow.generateTestSkeleton("demo-plugin", (pluginPath / "tests").string()));
    EXPECT_TRUE(fs::is_regular_file(pluginPath / "tests" / "demo-plugin_test.cpp"));

    const fs::path packageDirectory = temp.path() / "packages";
    ASSERT_TRUE(workflow.packagePlugin(pluginPath.string(), packageDirectory.string()));
    EXPECT_TRUE(fs::is_regular_file(packageDirectory / "demo-plugin.tar.gz"));
    EXPECT_GT(fs::file_size(packageDirectory / "demo-plugin.tar.gz"), 0u);

    EXPECT_FALSE(workflow.createPluginTemplate("demo-plugin", temp.path().string()));
    EXPECT_FALSE(workflow.validatePluginStructure((temp.path() / "missing").string()));
    EXPECT_FALSE(workflow.generateInterface("", temp.path().string()));
}

TEST(WorkflowAutomation, CopiesNamedTemplateAndReplacesContentTokens) {
    TemporaryDirectory temp;
    const fs::path templates = temp.path() / "templates";
    const fs::path basic = templates / "custom";
    writeFile(basic / "CMakeLists.txt", "project({{PLUGIN_IDENTIFIER}})\n");
    writeFile(basic / "src" / "plugin.cpp", "const char* name = \"{{PLUGIN_NAME}}\";\n");
    writeFile(basic / "include" / "plugin.hpp", "// ${PLUGIN_NAME}\n");

    WorkflowAutomation workflow;
    workflow.setTemplateDirectory(templates.string());
    EXPECT_EQ(workflow.getAvailableTemplates(), (std::vector<std::string>{"custom"}));
    ASSERT_TRUE(workflow.createPluginTemplate("my-plugin", "custom", temp.path().string()));
    const fs::path output = temp.path() / "my-plugin";
    EXPECT_TRUE(workflow.validatePluginStructure(output.string()));
    EXPECT_NE(readFile(output / "CMakeLists.txt").find("my_plugin"), std::string::npos);
    EXPECT_NE(readFile(output / "src" / "plugin.cpp").find("my-plugin"), std::string::npos);
    EXPECT_EQ(readFile(output / "include" / "plugin.hpp"), "// my-plugin\n");
    EXPECT_FALSE(workflow.createPluginTemplate("escape", "../custom", temp.path().string()));
}

TEST(PluginsAutomation, ConfigurationDrivesIntegratedSetupBuildTestAndDeployment) {
    TemporaryDirectory temp;
    const fs::path generated = temp.path() / "generated";
    const fs::path configuration = temp.path() / "automation.json";
    const fs::path saved = temp.path() / "saved.json";
    const fs::path deployed = temp.path() / "deployed";

    writeFile(configuration, json({
        {"output_root", generated.string()},
        {"build_command", "printf built > build.marker"},
        {"test_command", "test -f build.marker"},
        {"deploy_command", "mkdir -p {target} && cp -R {plugin} {target}/"}
    }).dump(2));

    PluginsAutomation automation;
    automation.loadConfiguration(configuration.string());
    automation.saveConfiguration(saved.string());
    EXPECT_EQ(json::parse(readFile(saved)), json::parse(readFile(configuration)));

    ASSERT_TRUE(automation.automatedPluginSetup("integrated-plugin", "missing-template-falls-back"));
    const fs::path pluginPath = generated / "integrated-plugin";
    EXPECT_TRUE(automation.getWorkflow().validatePluginStructure(pluginPath.string()));
    EXPECT_TRUE(automation.automatedBuildAndTest(pluginPath.string()));
    EXPECT_TRUE(fs::is_regular_file(pluginPath / "build.marker"));
    EXPECT_TRUE(automation.automatedDeployment(pluginPath.string(), deployed.string()));
    EXPECT_TRUE(fs::is_regular_file(deployed / "integrated-plugin" / "build.marker"));

    EXPECT_FALSE(automation.automatedBuildAndTest((temp.path() / "missing").string()));
    EXPECT_FALSE(automation.automatedDeployment("", deployed.string()));
}
