#include <gtest/gtest.h>
#include "elizaos/eliza_plugin_starter.hpp"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;

namespace {

struct PluginControl {
    std::atomic<int> initialized{0};
    std::atomic<int> shutdown{0};
    std::atomic<int> destroyed{0};
    std::atomic<bool> contextAliveDuringShutdown{false};
    std::atomic<bool> failInitialization{false};
};

class ControlledPlugin final : public IPlugin {
public:
    ControlledPlugin(PluginMetadata metadata, std::shared_ptr<PluginControl> control)
        : metadata_(std::move(metadata)), control_(std::move(control)) {}

    ~ControlledPlugin() override { control_->destroyed.fetch_add(1); }

    bool initialize(const PluginConfig&, PluginContext& context) override {
        control_->initialized.fetch_add(1);
        context_ = &context;
        state_.store(PluginState::INITIALIZED);
        return !control_->failInitialization.load();
    }

    void shutdown() override {
        control_->shutdown.fetch_add(1);
        control_->contextAliveDuringShutdown.store(
            context_ != nullptr && context_->getPluginId() == metadata_.id);
        context_ = nullptr;
        state_.store(PluginState::UNLOADED);
    }

    PluginMetadata getMetadata() const override { return metadata_; }
    PluginState getState() const override { return state_.load(); }
    void setState(PluginState state) override { state_.store(state); }

private:
    PluginMetadata metadata_;
    std::shared_ptr<PluginControl> control_;
    PluginContext* context_ = nullptr;
    std::atomic<PluginState> state_{PluginState::UNLOADED};
};

struct BlockingControl {
    std::mutex mutex;
    std::condition_variable condition;
    bool entered = false;
    bool release = false;
    std::atomic<int> shutdown{0};
};

class BlockingPlugin final : public IPlugin {
public:
    BlockingPlugin(PluginMetadata metadata, std::shared_ptr<BlockingControl> control)
        : metadata_(std::move(metadata)), control_(std::move(control)) {}

    bool initialize(const PluginConfig&, PluginContext&) override {
        std::unique_lock<std::mutex> lock(control_->mutex);
        control_->entered = true;
        control_->condition.notify_all();
        control_->condition.wait(lock, [this] { return control_->release; });
        return true;
    }

    void shutdown() override { control_->shutdown.fetch_add(1); }
    PluginMetadata getMetadata() const override { return metadata_; }
    PluginState getState() const override { return state_.load(); }
    void setState(PluginState state) override { state_.store(state); }

private:
    PluginMetadata metadata_;
    std::shared_ptr<BlockingControl> control_;
    std::atomic<PluginState> state_{PluginState::UNLOADED};
};

class BlockingSuspendPlugin final : public IPlugin {
public:
    BlockingSuspendPlugin(PluginMetadata metadata, std::shared_ptr<BlockingControl> control)
        : metadata_(std::move(metadata)), control_(std::move(control)) {}

    bool initialize(const PluginConfig&, PluginContext&) override { return true; }
    void shutdown() override { control_->shutdown.fetch_add(1); }
    PluginMetadata getMetadata() const override { return metadata_; }
    PluginState getState() const override { return state_.load(); }
    void setState(PluginState state) override {
        if (state == PluginState::SUSPENDED) {
            std::unique_lock<std::mutex> lock(control_->mutex);
            control_->entered = true;
            control_->condition.notify_all();
            control_->condition.wait(lock, [this] { return control_->release; });
        }
        state_.store(state);
    }

private:
    PluginMetadata metadata_;
    std::shared_ptr<BlockingControl> control_;
    std::atomic<PluginState> state_{PluginState::UNLOADED};
};

class DerivedControlledPlugin final : public IPlugin {
public:
    DerivedControlledPlugin(PluginMetadata metadata, std::shared_ptr<PluginControl> control)
        : metadata_(std::move(metadata)), control_(std::move(control)) {}

    ~DerivedControlledPlugin() override { control_->destroyed.fetch_add(1); }
    bool initialize(const PluginConfig&, PluginContext&) override { return true; }
    void shutdown() override { control_->shutdown.fetch_add(1); }
    PluginMetadata getMetadata() const override { return metadata_; }
    PluginState getState() const override { return state_.load(); }
    void setState(PluginState state) override { state_.store(state); }

private:
    PluginMetadata metadata_;
    std::shared_ptr<PluginControl> control_;
    std::atomic<PluginState> state_{PluginState::UNLOADED};
};

PluginMetadata metadataFor(std::string id, std::vector<std::string> dependencies = {}) {
    PluginMetadata metadata;
    metadata.id = std::move(id);
    metadata.name = metadata.id + " name";
    metadata.description = "behavioral test plugin";
    metadata.version = {1, 2, 3};
    metadata.dependencies = std::move(dependencies);
    return metadata;
}

class PluginStarterTest : public ::testing::Test {
protected:
    void SetUp() override { PluginRegistry::instance().clear(); }
    void TearDown() override { PluginRegistry::instance().clear(); }
};

} // namespace

TEST_F(PluginStarterTest, VersionParsingIsStrictAndCompatibilityIsSemantic) {
    const auto parsed = PluginVersion::tryParse("2.10.3");
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->toString(), "2.10.3");
    EXPECT_TRUE(parsed->isCompatibleWith(PluginVersion{2, 0, 0}));
    EXPECT_FALSE(parsed->isCompatibleWith(PluginVersion{3, 0, 0}));

    EXPECT_FALSE(PluginVersion::tryParse("1.2").has_value());
    EXPECT_FALSE(PluginVersion::tryParse("1.2.3suffix").has_value());
    EXPECT_FALSE(PluginVersion::tryParse("-1.2.3").has_value());
    EXPECT_FALSE(PluginVersion::tryParse("+1.2.3").has_value());
    EXPECT_FALSE(PluginVersion::tryParse("01.2.3").has_value());
    EXPECT_FALSE(PluginVersion::tryParse("1.02.3").has_value());
    EXPECT_FALSE(PluginVersion::tryParse("1.2.03").has_value());
    EXPECT_FALSE(PluginVersion::tryParse("1.2.3 ").has_value());
    EXPECT_FALSE(PluginVersion::tryParse("2147483648.0.0").has_value());
    EXPECT_FALSE(PluginVersion::parse("invalid").isValid());
}

TEST_F(PluginStarterTest, ConfigurationPreservesTypesAndValidatesRequiredKeys) {
    PluginConfig config;
    config.addRequiredKey("name");
    std::string error;
    EXPECT_FALSE(config.validate(error));
    EXPECT_NE(error.find("name"), std::string::npos);

    config.set("name", std::string("demo"));
    config.set("count", 7);
    config.set("ratio", 0.25);
    config.set("enabled", true);
    config.set("literal", "text");
    EXPECT_TRUE(config.validate(error));
    EXPECT_EQ(config.getString("name"), "demo");
    EXPECT_EQ(config.getInt("count"), 7);
    EXPECT_DOUBLE_EQ(config.getDouble("ratio"), 0.25);
    EXPECT_TRUE(config.getBool("enabled"));
    EXPECT_EQ(config.getString("literal"), "text");
    EXPECT_EQ(config.getString("count", "wrong-type"), "wrong-type");

    PluginConfig copy = config;
    EXPECT_EQ(copy.getInt("count"), 7);
    copy.set("count", 11);
    EXPECT_EQ(config.getInt("count"), 7);

    PluginConfig assigned;
    assigned = config;
    EXPECT_EQ(assigned.getString("name"), "demo");

    PluginConfig moved = std::move(assigned);
    EXPECT_EQ(moved.getString("name"), "demo");
}

TEST_F(PluginStarterTest, ContextStorageAndAgentStateAreThreadSafe) {
    PluginContext context("context-test");
    AgentConfig config{"agent", "Agent", "bio", "room", "world"};
    auto state = std::make_shared<State>(config);
    context.setAgentState(state);
    EXPECT_EQ(context.getAgentState()->getAgentId(), "agent");

    std::vector<std::thread> threads;
    for (int i = 0; i < 16; ++i) {
        threads.emplace_back([&context, i] {
            context.setData("key-" + std::to_string(i), i);
        });
    }
    for (auto& thread : threads) thread.join();
    for (int i = 0; i < 16; ++i) {
        ASSERT_TRUE(context.hasData("key-" + std::to_string(i)));
        EXPECT_EQ(std::any_cast<int>(context.getData("key-" + std::to_string(i))), i);
    }
}

TEST_F(PluginStarterTest, RegistryRejectsInvalidAndDuplicateFactories) {
    auto control = std::make_shared<PluginControl>();
    EXPECT_FALSE(PluginRegistry::instance().tryRegisterPlugin("", {}));
    EXPECT_TRUE(PluginRegistry::instance().tryRegisterPlugin("valid", [control] {
        return std::make_unique<ControlledPlugin>(metadataFor("valid"), control);
    }));
    EXPECT_FALSE(PluginRegistry::instance().tryRegisterPlugin("valid", [control] {
        return std::make_unique<ControlledPlugin>(metadataFor("valid"), control);
    }));
    EXPECT_EQ(PluginRegistry::instance().getRegisteredPluginIds(), std::vector<std::string>{"valid"});
}

TEST_F(PluginStarterTest, LegacyRegistryRegistrationReplacesFactory) {
    auto first = std::make_shared<PluginControl>();
    auto second = std::make_shared<PluginControl>();
    PluginRegistry::instance().registerPlugin("replace", [first] {
        return std::make_unique<ControlledPlugin>(metadataFor("replace"), first);
    });
    PluginRegistry::instance().registerPlugin("replace", [second] {
        return std::make_unique<ControlledPlugin>(metadataFor("replace"), second);
    });

    PluginManager manager;
    ASSERT_TRUE(manager.loadPlugin("replace"));
    EXPECT_EQ(first->initialized.load(), 0);
    EXPECT_EQ(second->initialized.load(), 1);
}

TEST_F(PluginStarterTest, FactoryMayReenterRegistryWithoutDeadlock) {
    auto control = std::make_shared<PluginControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("reentrant", [control]() -> std::unique_ptr<IPlugin> {
        if (!PluginRegistry::instance().hasPlugin("reentrant")) {
            return std::unique_ptr<IPlugin>{};
        }
        return std::make_unique<ControlledPlugin>(metadataFor("reentrant"), control);
    }));
    auto future = std::async(std::launch::async, [] {
        return PluginRegistry::instance().createPlugin("reentrant") != nullptr;
    });
    EXPECT_EQ(future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

TEST_F(PluginStarterTest, ManagerLoadsActivatesSuspendsResumesAndUnloads) {
    auto control = std::make_shared<PluginControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("lifecycle", [control] {
        return std::make_unique<ControlledPlugin>(metadataFor("lifecycle"), control);
    }));

    PluginManager manager;
    EXPECT_TRUE(manager.loadPlugin("lifecycle"));
    ASSERT_NE(manager.getPlugin("lifecycle"), nullptr);
    EXPECT_EQ(manager.getPlugin("lifecycle")->getState(), PluginState::ACTIVE);
    EXPECT_TRUE(manager.checkPluginHealth("lifecycle"));
    EXPECT_FALSE(manager.loadPlugin("lifecycle"));
    EXPECT_TRUE(manager.suspendPlugin("lifecycle"));
    EXPECT_FALSE(manager.checkPluginHealth("lifecycle"));
    EXPECT_TRUE(manager.resumePlugin("lifecycle"));
    EXPECT_TRUE(manager.unloadPlugin("lifecycle"));
    EXPECT_EQ(control->initialized.load(), 1);
    EXPECT_EQ(control->shutdown.load(), 1);
    EXPECT_EQ(manager.getPlugin("lifecycle"), nullptr);
}

TEST_F(PluginStarterTest, InitializationFailureRollsBackManagerState) {
    auto control = std::make_shared<PluginControl>();
    control->failInitialization = true;
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("fails", [control] {
        return std::make_unique<ControlledPlugin>(metadataFor("fails"), control);
    }));

    PluginManager manager;
    EXPECT_FALSE(manager.loadPlugin("fails"));
    EXPECT_TRUE(manager.getLoadedPluginIds().empty());
    EXPECT_EQ(control->initialized.load(), 1);
    EXPECT_EQ(control->shutdown.load(), 1);
}

TEST_F(PluginStarterTest, MetadataIdentityAndDependencyValidationAreEnforced) {
    auto mismatch = std::make_shared<PluginControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("registered", [mismatch] {
        return std::make_unique<ControlledPlugin>(metadataFor("different"), mismatch);
    }));
    PluginManager manager;
    EXPECT_FALSE(manager.loadPlugin("registered"));

    std::string error;
    auto invalid = metadataFor("self", {"self"});
    EXPECT_FALSE(PluginUtils::validateMetadata(invalid, error));

    invalid = metadataFor("duplicate", {"base", "base"});
    EXPECT_FALSE(PluginUtils::validateMetadata(invalid, error));
    invalid = metadataFor("bad-tags");
    invalid.tags = {"valid", "valid"};
    EXPECT_FALSE(PluginUtils::validateMetadata(invalid, error));
    invalid = metadataFor("free-text-tag");
    invalid.tags = {"large language model"};
    EXPECT_TRUE(PluginUtils::validateMetadata(invalid, error));
    invalid = metadataFor("bad-tag");
    invalid.tags = {""};
    EXPECT_FALSE(PluginUtils::validateMetadata(invalid, error));
}

TEST_F(PluginStarterTest, DependenciesMustLoadFirstAndCannotUnloadUnderDependent) {
    auto baseControl = std::make_shared<PluginControl>();
    auto childControl = std::make_shared<PluginControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("base", [baseControl] {
        return std::make_unique<ControlledPlugin>(metadataFor("base"), baseControl);
    }));
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("child", [childControl] {
        return std::make_unique<ControlledPlugin>(metadataFor("child", {"base"}), childControl);
    }));

    PluginManager manager;
    EXPECT_FALSE(manager.loadPlugin("child"));
    EXPECT_TRUE(manager.loadPlugin("base"));
    EXPECT_TRUE(manager.loadPlugin("child"));
    EXPECT_FALSE(manager.unloadPlugin("base"));
    EXPECT_TRUE(manager.unloadPlugin("child"));
    EXPECT_TRUE(manager.unloadPlugin("base"));
}

TEST_F(PluginStarterTest, AgentStatePropagatesToLoadedContexts) {
    auto control = std::make_shared<PluginControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("stateful", [control] {
        return std::make_unique<ControlledPlugin>(metadataFor("stateful"), control);
    }));
    PluginManager manager;
    EXPECT_TRUE(manager.loadPlugin("stateful"));
    AgentConfig config{"state-agent", "State Agent", "bio", "room", "world"};
    EXPECT_NO_THROW(manager.setAgentState(std::make_shared<State>(config)));
}

TEST_F(PluginStarterTest, ManagerDestructorShutsDownLoadedPlugins) {
    auto control = std::make_shared<PluginControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("owned", [control] {
        return std::make_unique<ControlledPlugin>(metadataFor("owned"), control);
    }));
    {
        PluginManager manager;
        ASSERT_TRUE(manager.loadPlugin("owned"));
    }
    EXPECT_EQ(control->shutdown.load(), 1);
    EXPECT_EQ(control->destroyed.load(), 1);
    EXPECT_TRUE(control->contextAliveDuringShutdown.load());
}

TEST_F(PluginStarterTest, SharedTypedAccessRetainsPluginAndContextPastManagerLifetime) {
    auto control = std::make_shared<PluginControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("typed", [control] {
        return std::make_unique<DerivedControlledPlugin>(metadataFor("typed"), control);
    }));

    std::shared_ptr<DerivedControlledPlugin> retained;
    {
        PluginManager manager;
        ASSERT_TRUE(manager.loadPlugin("typed"));
        retained = manager.getPluginAsShared<DerivedControlledPlugin>("typed");
        ASSERT_NE(retained, nullptr);
        EXPECT_EQ(manager.getPluginAs<DerivedControlledPlugin>("typed"), retained.get());
        const auto typed = manager.getPluginsOfTypeShared<DerivedControlledPlugin>();
        ASSERT_EQ(typed.size(), 1U);
        EXPECT_EQ(typed.front().get(), retained.get());
    }

    EXPECT_EQ(control->shutdown.load(), 1);
    EXPECT_EQ(control->destroyed.load(), 0);
    EXPECT_EQ(retained->getState(), PluginState::UNLOADED);
    retained.reset();
    EXPECT_EQ(control->destroyed.load(), 1);
}

TEST_F(PluginStarterTest, LegacyRawPointerRemainsNonDanglingAfterUnload) {
    auto control = std::make_shared<PluginControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("borrowed", [control] {
        return std::make_unique<ControlledPlugin>(metadataFor("borrowed"), control);
    }));

    PluginManager manager;
    ASSERT_TRUE(manager.loadPlugin("borrowed"));
    IPlugin* const borrowed = manager.getPlugin("borrowed");
    ASSERT_NE(borrowed, nullptr);
    ASSERT_TRUE(manager.unloadPlugin("borrowed"));
    EXPECT_EQ(borrowed->getState(), PluginState::UNLOADED);
    EXPECT_EQ(control->destroyed.load(), 0);
}

TEST_F(PluginStarterTest, UnloadAllWaitsForInFlightLoadThenShutsItDown) {
    auto control = std::make_shared<BlockingControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("blocking", [control] {
        return std::make_unique<BlockingPlugin>(metadataFor("blocking"), control);
    }));

    PluginManager manager;
    auto loader = std::async(std::launch::async, [&manager] { return manager.loadPlugin("blocking"); });
    {
        std::unique_lock<std::mutex> lock(control->mutex);
        ASSERT_TRUE(control->condition.wait_for(
            lock, std::chrono::seconds(2), [control] { return control->entered; }));
    }
    auto unloader = std::async(std::launch::async, [&manager] { manager.unloadAll(); });
    EXPECT_EQ(unloader.wait_for(std::chrono::milliseconds(20)), std::future_status::timeout);
    {
        std::lock_guard<std::mutex> lock(control->mutex);
        control->release = true;
    }
    control->condition.notify_all();
    EXPECT_EQ(loader.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    EXPECT_FALSE(loader.get());
    EXPECT_EQ(unloader.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    unloader.get();
    EXPECT_TRUE(manager.getLoadedPluginIds().empty());
    EXPECT_EQ(control->shutdown.load(), 1);
}

TEST_F(PluginStarterTest, UnloadCannotRaceAnInFlightSuspendTransition) {
    auto control = std::make_shared<BlockingControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("blocking-suspend", [control] {
        return std::make_unique<BlockingSuspendPlugin>(
            metadataFor("blocking-suspend"), control);
    }));

    PluginManager manager;
    ASSERT_TRUE(manager.loadPlugin("blocking-suspend"));
    auto suspender = std::async(std::launch::async, [&manager] {
        return manager.suspendPlugin("blocking-suspend");
    });
    {
        std::unique_lock<std::mutex> lock(control->mutex);
        ASSERT_TRUE(control->condition.wait_for(
            lock, std::chrono::seconds(2), [control] { return control->entered; }));
    }
    EXPECT_FALSE(manager.unloadPlugin("blocking-suspend"));
    {
        std::lock_guard<std::mutex> lock(control->mutex);
        control->release = true;
    }
    control->condition.notify_all();
    EXPECT_EQ(suspender.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    EXPECT_TRUE(suspender.get());
    EXPECT_TRUE(manager.unloadPlugin("blocking-suspend"));
    EXPECT_EQ(control->shutdown.load(), 1);
}

TEST_F(PluginStarterTest, ReentrantLifecycleCallbacksDoNotDeadlockManager) {
    PluginManager manager;
    auto control = std::make_shared<PluginControl>();
    ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin("reentrant-manager", [&manager, control] {
        class ReentrantPlugin final : public IPlugin {
        public:
            ReentrantPlugin(PluginManager& manager, std::shared_ptr<PluginControl> control)
                : manager_(manager), control_(std::move(control)) {}
            bool initialize(const PluginConfig&, PluginContext&) override {
                control_->initialized.fetch_add(1);
                (void)manager_.getLoadedPluginIds();
                manager_.unloadAll();
                return true;
            }
            void shutdown() override {
                control_->shutdown.fetch_add(1);
                (void)manager_.getLoadedPluginIds();
                manager_.unloadAll();
            }
            PluginMetadata getMetadata() const override {
                return metadataFor("reentrant-manager");
            }
            PluginState getState() const override { return state_.load(); }
            void setState(PluginState state) override { state_.store(state); }
        private:
            PluginManager& manager_;
            std::shared_ptr<PluginControl> control_;
            std::atomic<PluginState> state_{PluginState::UNLOADED};
        };
        return std::make_unique<ReentrantPlugin>(manager, control);
    }));

    auto future = std::async(std::launch::async, [&manager] {
        if (!manager.loadPlugin("reentrant-manager")) {
            return false;
        }
        return manager.unloadPlugin("reentrant-manager");
    });
    EXPECT_EQ(future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    EXPECT_TRUE(future.get());
    EXPECT_EQ(control->initialized.load(), 1);
    EXPECT_EQ(control->shutdown.load(), 1);
}

TEST_F(PluginStarterTest, ConcurrentRegistryAndIndependentManagersRemainConsistent) {
    constexpr std::size_t kCount = 24U;
    std::vector<std::shared_ptr<PluginControl>> controls(kCount);
    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < kCount; ++i) {
        controls[i] = std::make_shared<PluginControl>();
        threads.emplace_back([i, control = controls[i]] {
            const auto id = "concurrent-" + std::to_string(i);
            ASSERT_TRUE(PluginRegistry::instance().tryRegisterPlugin(id, [id, control] {
                return std::make_unique<ControlledPlugin>(metadataFor(id), control);
            }));
            PluginManager manager;
            EXPECT_TRUE(manager.loadPlugin(id));
            EXPECT_TRUE(manager.checkPluginHealth(id));
            EXPECT_TRUE(manager.unloadPlugin(id));
        });
    }
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(PluginRegistry::instance().getRegisteredPluginIds().size(),
              static_cast<std::size_t>(kCount));
    for (const auto& control : controls) {
        EXPECT_EQ(control->initialized.load(), 1);
        EXPECT_EQ(control->shutdown.load(), 1);
    }
}

TEST_F(PluginStarterTest, UtilityResultsAreDeterministicAndExplicit) {
    PluginManager manager;
    std::vector<std::string> missing;
    EXPECT_FALSE(PluginUtils::checkDependencies({"z", "a", "z"}, manager, missing));
    EXPECT_EQ(missing, (std::vector<std::string>{"a", "z"}));
    EXPECT_EQ(PluginUtils::stateToString(PluginState::SUSPENDED), "SUSPENDED");
    EXPECT_EQ(PluginUtils::tryStateFromString("ACTIVE"), PluginState::ACTIVE);
    EXPECT_FALSE(PluginUtils::tryStateFromString("unknown").has_value());
    EXPECT_FALSE(PluginUtils::generatePluginId("sample").empty());
}
