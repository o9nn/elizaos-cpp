// elizas_world_test.cpp - E2E tests for ElizasWorld virtual world.
#include <gtest/gtest.h>
#include "elizaos/elizas_world.hpp"
#include <cmath>
#include <atomic>
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

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

TEST_F(ElizasWorldTest, SaveLoadRoundtripPreservesCompleteWorldState) {
    namespace fs = std::filesystem;
    static std::atomic<unsigned long long> counter{0};
    const fs::path directory = fs::temp_directory_path() /
        ("elizas_world_roundtrip_" + std::to_string(counter.fetch_add(1)));
    const fs::path path = directory / "world.json";
    fs::remove_all(directory);

    auto environment = mkEnv("creative-zone", 12.5, -4.0, 7.0, 250.0);
    environment.description = "A collaborative world center";
    environment.properties = {{"theme", "living-structure"}, {"access", "open"}};
    environment.createdAt = std::chrono::system_clock::time_point(std::chrono::milliseconds(123456));
    ASSERT_TRUE(world.addEnvironment(environment));

    auto agent = mkAgent("eliza", 10.0, -3.0, 8.0);
    agent.velocity = {0.5, 1.5, -0.25};
    agent.interactionRadius = 17.5;
    agent.currentEnvironment = environment.id;
    agent.attributes = {{"role", "coordinator"}, {"mood", "curious"}};
    agent.lastUpdate = std::chrono::system_clock::time_point(std::chrono::milliseconds(234567));
    ASSERT_TRUE(world.addAgent(agent));

    WorldInteraction interaction;
    interaction.id = "interaction-1";
    interaction.initiatorId = agent.agentId;
    interaction.targetId = agent.agentId;
    interaction.type = "reflection";
    interaction.location = {11.0, -3.5, 7.5};
    interaction.timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(345678));
    interaction.metadata = {{"topic", "autonomy"}, {"result", "coherent"}};
    interaction.completed = true;
    ASSERT_TRUE(world.recordInteraction(interaction));

    world.setWorldBounds({-500.0, -400.0, -300.0}, {500.0, 400.0, 300.0});
    world.setSimulationSpeed(2.25);
    world.setAutoUpdate(true, 0.25);

    ASSERT_TRUE(world.saveWorldState(path.string()));
    ASSERT_TRUE(fs::is_regular_file(path));
    EXPECT_GT(fs::file_size(path), 0u);

    ElizasWorld restored;
    ASSERT_TRUE(restored.loadWorldState(path.string()));
    EXPECT_EQ(restored.getEnvironmentCount(), 1u);
    EXPECT_EQ(restored.getAgentCount(), 1u);
    EXPECT_EQ(restored.getInteractionCount(), 1u);

    const auto restoredEnvironment = restored.getEnvironment(environment.id);
    ASSERT_TRUE(restoredEnvironment.has_value());
    EXPECT_EQ(restoredEnvironment->description, environment.description);
    EXPECT_EQ(restoredEnvironment->properties, environment.properties);
    EXPECT_DOUBLE_EQ(restoredEnvironment->center.x, environment.center.x);
    EXPECT_DOUBLE_EQ(restoredEnvironment->radius, environment.radius);
    EXPECT_EQ(restoredEnvironment->createdAt, environment.createdAt);

    const auto restoredAgent = restored.getAgent(agent.agentId);
    ASSERT_TRUE(restoredAgent.has_value());
    EXPECT_EQ(restoredAgent->attributes, agent.attributes);
    EXPECT_EQ(restoredAgent->currentEnvironment, environment.id);
    EXPECT_DOUBLE_EQ(restoredAgent->velocity.y, agent.velocity.y);
    EXPECT_DOUBLE_EQ(restoredAgent->interactionRadius, agent.interactionRadius);
    EXPECT_EQ(restoredAgent->lastUpdate, agent.lastUpdate);

    const auto restoredInteractions = restored.getInteractionHistory(agent.agentId);
    ASSERT_EQ(restoredInteractions.size(), 1u);
    EXPECT_EQ(restoredInteractions.front().metadata, interaction.metadata);
    EXPECT_EQ(restoredInteractions.front().timestamp, interaction.timestamp);
    EXPECT_TRUE(restoredInteractions.front().completed);

    using nlohmann::json;
    const json originalDocument = json::parse(world.exportWorldData());
    const json restoredDocument = json::parse(restored.exportWorldData());
    EXPECT_EQ(restoredDocument, originalDocument);
    EXPECT_EQ(restoredDocument.at("schema"), "elizaos.world_state");
    EXPECT_EQ(restoredDocument.at("version"), 1);
    EXPECT_DOUBLE_EQ(restoredDocument.at("simulation").at("speed").get<double>(), 2.25);
    EXPECT_TRUE(restoredDocument.at("simulation").at("auto_update").get<bool>());

    fs::remove_all(directory);
}

TEST_F(ElizasWorldTest, ImportRejectsMalformedOrInconsistentDataTransactionally) {
    using nlohmann::json;
    auto environment = mkEnv("env");
    ASSERT_TRUE(world.addEnvironment(environment));
    auto agent = mkAgent("agent");
    agent.currentEnvironment = environment.id;
    ASSERT_TRUE(world.addAgent(agent));
    const json baseline = json::parse(world.exportWorldData());

    EXPECT_FALSE(world.importWorldData("not-json"));
    EXPECT_FALSE(world.importWorldData("{}"));

    json wrongVersion = baseline;
    wrongVersion["version"] = 99;
    EXPECT_FALSE(world.importWorldData(wrongVersion.dump()));

    json duplicateEnvironment = baseline;
    duplicateEnvironment["environments"].push_back(duplicateEnvironment["environments"].front());
    EXPECT_FALSE(world.importWorldData(duplicateEnvironment.dump()));

    json danglingEnvironment = baseline;
    danglingEnvironment["agents"][0]["current_environment"] = "missing";
    EXPECT_FALSE(world.importWorldData(danglingEnvironment.dump()));

    json invalidBounds = baseline;
    invalidBounds["world_bounds"]["min"]["x"] = 100.0;
    invalidBounds["world_bounds"]["max"]["x"] = -100.0;
    EXPECT_FALSE(world.importWorldData(invalidBounds.dump()));

    EXPECT_EQ(json::parse(world.exportWorldData()), baseline)
        << "Rejected imports must not partially mutate live world state";
}

TEST_F(ElizasWorldTest, PersistenceReportsRealFilesystemFailures) {
    EXPECT_FALSE(world.saveWorldState(""));
    EXPECT_FALSE(world.loadWorldState(""));
    EXPECT_FALSE(world.loadWorldState("/definitely/missing/elizas_world_state.json"));
    EXPECT_FALSE(world.saveWorldState("/proc/elizaos/world_state.json"));
}
