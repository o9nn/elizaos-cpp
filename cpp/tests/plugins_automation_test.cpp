// plugins_automation_test.cpp - E2E tests for plugin registry & CI pipeline.
#include <gtest/gtest.h>
#include "elizaos/plugins_automation.hpp"

using namespace elizaos;

namespace {
class TestPlugin : public Plugin {
public:
    explicit TestPlugin(const std::string& name) : name_(name) {}
    bool initialize(const PluginMetadata&) override { status_ = PluginStatus::LOADED; return true; }
    bool activate() override { status_ = PluginStatus::ACTIVE; return true; }
    bool deactivate() override { status_ = PluginStatus::INACTIVE; return true; }
    bool shutdown() override { status_ = PluginStatus::UNKNOWN; return true; }
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return "1.0.0"; }
    PluginStatus getStatus() const override { return status_; }
    std::vector<std::string> getDependencies() const override { return {}; }
private:
    std::string name_;
    PluginStatus status_ = PluginStatus::UNKNOWN;
};
}

TEST(PluginMetadata, ConvenienceConstructor) {
    PluginMetadata m("foo", "1.0", "desc");
    EXPECT_EQ(m.name, "foo");
    EXPECT_EQ(m.version, "1.0");
}

TEST(PluginRegistry, RegisterAndUnregister) {
    PluginRegistry r;
    auto p = std::make_shared<TestPlugin>("p1");
    PluginMetadata md("p1", "1.0", "");
    EXPECT_TRUE(r.registerPlugin(p, md));
    EXPECT_NE(r.getPlugin("p1"), nullptr);
    EXPECT_TRUE(r.unregisterPlugin("p1"));
    EXPECT_EQ(r.getPlugin("p1"), nullptr);
}

TEST(PluginRegistry, GetNamesAndStatus) {
    PluginRegistry r;
    r.registerPlugin(std::make_shared<TestPlugin>("p1"), PluginMetadata("p1", "1.0", ""));
    r.registerPlugin(std::make_shared<TestPlugin>("p2"), PluginMetadata("p2", "1.0", ""));
    EXPECT_EQ(r.getPluginNames().size(), 2u);
    EXPECT_NE(r.getPluginStatus("p1"), PluginStatus::FAILED);
}

TEST(PluginRegistry, ActiveAndFailedListsAreLists) {
    PluginRegistry r;
    EXPECT_NO_THROW(r.getActivePlugins());
    EXPECT_NO_THROW(r.getFailedPlugins());
}

TEST(PluginRegistry, ResolveDependenciesNoCycle) {
    PluginRegistry r;
    PluginMetadata md("base", "1.0", "");
    r.registerPlugin(std::make_shared<TestPlugin>("base"), md);
    EXPECT_NO_THROW(r.resolveDependencies("base"));
    auto chain = r.getDependencyChain("base");
    SUCCEED() << "chain=" << chain.size();
}

TEST(CIPipeline, ConstructionAndConfigSetters) {
    CIPipeline p;
    EXPECT_NO_THROW(p.setBuildCommand("echo build"));
}
