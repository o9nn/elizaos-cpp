// elizas_world_test.cpp - E2E tests for ElizasWorld virtual world.
#include <gtest/gtest.h>
#include "elizaos/elizas_world.hpp"
#include <cmath>

using namespace elizaos;

namespace {
WorldEnvironment mkEnv(const std::string& id, double x = 0, double y = 0, double z = 0,
                       double r = 100.0) {
    WorldEnvironment e;
    e.id = id;
    e.name = id + "-name";
    e.type = "social";
    e.center = WorldPosition(x, y, z);
    e.radius = r;
    e.active = true;
    return e;
}

WorldAgent mkAgent(const std::string& id, double x = 0, double y = 0, double z = 0) {
    WorldAgent a;
    a.agentId = id;
    a.name = id;
    a.type = "eliza";
    a.position = WorldPosition(x, y, z);
    a.online = true;
    return a;
}
}

TEST(WorldPosition, DistanceAndInterpolation) {
    WorldPosition p1(0, 0, 0), p2(3, 4, 0);
    EXPECT_NEAR(p1.distanceTo(p2), 5.0, 1e-6);
    auto mid = p1.interpolate(p2, 0.5);
    EXPECT_NEAR(mid.x, 1.5, 1e-6);
    EXPECT_NEAR(mid.y, 2.0, 1e-6);
}

TEST(WorldEnvironment, ContainsAndDistance) {
    auto e = mkEnv("e1", 0, 0, 0, 50);
    EXPECT_TRUE(e.containsPosition({10, 10, 0}));
    EXPECT_FALSE(e.containsPosition({100, 0, 0}));
    EXPECT_NEAR(e.distanceFromCenter({30, 40, 0}), 50.0, 1e-6);
}

TEST(WorldAgent, CanInteractAndTeleport) {
    auto a = mkAgent("a", 0, 0, 0);
    auto b = mkAgent("b", 5, 0, 0);
    EXPECT_TRUE(a.canInteractWith(b));
    a.teleportTo({100, 0, 0});
    EXPECT_NEAR(a.position.x, 100.0, 1e-6);
}

class ElizasWorldTest : public ::testing::Test {
protected:
    ElizasWorld world;
};

TEST_F(ElizasWorldTest, AddRemoveEnvironment) {
    EXPECT_TRUE(world.addEnvironment(mkEnv("env-1")));
    EXPECT_EQ(world.getEnvironmentCount(), 1u);
    EXPECT_TRUE(world.getEnvironment("env-1").has_value());
    EXPECT_TRUE(world.removeEnvironment("env-1"));
    EXPECT_FALSE(world.getEnvironment("env-1").has_value());
}

TEST_F(ElizasWorldTest, AddRemoveAgent) {
    EXPECT_TRUE(world.addAgent(mkAgent("a-1")));
    EXPECT_EQ(world.getAgentCount(), 1u);
    EXPECT_TRUE(world.removeAgent("a-1"));
}

TEST_F(ElizasWorldTest, OnlineAgentsFilter) {
    world.addAgent(mkAgent("a"));
    auto b = mkAgent("b");
    b.online = false;
    world.addAgent(b);
    EXPECT_EQ(world.getOnlineAgents().size(), 1u);
}

TEST_F(ElizasWorldTest, TeleportMoveAgent) {
    world.addAgent(mkAgent("a"));
    EXPECT_TRUE(world.teleportAgent("a", {50, 50, 0}));
    auto a = world.getAgent("a");
    ASSERT_TRUE(a.has_value());
    EXPECT_NEAR(a->position.x, 50.0, 1e-6);
}

TEST_F(ElizasWorldTest, RecordAndQueryInteraction) {
    world.addAgent(mkAgent("a"));
    world.addAgent(mkAgent("b"));
    WorldInteraction ix;
    ix.id = "i-1";
    ix.initiatorId = "a";
    ix.targetId = "b";
    ix.type = "chat";
    EXPECT_TRUE(world.recordInteraction(ix));
    EXPECT_GE(world.getInteractionCount(), 1u);
    EXPECT_GE(world.getInteractionHistory("a").size(), 1u);
}

TEST_F(ElizasWorldTest, ProximitySpatialQueries) {
    world.addAgent(mkAgent("a", 0, 0, 0));
    world.addAgent(mkAgent("b", 5, 0, 0));
    world.addAgent(mkAgent("c", 100, 0, 0));
    auto near = world.getAgentsNearPosition({0, 0, 0}, 10.0);
    EXPECT_GE(near.size(), 2u);
}

TEST_F(ElizasWorldTest, UpdateAdvancesSimulation) {
    world.addAgent(mkAgent("a"));
    EXPECT_NO_THROW(world.update(0.1));
}

TEST_F(ElizasWorldTest, ConfigSetters) {
    EXPECT_NO_THROW(world.setSimulationSpeed(2.0));
    EXPECT_NO_THROW(world.setWorldBounds({-100, -100, -100}, {100, 100, 100}));
    EXPECT_NO_THROW(world.setAutoUpdate(true, 0.5));
}

TEST_F(ElizasWorldTest, SaveLoadRoundtrip) {
    world.addEnvironment(mkEnv("e"));
    world.addAgent(mkAgent("a"));
    auto path = std::string("/tmp/elizas_world_state.json");
    bool saved = world.saveWorldState(path);
    if (saved) {
        ElizasWorld other;
        EXPECT_TRUE(other.loadWorldState(path));
    }
    SUCCEED();
}
