// the_org_test.cpp - E2E tests for TheOrgAgent (via concrete CommunityManagerAgent).
#include <gtest/gtest.h>
#include "elizaos/the_org.hpp"
#include "elizaos/core.hpp"

using namespace elizaos;

namespace {
AgentConfig mkAC(const std::string& name) {
    AgentConfig c;
    c.agentName = name;
    c.bio = "test";
    return c;
}
}

TEST(PlatformConfig, BasicConstruction) {
    PlatformConfig p;
    p.type = PlatformType::DISCORD;
    p.applicationId = "app";
    p.apiToken = "tok";
    EXPECT_EQ(p.type, PlatformType::DISCORD);
}

TEST(OrganizationConfig, BasicFields) {
    OrganizationConfig o;
    o.name = "Org";
    o.description = "test org";
    EXPECT_EQ(o.name, "Org");
}

TEST(AgentRole, EnumValuesDistinct) {
    EXPECT_NE(static_cast<int>(AgentRole::COMMUNITY_MANAGER),
              static_cast<int>(AgentRole::SOCIAL_MEDIA_MANAGER));
}

TEST(CommunityManagerAgent, ConstructionAndConfigGetSet) {
    CommunityManagerAgent a(mkAC("Eli5"));
    a.updateConfig({{"k", "v"}});
    EXPECT_EQ(a.getConfigValue("k"), "v");
}

TEST(CommunityManagerAgent, MemoryCreate) {
    CommunityManagerAgent a(mkAC("X"));
    auto m = a.createMemory("hello");
    ASSERT_NE(m, nullptr);
    a.addMemory(m);
    auto found = a.searchMemories("hello", 5);
    SUCCEED() << "results=" << found.size();
}

TEST(CommunityManagerAgent, TaskCreateAndComplete) {
    CommunityManagerAgent a(mkAC("X"));
    auto id = a.createTask("task1", "do something", 1);
    EXPECT_FALSE(id.empty());
    EXPECT_NO_THROW(a.completeTask(id));
}

TEST(CommunityManagerAgent, AddPlatform) {
    CommunityManagerAgent a(mkAC("X"));
    PlatformConfig p; p.type = PlatformType::SLACK;
    EXPECT_NO_THROW(a.addPlatform(p));
    EXPECT_NO_THROW(a.removePlatform(PlatformType::SLACK));
}
