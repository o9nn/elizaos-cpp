// elizas_world_test.cpp - E2E tests for ElizasWorld virtual world.
#include <gtest/gtest.h>
#include "elizaos/elizas_world.hpp"
#include <nlohmann/json.hpp>
#include <atomic>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace elizaos;

namespace {
using json = nlohmann::json;

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

std::chrono::system_clock::time_point clockTime(std::int64_t ticks) {
    return std::chrono::system_clock::time_point(
        std::chrono::system_clock::duration(ticks));
}

std::filesystem::path temporaryPath(const std::string& stem) {
    static std::atomic<unsigned long long> counter{0};
    return std::filesystem::temp_directory_path() /
           (stem + "_" + std::to_string(counter.fetch_add(1)) + ".json");
}

class ScopedPath {
public:
    explicit ScopedPath(std::filesystem::path path) : path_(std::move(path)) {}
    ~ScopedPath() {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }
    const std::filesystem::path& get() const { return path_; }

private:
    std::filesystem::path path_;
};

std::string readFile(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(input),
                       std::istreambuf_iterator<char>());
}

void populateCompleteWorld(ElizasWorld& target) {
    target.setWorldBounds({-50.25, -60.5, -70.75}, {80.125, 90.25, 100.5});
    target.setSimulationSpeed(2.5);
    target.setAutoUpdate(true, 0.03125);

    WorldEnvironment environment;
    environment.id = "env-\u4E2D\u592E";
    environment.name = "Central \"Plaza\"";
    environment.description = "line one\nline two\\snowman \u2603";
    environment.type = "social/creative";
    environment.center = {1.25, -2.5, 3.75};
    environment.radius = 25.125;
    environment.properties = {
        {"escaped\"key", "tab\tnewline\nslash\\"},
        {"language", "Espa\u00F1ol \u65E5\u672C\u8A9E"}
    };
    environment.createdAt = clockTime(123456789);
    environment.active = true;
    ASSERT_TRUE(target.addEnvironment(environment));

    WorldAgent first;
    first.agentId = "agent-\u03B1";
    first.name = "Alice \"A\"";
    first.type = "eliza\nresearcher";
    first.position = {2.0, -3.0, 4.0};
    first.velocity = {-1.5, 2.25, -3.125};
    first.interactionRadius = 7.75;
    first.currentEnvironment = environment.id;
    first.attributes = {
        {"bio", "Unicode \u03BB and \\ escaped"},
        {"mood", "curious\nfocused"}
    };
    first.lastUpdate = clockTime(223456789);
    first.online = false;
    ASSERT_TRUE(target.addAgent(first));

    WorldAgent second;
    second.agentId = "agent-beta";
    second.name = "Bob";
    second.type = "trader";
    second.position = {5.0, 6.0, 7.0};
    second.velocity = {0.125, -0.25, 0.5};
    second.interactionRadius = 0.0;
    second.currentEnvironment = environment.id;
    second.attributes = {{"quote", "say \"hello\""}};
    second.lastUpdate = clockTime(323456789);
    second.online = true;
    ASSERT_TRUE(target.addAgent(second));

    WorldInteraction interaction;
    interaction.id = "interaction-\u96EA";
    interaction.initiatorId = first.agentId;
    interaction.targetId = second.agentId;
    interaction.type = "chat/collaboration";
    interaction.location = {3.5, -1.25, 6.75};
    interaction.timestamp = clockTime(423456789);
    interaction.metadata = {
        {"message", "Hello, \u4E16\u754C!\n\"quoted\""},
        {"path", "C:\\world\\state"}
    };
    interaction.completed = true;
    ASSERT_TRUE(target.recordInteraction(interaction));
}

void expectCompleteWorldFields(const ElizasWorld& target) {
    const auto environments = target.getAllEnvironments();
    ASSERT_EQ(environments.size(), 1u);
    EXPECT_EQ(environments[0].id, "env-\u4E2D\u592E");
    EXPECT_EQ(environments[0].name, "Central \"Plaza\"");
    EXPECT_EQ(environments[0].description, "line one\nline two\\snowman \u2603");
    EXPECT_EQ(environments[0].type, "social/creative");
    EXPECT_DOUBLE_EQ(environments[0].center.x, 1.25);
    EXPECT_DOUBLE_EQ(environments[0].center.y, -2.5);
    EXPECT_DOUBLE_EQ(environments[0].center.z, 3.75);
    EXPECT_DOUBLE_EQ(environments[0].radius, 25.125);
    EXPECT_EQ(environments[0].properties.at("language"), "Espa\u00F1ol \u65E5\u672C\u8A9E");
    EXPECT_EQ(environments[0].properties.at("escaped\"key"),
              "tab\tnewline\nslash\\");
    EXPECT_EQ(environments[0].createdAt, clockTime(123456789));
    EXPECT_TRUE(environments[0].active);

    const auto agents = target.getAllAgents();
    ASSERT_EQ(agents.size(), 2u);
    EXPECT_EQ(agents[0].agentId, "agent-\u03B1");
    EXPECT_EQ(agents[0].name, "Alice \"A\"");
    EXPECT_EQ(agents[0].type, "eliza\nresearcher");
    EXPECT_DOUBLE_EQ(agents[0].position.x, 2.0);
    EXPECT_DOUBLE_EQ(agents[0].position.y, -3.0);
    EXPECT_DOUBLE_EQ(agents[0].position.z, 4.0);
    EXPECT_DOUBLE_EQ(agents[0].velocity.x, -1.5);
    EXPECT_DOUBLE_EQ(agents[0].velocity.y, 2.25);
    EXPECT_DOUBLE_EQ(agents[0].velocity.z, -3.125);
    EXPECT_DOUBLE_EQ(agents[0].interactionRadius, 7.75);
    EXPECT_EQ(agents[0].currentEnvironment, environments[0].id);
    EXPECT_EQ(agents[0].attributes.at("bio"), "Unicode \u03BB and \\ escaped");
    EXPECT_EQ(agents[0].attributes.at("mood"), "curious\nfocused");
    EXPECT_EQ(agents[0].lastUpdate, clockTime(223456789));
    EXPECT_FALSE(agents[0].online);

    EXPECT_EQ(agents[1].agentId, "agent-beta");
    EXPECT_DOUBLE_EQ(agents[1].velocity.x, 0.125);
    EXPECT_DOUBLE_EQ(agents[1].interactionRadius, 0.0);
    EXPECT_EQ(agents[1].attributes.at("quote"), "say \"hello\"");
    EXPECT_EQ(agents[1].lastUpdate, clockTime(323456789));
    EXPECT_TRUE(agents[1].online);

    const auto interactions = target.getRecentInteractions();
    ASSERT_EQ(interactions.size(), 1u);
    EXPECT_EQ(interactions[0].id, "interaction-\u96EA");
    EXPECT_EQ(interactions[0].initiatorId, agents[0].agentId);
    EXPECT_EQ(interactions[0].targetId, agents[1].agentId);
    EXPECT_EQ(interactions[0].type, "chat/collaboration");
    EXPECT_DOUBLE_EQ(interactions[0].location.x, 3.5);
    EXPECT_DOUBLE_EQ(interactions[0].location.y, -1.25);
    EXPECT_DOUBLE_EQ(interactions[0].location.z, 6.75);
    EXPECT_EQ(interactions[0].timestamp, clockTime(423456789));
    EXPECT_EQ(interactions[0].metadata.at("message"),
              "Hello, \u4E16\u754C!\n\"quoted\"");
    EXPECT_EQ(interactions[0].metadata.at("path"), "C:\\world\\state");
    EXPECT_TRUE(interactions[0].completed);
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

TEST_F(ElizasWorldTest, ExportImportIsExactDeterministicAndPreservesEveryField) {
    populateCompleteWorld(world);

    const std::string exported = world.exportWorldData();
    EXPECT_EQ(exported, world.exportWorldData());
    EXPECT_NE(exported.find("\\n"), std::string::npos);
    EXPECT_NE(exported.find("\\\\"), std::string::npos);
    EXPECT_NE(exported.find("\u65E5\u672C\u8A9E"), std::string::npos);

    const json document = json::parse(exported);
    EXPECT_EQ(document.at("format"), "elizaos.elizas_world");
    EXPECT_EQ(document.at("version"), 1);
    EXPECT_EQ(document.at("world").at("bounds").at("min").at("x"), -50.25);
    EXPECT_EQ(document.at("world").at("bounds").at("max").at("z"), 100.5);
    EXPECT_EQ(document.at("world").at("simulation_speed"), 2.5);
    EXPECT_TRUE(document.at("world").at("auto_update").at("enabled"));
    EXPECT_EQ(document.at("world").at("auto_update").at("interval"), 0.03125);

    ElizasWorld restored;
    ASSERT_TRUE(restored.importWorldData(exported));
    EXPECT_EQ(restored.exportWorldData(), exported);
    expectCompleteWorldFields(restored);
}

TEST_F(ElizasWorldTest, EmptyWorldRoundTripsExactly) {
    const std::string exported = world.exportWorldData();
    ElizasWorld restored;
    ASSERT_TRUE(restored.importWorldData(exported));
    EXPECT_EQ(restored.exportWorldData(), exported);
    EXPECT_EQ(restored.getEnvironmentCount(), 0u);
    EXPECT_EQ(restored.getAgentCount(), 0u);
    EXPECT_EQ(restored.getInteractionCount(), 0u);
}

TEST_F(ElizasWorldTest, FileRoundTripIsExactAndOverwriteReplacesPriorState) {
    ScopedPath file(temporaryPath("elizas_world_roundtrip"));
    populateCompleteWorld(world);
    const std::string firstExport = world.exportWorldData();

    ASSERT_TRUE(world.saveWorldState(file.get().string()));
    EXPECT_EQ(readFile(file.get()), firstExport);

    ElizasWorld restored;
    ASSERT_TRUE(restored.loadWorldState(file.get().string()));
    EXPECT_EQ(restored.exportWorldData(), firstExport);
    expectCompleteWorldFields(restored);

    ElizasWorld empty;
    const std::string secondExport = empty.exportWorldData();
    ASSERT_NE(secondExport, firstExport);
    ASSERT_TRUE(empty.saveWorldState(file.get().string()));
    EXPECT_EQ(readFile(file.get()), secondExport);

    ElizasWorld overwritten;
    ASSERT_TRUE(overwritten.loadWorldState(file.get().string()));
    EXPECT_EQ(overwritten.exportWorldData(), secondExport);
}

TEST_F(ElizasWorldTest, MalformedAndSemanticallyInvalidImportsRollBack) {
    populateCompleteWorld(world);
    const std::string baseline = world.exportWorldData();
    const json valid = json::parse(baseline);
    std::vector<std::string> invalidDocuments = {
        "{ definitely not JSON",
        "null",
        "{}"
    };

    auto addInvalid = [&](const auto& mutation) {
        json invalid = valid;
        mutation(invalid);
        invalidDocuments.push_back(invalid.dump());
    };
    addInvalid([](json& value) { value["version"] = 2; });
    addInvalid([](json& value) { value.erase("agents"); });
    addInvalid([](json& value) { value["world"]["bounds"]["min"]["x"] = 1000.0; });
    addInvalid([](json& value) { value["world"]["simulation_speed"] = -0.01; });
    addInvalid([](json& value) { value["world"]["simulation_speed"] = nullptr; });
    addInvalid([](json& value) { value["world"]["auto_update"]["interval"] = -1.0; });
    addInvalid([](json& value) { value["environments"][0]["radius"] = -1.0; });
    addInvalid([](json& value) { value["environments"][0]["center"]["z"] = "NaN"; });
    addInvalid([](json& value) { value["agents"][0]["interaction_radius"] = -1.0; });
    addInvalid([](json& value) { value["agents"][0]["position"]["x"] = 999.0; });
    addInvalid([](json& value) { value["agents"][0]["velocity"]["y"] = nullptr; });
    addInvalid([](json& value) {
        value["agents"][0]["current_environment"] = "missing-environment";
    });
    addInvalid([](json& value) {
        value["interactions"][0]["initiator_id"] = "missing-agent";
    });
    addInvalid([](json& value) {
        value["interactions"][0]["target_id"] = "missing-agent";
    });
    addInvalid([](json& value) { value["interactions"][0]["location"]["x"] = false; });
    addInvalid([](json& value) { value["environments"][0]["properties"]["bad"] = 1; });
    addInvalid([](json& value) { value["agents"][0]["attributes"]["bad"] = true; });
    addInvalid([](json& value) { value["interactions"][0]["metadata"]["bad"] = {}; });

    for (const auto& invalid : invalidDocuments) {
        EXPECT_FALSE(world.importWorldData(invalid));
        EXPECT_EQ(world.exportWorldData(), baseline);
    }
}

TEST_F(ElizasWorldTest, DuplicateIdentifiersAreRejectedTransactionally) {
    populateCompleteWorld(world);
    const std::string baseline = world.exportWorldData();
    const json valid = json::parse(baseline);

    for (const std::string collection : {"environments", "agents", "interactions"}) {
        json duplicate = valid;
        duplicate[collection].push_back(duplicate[collection][0]);
        EXPECT_FALSE(world.importWorldData(duplicate.dump())) << collection;
        EXPECT_EQ(world.exportWorldData(), baseline) << collection;
    }
}

TEST_F(ElizasWorldTest, ImportPreservesCallbacksWithoutInvokingThem) {
    ElizasWorld source;
    populateCompleteWorld(source);

    int entered = 0;
    int left = 0;
    int interactions = 0;
    int environmentChanges = 0;
    world.onAgentEnterEnvironment([&](const std::string&, const std::string&) { ++entered; });
    world.onAgentLeaveEnvironment([&](const std::string&, const std::string&) { ++left; });
    world.onAgentInteraction([&](const std::string&, const std::string&) { ++interactions; });
    world.onEnvironmentChange([&](const std::string&, const std::string&) {
        ++environmentChanges;
    });

    ASSERT_TRUE(world.importWorldData(source.exportWorldData()));
    EXPECT_EQ(entered, 0);
    EXPECT_EQ(left, 0);
    EXPECT_EQ(interactions, 0);
    EXPECT_EQ(environmentChanges, 0);

    auto environment = world.getAllEnvironments().front();
    environment.description = "updated";
    EXPECT_TRUE(world.updateEnvironment(environment));
    EXPECT_EQ(environmentChanges, 1);

    WorldInteraction extra = world.getRecentInteractions().front();
    extra.id = "extra";
    EXPECT_TRUE(world.recordInteraction(extra));
    EXPECT_EQ(interactions, 1);

    EXPECT_TRUE(world.teleportAgent("agent-beta", {79.0, 89.0, 99.0}));
    EXPECT_EQ(left, 1);
    EXPECT_TRUE(world.teleportAgent("agent-beta", {5.0, 6.0, 7.0}));
    EXPECT_EQ(entered, 1);
}

TEST_F(ElizasWorldTest, FilePersistenceRejectsInvalidPathsAndInputWithoutMutation) {
    populateCompleteWorld(world);
    const std::string baseline = world.exportWorldData();
    ScopedPath missingParent(temporaryPath("elizas_world_missing_parent"));
    const auto child = missingParent.get() / "state.json";
    ScopedPath directory(temporaryPath("elizas_world_directory"));
    ASSERT_TRUE(std::filesystem::create_directory(directory.get()));

    EXPECT_FALSE(world.saveWorldState(""));
    EXPECT_FALSE(world.saveWorldState(child.string()));
    EXPECT_FALSE(world.saveWorldState(directory.get().string()));
    EXPECT_FALSE(world.loadWorldState(""));
    EXPECT_FALSE(world.loadWorldState(child.string()));
    EXPECT_FALSE(world.loadWorldState(directory.get().string()));
    EXPECT_EQ(world.exportWorldData(), baseline);

    ScopedPath malformedFile(temporaryPath("elizas_world_malformed"));
    {
        std::ofstream output(malformedFile.get(), std::ios::binary);
        ASSERT_TRUE(output.is_open());
        output << "{malformed";
    }
    EXPECT_FALSE(world.loadWorldState(malformedFile.get().string()));
    EXPECT_EQ(world.exportWorldData(), baseline);
}
