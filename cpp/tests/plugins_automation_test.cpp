// plugins_automation_test.cpp - Behavioral E2E tests for plugin automation.
#include <gtest/gtest.h>

#include "elizaos/plugins_automation.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

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

class ScopedTempDirectory {
public:
    explicit ScopedTempDirectory(const std::string& name) {
        path_ = fs::temp_directory_path() /
                (name + "_" + std::to_string(elizaos_getpid()) + "_" +
                 std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::error_code ec;
        fs::create_directories(path_, ec);
        if (ec) throw std::runtime_error("Unable to create temporary directory: " + ec.message());
    }

    ~ScopedTempDirectory() {
        std::error_code ec;
        fs::remove_all(path_, ec);
    }

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

class TestPlugin : public Plugin {
public:
    TestPlugin(std::string name, std::vector<std::string> dependencies = {})
        : name_(std::move(name)), dependencies_(std::move(dependencies)) {}

    bool initialize(const PluginMetadata&) override {
        status_ = PluginStatus::LOADED;
        return true;
    }
    bool activate() override {
        status_ = PluginStatus::ACTIVE;
        return true;
    }
    bool deactivate() override {
        status_ = PluginStatus::INACTIVE;
        return true;
    }
    bool shutdown() override {
        status_ = PluginStatus::UNKNOWN;
        return true;
    }
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return "1.0.0"; }
    PluginStatus getStatus() const override { return status_; }
    std::vector<std::string> getDependencies() const override { return dependencies_; }

private:
    std::string name_;
    std::vector<std::string> dependencies_;
    PluginStatus status_ = PluginStatus::UNKNOWN;
};

} // namespace

TEST(PluginMetadata, ConvenienceConstructor) {
    PluginMetadata metadata("foo", "1.0", "desc");
    EXPECT_EQ(metadata.name, "foo");
    EXPECT_EQ(metadata.version, "1.0");
    EXPECT_EQ(metadata.description, "desc");
}

TEST(PluginRegistry, LifecycleAndDependencyResolutionAreReal) {
    PluginRegistry registry;
    auto base = std::make_shared<TestPlugin>("base");
    PluginMetadata baseMetadata("base", "1.0", "base dependency");
    ASSERT_TRUE(registry.registerPlugin(base, baseMetadata));
    ASSERT_TRUE(base->initialize(baseMetadata));
    ASSERT_TRUE(base->activate());

    auto dependent = std::make_shared<TestPlugin>("dependent", std::vector<std::string>{"base"});
    PluginMetadata dependentMetadata("dependent", "1.0", "dependent plugin");
    dependentMetadata.dependencies = {"base"};
    ASSERT_TRUE(registry.registerPlugin(dependent, dependentMetadata));
    EXPECT_TRUE(registry.resolveDependencies("dependent"));
    EXPECT_EQ(registry.getDependencyChain("dependent"), std::vector<std::string>{"base"});
    EXPECT_EQ(registry.getActivePlugins(), std::vector<std::string>{"base"});

    EXPECT_FALSE(registry.registerPlugin(dependent, dependentMetadata));
    EXPECT_TRUE(registry.unregisterPlugin("dependent"));
    EXPECT_FALSE(registry.unregisterPlugin("dependent"));
}

TEST(PluginRegistry, MissingDependenciesAreRejected) {
    PluginRegistry registry;
    PluginMetadata metadata("dependent", "1.0", "");
    metadata.dependencies = {"missing"};
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>("dependent"), metadata));
    EXPECT_TRUE(registry.getPluginNames().empty());
}

TEST(CIPipeline, ExecutesCommandsAndCapturesStatus) {
    CIPipeline pipeline;
    pipeline.setBuildCommand("cmake -E echo build-ok");

    ASSERT_TRUE(pipeline.buildPlugin("sample-plugin").get());
    const auto status = pipeline.getStatus("sample-plugin");
    EXPECT_EQ(status.pluginName, "sample-plugin");
    EXPECT_EQ(status.stage, "build");
    EXPECT_FALSE(status.inProgress);
    EXPECT_TRUE(status.success);
    EXPECT_NE(status.output.find("build-ok"), std::string::npos);
    EXPECT_TRUE(status.error.empty());
    ASSERT_EQ(pipeline.getAllStatuses().size(), 1u);
}

TEST(CIPipeline, PropagatesCommandFailure) {
    CIPipeline pipeline;
    pipeline.setTestCommand("cmake -E false");

    EXPECT_FALSE(pipeline.testPlugin("broken-plugin").get());
    const auto status = pipeline.getStatus("broken-plugin");
    EXPECT_EQ(status.stage, "test");
    EXPECT_FALSE(status.inProgress);
    EXPECT_FALSE(status.success);
    EXPECT_FALSE(status.error.empty());
}

TEST(CIPipeline, ExpandsPluginAndTargetTokens) {
    CIPipeline pipeline;
    pipeline.setDeployCommand("cmake -E echo {plugin} {target}");

    ASSERT_TRUE(pipeline.deployPlugin("sample-plugin", "staging-area").get());
    const auto status = pipeline.getStatus("sample-plugin");
    EXPECT_EQ(status.stage, "deploy");
    EXPECT_NE(status.output.find("sample-plugin"), std::string::npos);
    EXPECT_NE(status.output.find("staging-area"), std::string::npos);
}

TEST(PluginTester, RunsSelectedTestsAndReportsExceptions) {
    PluginTester tester;
    tester.addTestCase("alpha::passes", [] { return true; });
    tester.addTestCase("alpha::fails", [] { return false; });
    tester.addTestCase("beta::throws", []() -> bool { throw std::runtime_error("boom"); });

    const auto alpha = tester.runTests("alpha");
    ASSERT_EQ(alpha.size(), 2u);
    EXPECT_EQ(alpha[0].testName, "alpha::fails");
    EXPECT_FALSE(alpha[0].passed);
    EXPECT_EQ(alpha[0].message, "returned false");
    EXPECT_EQ(alpha[1].testName, "alpha::passes");
    EXPECT_TRUE(alpha[1].passed);

    const auto all = tester.runAllTests();
    ASSERT_EQ(all.size(), 3u);
    const auto thrown = std::find_if(all.begin(), all.end(), [](const auto& result) {
        return result.testName == "beta::throws";
    });
    ASSERT_NE(thrown, all.end());
    EXPECT_FALSE(thrown->passed);
    EXPECT_NE(thrown->message.find("boom"), std::string::npos);

    tester.removeTestCase("alpha::fails");
    EXPECT_EQ(tester.runTests("alpha").size(), 1u);
}

TEST(PluginTester, EnforcesConfiguredTimeout) {
    PluginTester tester;
    tester.setTimeout(0);
    tester.addTestCase("slow::case", [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        return true;
    });

    const auto results = tester.runTests("slow");
    ASSERT_EQ(results.size(), 1u);
    EXPECT_FALSE(results.front().passed);
    EXPECT_NE(results.front().message.find("timed out"), std::string::npos);
}

TEST(WorkflowAutomation, GeneratesValidatesDocumentsAndPackagesPlugin) {
    ScopedTempDirectory temp("elizaos_plugin_workflow");
    WorkflowAutomation workflow;
    const fs::path plugin = temp.path() / "sample_plugin";

    ASSERT_TRUE(workflow.createPluginTemplate("sample_plugin", plugin.string()));
    EXPECT_TRUE(workflow.validatePluginStructure(plugin.string()));
    EXPECT_TRUE(fs::is_regular_file(plugin / "include/sample_plugin.hpp"));
    EXPECT_TRUE(fs::is_regular_file(plugin / "src/sample_plugin.cpp"));
    EXPECT_NE(readFile(plugin / "plugin.json").find("sample_plugin"), std::string::npos);

    ASSERT_TRUE(workflow.generatePluginDocs(plugin.string()));
    EXPECT_NE(readFile(plugin / "README.md").find("CMakeLists.txt"), std::string::npos);

    ASSERT_TRUE(workflow.generateInterface("StorageBridge", (plugin / "include").string()));
    ASSERT_TRUE(workflow.generateTestSkeleton("sample_plugin", (plugin / "tests").string()));
    EXPECT_TRUE(fs::is_regular_file(plugin / "include/StorageBridge.hpp"));
    EXPECT_TRUE(fs::is_regular_file(plugin / "tests/test_sample_plugin.cpp"));

    const fs::path packaged = temp.path() / "packaged_plugin";
    ASSERT_TRUE(workflow.packagePlugin(plugin.string(), packaged.string()));
    EXPECT_TRUE(fs::is_regular_file(packaged / "PACKAGE_MANIFEST.txt"));
    EXPECT_TRUE(workflow.validatePluginStructure(packaged.string()));
    EXPECT_FALSE(workflow.packagePlugin(plugin.string(), packaged.string()));
}

TEST(WorkflowAutomation, DiscoversAndCopiesConfiguredTemplates) {
    ScopedTempDirectory temp("elizaos_plugin_templates");
    const fs::path templates = temp.path() / "templates";
    const fs::path basic = templates / "basic";
    ASSERT_TRUE(fs::create_directories(basic));
    {
        std::ofstream metadata(basic / "plugin.json");
        metadata << "{\"name\":\"{{PLUGIN_NAME}}\",\"class\":\"{{PLUGIN_CLASS}}\"}\n";
    }

    WorkflowAutomation workflow;
    workflow.setTemplateDirectory(templates.string());
    EXPECT_EQ(workflow.getAvailableTemplates(), std::vector<std::string>{"basic"});

    workflow.setTemplateDirectory(basic.string());
    const fs::path destination = temp.path() / "from_template";
    ASSERT_TRUE(workflow.createPluginTemplate("my-plugin", destination.string()));
    const auto metadata = readFile(destination / "plugin.json");
    EXPECT_NE(metadata.find("my-plugin"), std::string::npos);
    EXPECT_NE(metadata.find("my_plugin"), std::string::npos);
    EXPECT_EQ(metadata.find("{{PLUGIN_NAME}}"), std::string::npos);
}

TEST(PluginsAutomation, LoadsConfigurationAndOrchestratesRealOperations) {
    ScopedTempDirectory temp("elizaos_plugins_automation");
    const fs::path configPath = temp.path() / "automation.conf";
    const fs::path outputRoot = temp.path() / "generated";
    {
        std::ofstream config(configPath);
        config << "plugin_output_root=" << outputRoot.string() << "\n";
        config << "template_directory=" << (temp.path() / "missing_templates").string() << "\n";
    }

    PluginsAutomation automation;
    automation.loadConfiguration(configPath.string());
    ASSERT_TRUE(automation.automatedPluginSetup("orchestrated", ""));
    const fs::path plugin = outputRoot / "orchestrated";
    EXPECT_TRUE(automation.getWorkflow().validatePluginStructure(plugin.string()));

    automation.getPipeline().setBuildCommand("cmake -E echo build");
    automation.getPipeline().setTestCommand("cmake -E echo test");
    EXPECT_TRUE(automation.automatedBuildAndTest(plugin.string()));

    automation.getPipeline().setDeployCommand("cmake -E echo {plugin} {target}");
    EXPECT_TRUE(automation.automatedDeployment("orchestrated", "staging"));
    EXPECT_FALSE(automation.automatedDeployment("", "staging"));

    const fs::path saved = temp.path() / "saved.conf";
    automation.saveConfiguration(saved.string());
    const auto content = readFile(saved);
    EXPECT_NE(content.find("plugin_output_root="), std::string::npos);
    EXPECT_NE(content.find("template_directory="), std::string::npos);
}
