// plugin_specification_test.cpp - lifecycle, validation, concurrency, and public-link tests.
#include <gtest/gtest.h>
#include "elizaos/plugin_specification.hpp"

#include <atomic>
#include <chrono>
#include <future>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace elizaos;

namespace {

PluginMetadata metadata(const std::string& name) {
    PluginMetadata value;
    value.name = name;
    value.displayName = name;
    value.author = "test";
    value.version = PluginVersion::fromString("1.2.3");
    value.apiVersion = PluginVersion::fromString("1.0.0");
    return value;
}

class TestPlugin final : public PluginInterface {
public:
    explicit TestPlugin(PluginMetadata value) : metadata_(std::move(value)) {}

    PluginMetadata getMetadata() const override {
        ++metadataCalls;
        if (onMetadata) onMetadata();
        return metadata_;
    }
    bool initialize(const std::unordered_map<std::string, std::any>&) override {
        ++initializeCalls;
        if (throwInitialize) throw std::runtime_error("initialize boom");
        initialized_.store(initializeResult);
        return initializeResult;
    }
    void shutdown() override {
        ++shutdownCalls;
        if (throwShutdown) throw std::runtime_error("shutdown boom");
        initialized_.store(false);
    }
    PluginResult execute(const PluginContext&) override {
        if (throwExecute) throw std::runtime_error("execute boom");
        PluginResult result; result.success = executeResult;
        result.message = executeResult ? "ok" : "failed"; return result;
    }
    bool validateConfiguration(const std::unordered_map<std::string, std::any>&) const override {
        return configurationValid;
    }
    bool healthCheck() const override {
        ++healthCalls;
        return healthy && PluginInterface::healthCheck();
    }

    PluginMetadata metadata_;
    mutable std::atomic<int> metadataCalls{0};
    mutable std::atomic<int> healthCalls{0};
    std::atomic<int> initializeCalls{0};
    std::atomic<int> shutdownCalls{0};
    bool initializeResult = true;
    bool executeResult = true;
    bool configurationValid = true;
    bool healthy = true;
    bool throwInitialize = false;
    bool throwShutdown = false;
    bool throwExecute = false;
    std::function<void()> onMetadata;
};

} // namespace

TEST(PluginVersion, StrictParsingAndRanges) {
    auto version = PluginVersion::fromString("2.3.4-beta+build7");
    EXPECT_EQ(version.toString(), "2.3.4-beta+build7");
    EXPECT_EQ(PluginVersion::fromString("1.2").toString(), "0.0.0");
    EXPECT_EQ(PluginVersion::fromString("bad").toString(), "0.0.0");

    PluginDependency dependency{"core", PluginVersion::fromString("1.2.0"),
                                PluginVersion::fromString("1.4.0"), true};
    EXPECT_TRUE(dependency.isSatisfiedBy(PluginVersion::fromString("1.3.9")));
    EXPECT_FALSE(dependency.isSatisfiedBy(PluginVersion::fromString("1.5.0")));
    EXPECT_FALSE(dependency.isSatisfiedBy(PluginVersion::fromString("2.0.0")));
}

TEST(PluginMetadata, EnforcesIdentityApiAndDependencyValidation) {
    auto valid = metadata("valid-plugin");
    EXPECT_TRUE(valid.validate());

    auto invalidApi = valid;
    invalidApi.apiVersion = PluginVersion::fromString("2.0.0");
    EXPECT_FALSE(invalidApi.validate());

    auto selfDependency = valid;
    selfDependency.dependencies.push_back(
        {valid.name, PluginVersion::fromString("1.0.0"), PluginVersion{0, 0, 0}, true});
    EXPECT_FALSE(selfDependency.validate());

    auto invalidRange = valid;
    invalidRange.dependencies.push_back(
        {"core", PluginVersion::fromString("2.0.0"), PluginVersion::fromString("1.0.0"), true});
    EXPECT_FALSE(invalidRange.validate());
}

TEST(PluginMetadata, JsonPreservesApiVersion) {
    auto original = metadata("json-plugin");
    original.apiVersion = PluginVersion::fromString("1.7.0");
    const auto restored = PluginMetadata::fromJson(original.toJson());
    EXPECT_EQ(restored.apiVersion.toString(), "1.7.0");
}

TEST(PluginRegistry, RejectsDuplicateIdsAndUnsatisfiedDependencies) {
    PluginRegistry registry;
    auto base = std::make_shared<TestPlugin>(metadata("base"));
    ASSERT_TRUE(registry.registerPlugin(base));
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>(metadata("base"))));

    auto dependentMetadata = metadata("dependent");
    dependentMetadata.dependencies.push_back(
        {"missing", PluginVersion::fromString("1.0.0"), {}, true});
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>(dependentMetadata)));

    dependentMetadata.dependencies[0].pluginName = "base";
    dependentMetadata.dependencies[0].minVersion = PluginVersion::fromString("9.0.0");
    EXPECT_FALSE(registry.registerPlugin(std::make_shared<TestPlugin>(dependentMetadata)));
}

TEST(PluginRegistry, DependencyOrderIsDeterministicAndProtectsProviders) {
    PluginRegistry registry;
    auto base = std::make_shared<TestPlugin>(metadata("base"));
    ASSERT_TRUE(registry.registerPlugin(base));
    auto childMetadata = metadata("child");
    childMetadata.dependencies.push_back(
        {"base", PluginVersion::fromString("1.0.0"), PluginVersion{0, 0, 0}, true});
    ASSERT_TRUE(registry.registerPlugin(std::make_shared<TestPlugin>(childMetadata)));
    EXPECT_EQ(registry.getDependencyOrder(), (std::vector<std::string>{"base", "child"}));
    EXPECT_FALSE(registry.unregisterPlugin("base"));
    EXPECT_TRUE(registry.unregisterPlugin("child"));
    EXPECT_TRUE(registry.unregisterPlugin("base"));
}

TEST(PluginRegistry, DoesNotInvokeMetadataOrShutdownUnderRegistryLock) {
    PluginRegistry registry;
    auto plugin = std::make_shared<TestPlugin>(metadata("reentrant"));
    plugin->onMetadata = [&] { (void)registry.getAllPlugins(); };
    auto registration = std::async(std::launch::async, [&] { return registry.registerPlugin(plugin); });
    ASSERT_EQ(registration.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    ASSERT_TRUE(registration.get());

    plugin->onMetadata = {};
    auto unregister = std::async(std::launch::async, [&] { return registry.unregisterPlugin("reentrant"); });
    ASSERT_EQ(unregister.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_TRUE(unregister.get());
}

TEST(PluginManager, SuccessfulInitializationHealthAndShutdownAgree) {
    auto registry = std::make_shared<PluginRegistry>();
    auto plugin = std::make_shared<TestPlugin>(metadata("healthy"));
    ASSERT_TRUE(registry->registerPlugin(plugin));
    PluginManager manager;
    manager.setRegistry(registry);

    ASSERT_TRUE(manager.initializeAll());
    EXPECT_TRUE(manager.isPluginEnabled("healthy"));
    EXPECT_TRUE(plugin->isInitialized());
    EXPECT_TRUE(manager.checkPluginHealth("healthy"));
    manager.shutdownAll();
    EXPECT_FALSE(plugin->isInitialized());
    EXPECT_FALSE(manager.isPluginEnabled("healthy"));
    EXPECT_FALSE(manager.checkPluginHealth("healthy"));
}

TEST(PluginManager, InitializeFailureRollsBackPriorPlugins) {
    auto registry = std::make_shared<PluginRegistry>();
    auto first = std::make_shared<TestPlugin>(metadata("a-first"));
    auto second = std::make_shared<TestPlugin>(metadata("b-second"));
    second->initializeResult = false;
    ASSERT_TRUE(registry->registerPlugin(first));
    ASSERT_TRUE(registry->registerPlugin(second));
    PluginManager manager;
    manager.setRegistry(registry);

    EXPECT_FALSE(manager.initializeAll());
    EXPECT_EQ(first->shutdownCalls.load(), 1);
    EXPECT_FALSE(first->isInitialized());
    EXPECT_FALSE(manager.isPluginEnabled("a-first"));
    EXPECT_FALSE(manager.isPluginEnabled("b-second"));
}

TEST(PluginManager, RollbackShutdownFailureRemainsTruthfullyEnabled) {
    auto registry = std::make_shared<PluginRegistry>();
    auto first = std::make_shared<TestPlugin>(metadata("a-first"));
    first->throwShutdown = true;
    auto second = std::make_shared<TestPlugin>(metadata("b-second"));
    second->initializeResult = false;
    ASSERT_TRUE(registry->registerPlugin(first));
    ASSERT_TRUE(registry->registerPlugin(second));
    PluginManager manager;
    manager.setRegistry(registry);

    EXPECT_FALSE(manager.initializeAll());
    EXPECT_TRUE(first->isInitialized());
    EXPECT_TRUE(manager.isPluginEnabled("a-first"));
    first->throwShutdown = false;
}

TEST(PluginManager, ExecutionExceptionPropagatesAsFailureWithoutDeadlock) {
    auto registry = std::make_shared<PluginRegistry>();
    auto plugin = std::make_shared<TestPlugin>(metadata("throwing"));
    plugin->throwExecute = true;
    ASSERT_TRUE(registry->registerPlugin(plugin));
    PluginManager manager;
    manager.setRegistry(registry);
    ASSERT_TRUE(manager.initializeAll());

    PluginContext context;
    auto result = manager.executePlugin("throwing", context);
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.message.find("execute boom"), std::string::npos);
}

TEST(PluginRegistry, ConcurrentReadsAndWritesRemainConsistent) {
    PluginRegistry registry;
    std::atomic<bool> stop{false};
    std::thread reader([&] {
        while (!stop.load()) {
            (void)registry.getAllPlugins();
            (void)registry.getStatistics();
            (void)registry.getDependencyOrder();
        }
    });
    for (int i = 0; i < 100; ++i) {
        const auto name = "plugin-" + std::to_string(i);
        ASSERT_TRUE(registry.registerPlugin(std::make_shared<TestPlugin>(metadata(name))));
        ASSERT_TRUE(registry.unregisterPlugin(name));
    }
    stop.store(true);
    reader.join();
    EXPECT_TRUE(registry.getAllPlugins().empty());
}

TEST(PluginRegistry, DynamicLoadingInvalidPathsFailTruthfully) {
    PluginRegistry registry;
    EXPECT_EQ(registry.loadPlugin(""), nullptr);
    EXPECT_EQ(registry.loadPlugin("/nonexistent/plugin.so"), nullptr);
    EXPECT_FALSE(registry.unloadPlugin("unknown"));
    EXPECT_FALSE(registry.hotReloadPlugin("unknown"));
}
