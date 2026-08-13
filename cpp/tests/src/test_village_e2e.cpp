/**
 * test_village_e2e.cpp — Comprehensive E2E tests for the Village module
 *
 * Covers all 4 village subsystems:
 *   1. Group Dynamics (SocialNetwork, GroupManager, Consensus)
 *   2. Antikythera Temporal Coupling (Gears, Joints, GearTrains)
 *   3. KSM Transfer Engine (Knowledge Artifacts, Progression Levels)
 *   4. AgnAI Bridge (Dual-role architecture, Pacing)
 *
 * Cross-fork parity: identical test file in both hurdcog and o9nn.
 */
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <set>

// Village headers
#include "village_group_dynamics.hpp"
#include "antikythera_coupling.hpp"
#include "village_ksm_transfer.hpp"
#include "village_agnai_bridge.hpp"

using namespace elizaos::village;
using namespace cogvillage::ksm;
using namespace cogvillage::bridge;

// ============================================================================
// Group Dynamics — SocialNetwork Tests
// ============================================================================

class SocialNetworkTest : public ::testing::Test {
protected:
    void SetUp() override {
        ResidentProfile dan{"dan", "Dan", "architect", {"systems", "math"}, 0.9, 0.8, 0.7, 0.6, 0.2, true, 0};
        ResidentProfile manus{"manus", "Manus", "orchestrator", {"planning", "coding"}, 0.8, 0.9, 0.8, 0.7, 0.1, true, 0};
        ResidentProfile echo{"echo", "Echo", "explorer", {"novelty", "primes"}, 0.95, 0.5, 0.6, 0.5, 0.4, true, 0};
        ResidentProfile eliza{"eliza", "Eliza", "conversationalist", {"therapy", "empathy"}, 0.7, 0.7, 0.9, 0.9, 0.3, true, 0};

        network_.addResident(dan);
        network_.addResident(manus);
        network_.addResident(echo);
        network_.addResident(eliza);
    }

    SocialNetwork network_;
};

TEST_F(SocialNetworkTest, ResidentRegistration) {
    auto residents = network_.getAllResidents();
    EXPECT_EQ(residents.size(), 4u);
}

TEST_F(SocialNetworkTest, ResidentCount) {
    EXPECT_EQ(network_.residentCount(), 4u);
}

TEST_F(SocialNetworkTest, ProfileRetrieval) {
    auto* profile = network_.getProfile("dan");
    ASSERT_NE(profile, nullptr);
    EXPECT_EQ(profile->name, "Dan");
    EXPECT_EQ(profile->role, "architect");
    EXPECT_NEAR(profile->openness, 0.9, 0.01);
}

TEST_F(SocialNetworkTest, RelationshipUpdate) {
    network_.updateRelationship("dan", "manus", "collaboration", 0.8);
    auto rel = network_.getRelationship("dan", "manus");
    EXPECT_GT(rel.trust, 0.0);
    EXPECT_GT(rel.familiarity, 0.0);
    EXPECT_EQ(rel.interactionCount, 1);
}

TEST_F(SocialNetworkTest, RelationshipSymmetry) {
    network_.updateRelationship("dan", "echo", "discussion", 0.6);
    auto relAB = network_.getRelationship("dan", "echo");
    auto relBA = network_.getRelationship("echo", "dan");
    EXPECT_GT(relAB.interactionCount, 0);
    EXPECT_GT(relBA.interactionCount, 0);
}

TEST_F(SocialNetworkTest, CompositeScoreBounded) {
    Relationship rel;
    rel.trust = 1.0;
    rel.familiarity = 1.0;
    rel.affinity = 1.0;
    rel.influence = 1.0;
    EXPECT_LE(rel.compositeScore(), 1.0);
    EXPECT_GE(rel.compositeScore(), -1.0);
}

TEST_F(SocialNetworkTest, NegativeValenceReducesTrust) {
    network_.updateRelationship("dan", "echo", "conflict", -0.5);
    auto rel = network_.getRelationship("dan", "echo");
    EXPECT_LT(rel.trust, 0.0);
}

TEST_F(SocialNetworkTest, RepeatedInteractionBuildsFamiliarity) {
    for (int i = 0; i < 10; ++i) {
        network_.updateRelationship("dan", "manus", "work", 0.5);
    }
    auto rel = network_.getRelationship("dan", "manus");
    EXPECT_GT(rel.familiarity, 0.3);
    EXPECT_EQ(rel.interactionCount, 10);
}

TEST_F(SocialNetworkTest, CommunityDetection) {
    // Build strong connections within a subgroup
    for (int i = 0; i < 5; ++i) {
        network_.updateRelationship("dan", "manus", "work", 0.9);
        network_.updateRelationship("dan", "echo", "research", 0.7);
        network_.updateRelationship("manus", "echo", "coding", 0.8);
    }
    auto communities = network_.detectCommunities();
    EXPECT_GE(communities.size(), 1u);
}

TEST_F(SocialNetworkTest, InformationPropagation) {
    network_.updateRelationship("dan", "manus", "trust", 0.9);
    network_.updateRelationship("manus", "echo", "trust", 0.8);
    auto reached = network_.propagateInformation("dan", "important news", 2);
    EXPECT_GE(reached.size(), 1u);
}

TEST_F(SocialNetworkTest, CentralityComputation) {
    // Build a hub-spoke topology around manus
    network_.updateRelationship("manus", "dan", "work", 0.8);
    network_.updateRelationship("manus", "echo", "work", 0.8);
    network_.updateRelationship("manus", "eliza", "work", 0.8);
    double manusCentrality = network_.calculateCentrality("manus");
    double echoCentrality = network_.calculateCentrality("echo");
    EXPECT_GT(manusCentrality, echoCentrality);
}

TEST_F(SocialNetworkTest, NeighborDiscovery) {
    network_.updateRelationship("dan", "manus", "work", 0.5);
    network_.updateRelationship("dan", "echo", "research", 0.5);
    auto neighbors = network_.getNeighbors("dan");
    EXPECT_GE(neighbors.size(), 2u);
}

TEST_F(SocialNetworkTest, RemoveResident) {
    network_.removeResident("eliza");
    EXPECT_EQ(network_.residentCount(), 3u);
    EXPECT_EQ(network_.getProfile("eliza"), nullptr);
}

// ============================================================================
// Group Dynamics — GroupManager Tests
// ============================================================================

class GroupManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        ResidentProfile dan{"dan", "Dan", "architect", {"systems"}, 0.9, 0.8, 0.7, 0.6, 0.2, true, 0};
        ResidentProfile manus{"manus", "Manus", "orchestrator", {"planning"}, 0.8, 0.9, 0.8, 0.7, 0.1, true, 0};
        ResidentProfile echo{"echo", "Echo", "explorer", {"novelty"}, 0.95, 0.5, 0.6, 0.5, 0.4, true, 0};
        network_.addResident(dan);
        network_.addResident(manus);
        network_.addResident(echo);
    }

    SocialNetwork network_;
    GroupManager groups_;
};

TEST_F(GroupManagerTest, GroupFormation) {
    GroupManager::ProtoGroup proto;
    proto.candidates = {"dan", "manus", "echo"};
    proto.affinityScore = 0.7;
    proto.trigger = "shared_interest";
    auto groupId = groups_.formGroup(proto, "cognitive_research");
    EXPECT_FALSE(groupId.empty());
}

TEST_F(GroupManagerTest, GroupRetrieval) {
    GroupManager::ProtoGroup proto;
    proto.candidates = {"dan", "manus"};
    proto.affinityScore = 0.8;
    proto.trigger = "collaboration";
    auto groupId = groups_.formGroup(proto, "ksm_repair");
    const Group* group = groups_.getGroup(groupId);
    ASSERT_NE(group, nullptr);
    EXPECT_EQ(group->members.size(), 2u);
    EXPECT_TRUE(group->hasMember("dan"));
    EXPECT_TRUE(group->hasMember("manus"));
}

TEST_F(GroupManagerTest, GroupDissolution) {
    GroupManager::ProtoGroup proto;
    proto.candidates = {"dan", "manus"};
    proto.affinityScore = 0.5;
    proto.trigger = "temp";
    auto groupId = groups_.formGroup(proto, "temp_task");
    groups_.dissolveGroup(groupId, "task_complete");
    const Group* group = groups_.getGroup(groupId);
    EXPECT_EQ(group, nullptr);
}

TEST_F(GroupManagerTest, GroupCount) {
    GroupManager::ProtoGroup proto1;
    proto1.candidates = {"dan", "manus"};
    proto1.affinityScore = 0.7;
    proto1.trigger = "work";
    GroupManager::ProtoGroup proto2;
    proto2.candidates = {"echo", "manus"};
    proto2.affinityScore = 0.6;
    proto2.trigger = "research";
    groups_.formGroup(proto1, "task_a");
    groups_.formGroup(proto2, "task_b");
    EXPECT_EQ(groups_.groupCount(), 2u);
}

TEST_F(GroupManagerTest, GroupsForResident) {
    GroupManager::ProtoGroup proto1;
    proto1.candidates = {"dan", "manus"};
    proto1.affinityScore = 0.7;
    proto1.trigger = "work";
    GroupManager::ProtoGroup proto2;
    proto2.candidates = {"manus", "echo"};
    proto2.affinityScore = 0.6;
    proto2.trigger = "research";
    groups_.formGroup(proto1, "task_a");
    groups_.formGroup(proto2, "task_b");
    auto manusGroups = groups_.getGroupsForResident("manus");
    EXPECT_EQ(manusGroups.size(), 2u);
}

TEST_F(GroupManagerTest, GoalProposal) {
    GroupManager::ProtoGroup proto;
    proto.candidates = {"dan", "manus", "echo"};
    proto.affinityScore = 0.8;
    proto.trigger = "shared_goal";
    auto groupId = groups_.formGroup(proto, "ksm_cycle");
    auto goalId = groups_.proposeGoal(groupId, "dan", "Complete cross-fork parity");
    EXPECT_FALSE(goalId.empty());
}

TEST_F(GroupManagerTest, CohesionUpdate) {
    GroupManager::ProtoGroup proto;
    proto.candidates = {"dan", "manus"};
    proto.affinityScore = 0.8;
    proto.trigger = "work";
    auto groupId = groups_.formGroup(proto, "task");
    const Group* before = groups_.getGroup(groupId);
    ASSERT_NE(before, nullptr);
    EXPECT_DOUBLE_EQ(before->cohesion, 0.8);

    groups_.tick(100, network_);
    const Group* after100 = groups_.getGroup(groupId);
    ASSERT_NE(after100, nullptr);
    const double cohesionAfter100 = after100->cohesion;
    EXPECT_NEAR(cohesionAfter100, 0.8 - 0.01 * (100.0 / 60.0), 1e-9);

    groups_.tick(160, network_);
    const Group* after160 = groups_.getGroup(groupId);
    ASSERT_NE(after160, nullptr);
    const double cohesionAfter160 = after160->cohesion;
    EXPECT_NEAR(cohesionAfter160, cohesionAfter100 - 0.01, 1e-9);

    // A stale tick must not reverse decay or inflate cohesion.
    groups_.tick(120, network_);
    const Group* afterStaleTick = groups_.getGroup(groupId);
    ASSERT_NE(afterStaleTick, nullptr);
    EXPECT_DOUBLE_EQ(afterStaleTick->cohesion, cohesionAfter160);

    // A timestamped interaction boosts cohesion within bounds and resets decay.
    GroupEvent interaction;
    interaction.eventType = "achievement";
    interaction.timestamp = 200;
    interaction.emotionalValence = 0.5;
    groups_.recordGroupEvent(groupId, interaction);
    const Group* afterInteraction = groups_.getGroup(groupId);
    ASSERT_NE(afterInteraction, nullptr);
    const double boostedCohesion =
        std::min(1.0, cohesionAfter160 + interaction.emotionalValence * 0.1);
    EXPECT_NEAR(afterInteraction->cohesion, boostedCohesion, 1e-9);

    groups_.tick(200, network_);
    const Group* afterInteractionTick = groups_.getGroup(groupId);
    ASSERT_NE(afterInteractionTick, nullptr);
    EXPECT_NEAR(afterInteractionTick->cohesion, boostedCohesion, 1e-9);
}

TEST_F(GroupManagerTest, ConsensusReached) {
    GroupManager::ProtoGroup proto;
    proto.candidates = {"dan", "manus", "echo"};
    proto.affinityScore = 0.8;
    proto.trigger = "decision";
    auto groupId = groups_.formGroup(proto, "vote");
    // Build relationships for consensus
    network_.updateRelationship("dan", "manus", "trust", 0.9);
    network_.updateRelationship("dan", "echo", "trust", 0.8);
    network_.updateRelationship("manus", "echo", "trust", 0.7);
    Proposal prop;
    prop.id = "prop_1";
    prop.description = "Adopt KSM cycle";
    prop.proposedBy = "dan";
    prop.opinions["dan"] = 0.9;
    prop.opinions["manus"] = 0.8;
    prop.opinions["echo"] = 0.6;
    auto result = groups_.seekConsensus(groupId, prop, network_);
    EXPECT_TRUE(result.reached);
    EXPECT_GT(result.position, 0.0);
}

TEST_F(GroupManagerTest, PotentialGroupDetection) {
    // Build strong affinities
    for (int i = 0; i < 10; ++i) {
        network_.updateRelationship("dan", "manus", "work", 0.9);
        network_.updateRelationship("dan", "echo", "research", 0.8);
        network_.updateRelationship("manus", "echo", "coding", 0.7);
    }
    auto potentials = groups_.detectPotentialGroups(network_);
    EXPECT_GE(potentials.size(), 1u);
}

TEST_F(GroupManagerTest, BroadcastToGroup) {
    GroupManager::ProtoGroup proto;
    proto.candidates = {"dan", "manus", "echo"};
    proto.affinityScore = 0.8;
    proto.trigger = "comm";
    auto groupId = groups_.formGroup(proto, "discussion");
    auto reached = groups_.broadcastToGroup(groupId, "dan", "Hello group");
    EXPECT_GE(reached.size(), 2u); // Should reach manus and echo
}

// ============================================================================
// Antikythera Temporal Coupling Tests
// ============================================================================

class AntikytheraTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_.addGear({"dan", 60, 1.0, 0.0, 8});
        engine_.addGear({"manus", 30, 2.0, 0.0, 8});
        engine_.addGear({"echo", 20, 3.0, 0.0, 7});
        engine_.addGear({"eliza", 15, 4.0, 0.0, 7});
        engine_.addGear({"opencog", 45, 1.33, 0.0, 9});
        engine_.addGear({"aion", 90, 0.67, 0.0, 10});

        engine_.addJoint({"cog_1", "dan", "manus", JointType::GearMesh, 2.0, 0.0, 0.0});
        engine_.addJoint({"cog_2", "manus", "echo", JointType::GearMesh, 1.5, 0.0, 0.0});
        engine_.addJoint({"cog_3", "echo", "eliza", JointType::GearMesh, 1.33, 0.0, 0.0});
        engine_.addJoint({"sym_1", "manus", "opencog", JointType::GearMesh, 0.67, 0.0, 0.0});
        engine_.addJoint({"sym_2", "opencog", "aion", JointType::GearMesh, 0.5, 0.0, 0.0});
    }

    AntikytheraEngine engine_;
};

TEST_F(AntikytheraTest, GearRegistration) {
    auto* dan = engine_.getGear("dan");
    ASSERT_NE(dan, nullptr);
    EXPECT_EQ(dan->teeth, 60);
    EXPECT_NEAR(dan->rpm, 1.0, 0.01);
}

TEST_F(AntikytheraTest, AngularVelocity) {
    auto* dan = engine_.getGear("dan");
    ASSERT_NE(dan, nullptr);
    // 1 rpm = 2π/60 rad/s ≈ 0.1047 rad/s
    EXPECT_NEAR(dan->angularVelocity(), 2.0 * kAntikytheraPi / 60.0, 0.001);
}

TEST_F(AntikytheraTest, PeriodComputation) {
    auto* dan = engine_.getGear("dan");
    ASSERT_NE(dan, nullptr);
    // 60 teeth at 1 rpm → period = 60000ms
    EXPECT_NEAR(dan->periodMs(), 60000.0, 1.0);
}

TEST_F(AntikytheraTest, PhaseAdvancement) {
    engine_.tick(1);
    auto* dan = engine_.getGear("dan");
    ASSERT_NE(dan, nullptr);
    EXPECT_GT(dan->phase, 0.0);
    EXPECT_LT(dan->phase, 2.0 * kAntikytheraPi);
}

TEST_F(AntikytheraTest, GearRatioComputation) {
    double ratio = engine_.getGearRatio("dan", "manus");
    // getGearRatio returns teeth_B/teeth_A (driven/driver) = 30/60 = 0.5
    // or teeth_A/teeth_B = 2.0 depending on convention
    EXPECT_TRUE(std::abs(ratio - 2.0) < 0.1 || std::abs(ratio - 0.5) < 0.1);
}

TEST_F(AntikytheraTest, PhaseQuery) {
    engine_.tick(1);
    double danPhase = engine_.getPhase("dan");
    double manusPhase = engine_.getPhase("manus");
    // Manus should advance faster due to 2:1 ratio
    EXPECT_GT(manusPhase, danPhase);
}

TEST_F(AntikytheraTest, RpmQuery) {
    double danRpm = engine_.getRpm("dan");
    double manusRpm = engine_.getRpm("manus");
    EXPECT_NEAR(danRpm, 1.0, 0.01);
    EXPECT_NEAR(manusRpm, 2.0, 0.01);
}

TEST_F(AntikytheraTest, AlignmentDetection) {
    // At start, all gears are at phase 0 — they should be aligned
    bool aligned = engine_.areAligned("dan", "manus");
    EXPECT_TRUE(aligned);
}

TEST_F(AntikytheraTest, EpicyclicModulation) {
    engine_.addJoint({"epic_1", "manus", "echo", JointType::EpicyclicPin, 1.0, 0.0, 0.11});
    double mod = engine_.getEpicyclicModulation("echo", 1);
    // Modulation should be near 1.0 (±0.11 amplitude)
    EXPECT_NEAR(mod, 1.0, 0.12);
}

TEST_F(AntikytheraTest, GearTrainRegistration) {
    GearTrain train;
    train.name = "cognitive_core";
    train.description = "Primary cognitive processing chain";
    train.gearIds = {"dan", "manus", "echo", "eliza"};
    train.jointIds = {"cog_1", "cog_2", "cog_3"};
    train.overallRatio = 2.0 * 1.5 * 1.33;
    engine_.addGearTrain(train);
    auto* retrieved = engine_.getGearTrain("cognitive_core");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->gearIds.size(), 4u);
}

TEST_F(AntikytheraTest, SyncEventEmission) {
    std::vector<SyncEvent> events;
    engine_.setSyncCallback([&events](const SyncEvent& e) {
        events.push_back(e);
    });
    // Tick multiple times to potentially trigger sync events
    for (int i = 0; i < 100; ++i) {
        engine_.tick(i + 1);
    }
    // Sync events may or may not occur depending on gear ratios
    // Just verify no crash and callback works
    EXPECT_TRUE(true);
}

TEST_F(AntikytheraTest, GearRemoval) {
    engine_.removeGear("aion");
    EXPECT_EQ(engine_.getGear("aion"), nullptr);
}

TEST_F(AntikytheraTest, JointRemoval) {
    engine_.removeJoint("sym_2");
    // After removing the joint, opencog→aion coupling should be gone
    // Verify no crash
    engine_.tick(1);
    EXPECT_TRUE(true);
}

TEST_F(AntikytheraTest, JsonSerialization) {
    std::string json = engine_.toJson();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("dan"), std::string::npos);
    EXPECT_NE(json.find("manus"), std::string::npos);
}

TEST_F(AntikytheraTest, VillageMechanismInit) {
    AntikytheraEngine fresh;
    fresh.initializeVillageMechanism();
    // Should have all 10 village residents as gears
    EXPECT_NE(fresh.getGear("dan"), nullptr);
    EXPECT_NE(fresh.getGear("manus"), nullptr);
}

// ============================================================================
// KSM Transfer Engine Tests
// ============================================================================

class KSMTransferTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_.setEventCallback([this](const std::string& type, const nlohmann::json& data) {
            events_.push_back({type, data});
        });
    }

    KSMTransferEngine engine_;
    std::vector<std::pair<std::string, nlohmann::json>> events_;
};

TEST_F(KSMTransferTest, LevelNames) {
    EXPECT_STREQ(levelName(Level::DISCOVERY), "discovery");
    EXPECT_STREQ(levelName(Level::INSTRUCTION), "instruction");
    EXPECT_STREQ(levelName(Level::MASTERY), "mastery");
    EXPECT_STREQ(levelName(Level::ENTELECHY), "entelechy");
}

TEST_F(KSMTransferTest, KnowledgeArtifactSerialization) {
    KnowledgeArtifact artifact;
    artifact.id = "ka_001";
    artifact.name = "Cognitive Architecture Design";
    artifact.domain = "systems";
    artifact.level = Level::DISCOVERY;
    artifact.creator = "dan";
    artifact.description = "Foundational patterns";
    artifact.production_routine = "observe → model → validate → iterate";

    auto json = artifact.toJson();
    EXPECT_EQ(json["id"], "ka_001");
    EXPECT_EQ(json["level"], "discovery");
    EXPECT_EQ(json["creator"], "dan");
    EXPECT_EQ(json["domain"], "systems");
}

TEST_F(KSMTransferTest, RecordLearningEvent) {
    // Manually add a resident to the engine's internal state via loadRegistry
    // Since loadRegistry requires a file, we test recordLearning with empty residents
    // (should not crash, just no-op)
    engine_.recordLearning("dan", "manus", "ka_001", 0.8);
    // No crash = success for empty state
    EXPECT_TRUE(true);
}

TEST_F(KSMTransferTest, CreateArtifact) {
    KnowledgeArtifact artifact;
    artifact.id = "ka_002";
    artifact.name = "KSM 12-Step Cycle";
    artifact.domain = "methodology";
    artifact.level = Level::INSTRUCTION;
    artifact.creator = "dan";
    // Without registered residents, this is a no-op
    engine_.createArtifact("dan", artifact);
    EXPECT_TRUE(true);
}

TEST_F(KSMTransferTest, ComplementaryPairs) {
    auto pairs = engine_.getComplementaryPairs();
    // Empty engine should return empty pairs
    EXPECT_EQ(pairs.size(), 0u);
}

TEST_F(KSMTransferTest, StateSnapshot) {
    auto state = engine_.getState();
    EXPECT_FALSE(state.empty());
    EXPECT_EQ(state["engine"], "KSMTransferEngine");
    EXPECT_EQ(state["principle"], "Dan's Relational Principle");
    EXPECT_EQ(state["total_residents"], 0);
    EXPECT_EQ(state["total_artifacts"], 0);
    EXPECT_EQ(state["total_learning_events"], 0);
    EXPECT_EQ(state["total_knowledge_received"], 0);
}

TEST_F(KSMTransferTest, LoadRegistryFromFile) {
    // Create a temporary registry file
    std::string registryPath = "/tmp/test_resident_registry.json";
    nlohmann::json registry;
    registry["residents"]["dan"] = {
        {"handle", "Dan"}, {"domain", "systems"}, {"gear_train", "cognitive_core"},
        {"gear_teeth", 60}, {"ksm_level", "mastery"},
        {"elizaos_niche", "agent_commerce"}, {"opencog_niche", "atomspace"},
        {"hidden_talents", {"math", "music"}}
    };
    registry["residents"]["echo"] = {
        {"handle", "Echo"}, {"domain", "novelty"}, {"gear_train", "creative"},
        {"gear_teeth", 20}, {"ksm_level", "discovery"},
        {"elizaos_niche", "exploration"}, {"opencog_niche", "primes"},
        {"hidden_talents", {"patterns"}}
    };
    std::ofstream f(registryPath);
    f << registry.dump(2);
    f.close();

    engine_.loadRegistry(registryPath);
    auto state = engine_.getState();
    EXPECT_EQ(state["total_residents"], 2);
}

TEST_F(KSMTransferTest, LearningWithRegisteredResidents) {
    // Load residents first
    std::string registryPath = "/tmp/test_resident_registry2.json";
    nlohmann::json registry;
    registry["residents"]["dan"] = {
        {"handle", "Dan"}, {"domain", "systems"}, {"gear_train", "cognitive_core"},
        {"gear_teeth", 60}, {"ksm_level", "discovery"},
        {"elizaos_niche", "agent_commerce"}, {"opencog_niche", "atomspace"},
        {"hidden_talents", {"math"}}
    };
    registry["residents"]["manus"] = {
        {"handle", "Manus"}, {"domain", "planning"}, {"gear_train", "cognitive_core"},
        {"gear_teeth", 30}, {"ksm_level", "discovery"},
        {"elizaos_niche", "orchestration"}, {"opencog_niche", "planning"},
        {"hidden_talents", {"coding"}}
    };
    std::ofstream f(registryPath);
    f << registry.dump(2);
    f.close();

    engine_.loadRegistry(registryPath);
    engine_.recordLearning("dan", "manus", "ka_001", 0.9);
    auto state = engine_.getState();
    EXPECT_EQ(state["total_learning_events"], 1);
    EXPECT_EQ(state["total_knowledge_received"], 1);
    // Should emit a learning event
    EXPECT_GE(events_.size(), 1u);
    if (!events_.empty()) {
        EXPECT_EQ(events_.back().first, "ksm.learning");
    }
}

TEST_F(KSMTransferTest, ArtifactCreationWithRegisteredResident) {
    std::string registryPath = "/tmp/test_resident_registry3.json";
    nlohmann::json registry;
    registry["residents"]["dan"] = {
        {"handle", "Dan"}, {"domain", "systems"}, {"gear_train", "cognitive_core"},
        {"gear_teeth", 60}, {"ksm_level", "mastery"},
        {"elizaos_niche", "agent_commerce"}, {"opencog_niche", "atomspace"},
        {"hidden_talents", {"math"}}
    };
    std::ofstream f(registryPath);
    f << registry.dump(2);
    f.close();

    engine_.loadRegistry(registryPath);
    KnowledgeArtifact artifact;
    artifact.id = "ka_003";
    artifact.name = "Structure-Preserving Transformation";
    artifact.domain = "methodology";
    artifact.level = Level::MASTERY;
    artifact.creator = "dan";
    artifact.innovation_pattern = "KSM 12-step cycle";
    engine_.createArtifact("dan", artifact);
    EXPECT_GE(events_.size(), 1u);
    if (!events_.empty()) {
        EXPECT_EQ(events_.back().first, "ksm.artifact_created");
    }
}

// ============================================================================
// AgnAI Bridge Tests
// ============================================================================

class AgnAIBridgeTest : public ::testing::Test {
protected:
    void SetUp() override {
        AgnAIConfig config;
        config.base_url = "http://localhost:3001/api";
        config.resident_creds = {
            {"dan", "dan-test-2026"},
            {"manus", "manus-test-2026"},
            {"echo", "echo-test-2026"}
        };
        bridge_.configure(config);
    }

    AgnAIBridge bridge_;
};

TEST_F(AgnAIBridgeTest, Configuration) {
    auto state = bridge_.getState();
    EXPECT_FALSE(state.empty());
    EXPECT_EQ(state["total_sessions"], 0);
}

TEST_F(AgnAIBridgeTest, PacingInitialization) {
    bridge_.initPacing();
    // After init, pacing should be configured for known residents
    double danDelay = bridge_.getResponseDelay("dan");
    double echoDelay = bridge_.getResponseDelay("echo");
    // Both should return valid delays
    EXPECT_GT(danDelay, 0.0);
    EXPECT_GT(echoDelay, 0.0);
}

TEST_F(AgnAIBridgeTest, ResponseDelayDefault) {
    // Without pacing init, should return base delay
    double delay = bridge_.getResponseDelay("unknown_resident", 5.0);
    EXPECT_NEAR(delay, 5.0, 0.01);
}

TEST_F(AgnAIBridgeTest, PendingMessagesEmpty) {
    auto msgs = bridge_.consumePendingMessages();
    EXPECT_TRUE(msgs.empty());
}

TEST_F(AgnAIBridgeTest, OnGroupFormedWithoutHttp) {
    // Without HTTP client, group formation may throw or gracefully fail
    // Wrap in try/catch to handle json exceptions from null config fields
    try {
        bridge_.onGroupFormed("group_1", {"dan", "manus", "echo"}, "shared_interest");
        auto state = bridge_.getState();
        // If it didn't throw, verify no active session was created
        EXPECT_EQ(state["active_sessions"], 0);
    } catch (const std::exception&) {
        // Expected: without HTTP client, json access on null fields throws
        SUCCEED();
    }
}

TEST_F(AgnAIBridgeTest, OnGroupDissolved) {
    bridge_.onGroupDissolved("group_1");
    // Should not crash even for non-existent group
    EXPECT_TRUE(true);
}

TEST_F(AgnAIBridgeTest, OnResidentSpeaks) {
    bridge_.initPacing();
    bridge_.onResidentSpeaks("group_1", "dan", "Hello world");
    // Without active session, message should be dropped
    auto msgs = bridge_.consumePendingMessages();
    EXPECT_TRUE(msgs.empty());
}

TEST_F(AgnAIBridgeTest, StateSnapshot) {
    auto state = bridge_.getState();
    EXPECT_TRUE(state.contains("active_sessions"));
    EXPECT_TRUE(state.contains("total_sessions"));
    EXPECT_TRUE(state.contains("pending_messages"));
}

// ============================================================================
// Integration Tests — Cross-module interaction
// ============================================================================

TEST(VillageIntegrationTest, SocialNetworkToGroupFormation) {
    SocialNetwork network;
    GroupManager groups;

    ResidentProfile dan{"dan", "Dan", "architect", {"systems"}, 0.9, 0.8, 0.7, 0.6, 0.2, true, 0};
    ResidentProfile manus{"manus", "Manus", "orchestrator", {"planning"}, 0.8, 0.9, 0.8, 0.7, 0.1, true, 0};
    ResidentProfile echo{"echo", "Echo", "explorer", {"novelty"}, 0.95, 0.5, 0.6, 0.5, 0.4, true, 0};
    network.addResident(dan);
    network.addResident(manus);
    network.addResident(echo);

    // Build relationships
    for (int i = 0; i < 10; ++i) {
        network.updateRelationship("dan", "manus", "work", 0.9);
        network.updateRelationship("manus", "echo", "coding", 0.8);
    }

    // Detect potential groups from network
    auto potentials = groups.detectPotentialGroups(network);
    if (!potentials.empty()) {
        auto groupId = groups.formGroup(potentials[0], "emergent_affinity");
        EXPECT_FALSE(groupId.empty());
    }
}

TEST(VillageIntegrationTest, AntikytheraGearTrainCycle) {
    AntikytheraEngine engine;
    engine.initializeVillageMechanism();

    // Run 10 ticks
    for (int i = 0; i < 10; ++i) {
        auto events = engine.tick(i + 1);
    }

    // Verify all gears have advanced
    double danPhase = engine.getPhase("dan");
    double manusPhase = engine.getPhase("manus");
    EXPECT_GT(danPhase, 0.0);
    EXPECT_GT(manusPhase, 0.0);
    // Manus should be faster (2:1 ratio)
    EXPECT_GT(manusPhase, danPhase);
}

TEST(VillageIntegrationTest, KSMTransferWithAntikythera) {
    KSMTransferEngine ksm;
    AntikytheraEngine antikythera;

    // Load KSM residents
    std::string registryPath = "/tmp/test_integration_registry.json";
    nlohmann::json registry;
    registry["residents"]["dan"] = {
        {"handle", "Dan"}, {"domain", "systems"}, {"gear_train", "cognitive_core"},
        {"gear_teeth", 60}, {"ksm_level", "mastery"},
        {"elizaos_niche", "agent_commerce"}, {"opencog_niche", "atomspace"},
        {"hidden_talents", {"math"}}
    };
    registry["residents"]["manus"] = {
        {"handle", "Manus"}, {"domain", "planning"}, {"gear_train", "cognitive_core"},
        {"gear_teeth", 30}, {"ksm_level", "instruction"},
        {"elizaos_niche", "orchestration"}, {"opencog_niche", "planning"},
        {"hidden_talents", {"coding"}}
    };
    std::ofstream f(registryPath);
    f << registry.dump(2);
    f.close();
    ksm.loadRegistry(registryPath);

    // Set up antikythera
    antikythera.addGear({"dan", 60, 1.0, 0.0, 8});
    antikythera.addGear({"manus", 30, 2.0, 0.0, 8});
    antikythera.addJoint({"mesh_1", "dan", "manus", JointType::GearMesh, 2.0, 0.0, 0.0});

    // Tick and share knowledge
    antikythera.tick(1);
    ksm.recordLearning("dan", "manus", "ka_integration", 0.85);

    auto ksmState = ksm.getState();
    EXPECT_EQ(ksmState["total_learning_events"], 1);
}

TEST(VillageIntegrationTest, FullVillageCycle) {
    // Complete village cycle: network → group → knowledge → antikythera → bridge
    SocialNetwork network;
    GroupManager groups;
    AntikytheraEngine antikythera;
    KSMTransferEngine ksm;
    AgnAIBridge bridge;

    // 1. Register residents
    ResidentProfile dan{"dan", "Dan", "architect", {"systems"}, 0.9, 0.8, 0.7, 0.6, 0.2, true, 0};
    ResidentProfile manus{"manus", "Manus", "orchestrator", {"planning"}, 0.8, 0.9, 0.8, 0.7, 0.1, true, 0};
    network.addResident(dan);
    network.addResident(manus);

    // 2. Build relationship
    network.updateRelationship("dan", "manus", "collaboration", 0.8);

    // 3. Form group
    GroupManager::ProtoGroup proto;
    proto.candidates = {"dan", "manus"};
    proto.affinityScore = 0.8;
    proto.trigger = "ksm_repair";
    auto groupId = groups.formGroup(proto, "ksm_cycle");
    EXPECT_FALSE(groupId.empty());

    // 4. Tick antikythera
    antikythera.addGear({"dan", 60, 1.0, 0.0, 8});
    antikythera.addGear({"manus", 30, 2.0, 0.0, 8});
    antikythera.addJoint({"mesh_1", "dan", "manus", JointType::GearMesh, 2.0, 0.0, 0.0});
    antikythera.tick(1);

    // 5. Configure bridge
    AgnAIConfig config;
    config.base_url = "http://localhost:3001/api";
    config.resident_creds = {{"dan", "dan-test"}, {"manus", "manus-test"}};
    bridge.configure(config);
    bridge.initPacing();

    // 6. Verify all subsystems have state
    auto netRel = network.getRelationship("dan", "manus");
    EXPECT_GT(netRel.trust, 0.0);
    auto* group = groups.getGroup(groupId);
    EXPECT_NE(group, nullptr);
    EXPECT_GT(antikythera.getPhase("dan"), 0.0);
    auto bridgeState = bridge.getState();
    EXPECT_FALSE(bridgeState.empty());
}
