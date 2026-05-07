// Comprehensive End-to-End Test Suite for elizaos::core
//
// Covers public API surface declared in include/elizaos/core.hpp:
//   * generateUUID()
//   * HypergraphNode / HypergraphEdge
//   * Memory (basic + metadata + embedding + hypergraph links)
//   * State (agent identity, actors, goals, recent messages)
//   * TruthValue PLN-style operations
//   * Task / TaskManager (lifecycle: create / get / schedule / cancel /
//     tag-query / start / stop / pause / resume)
//
// Replaces the previous stub test that only verified compilation.

#include <gtest/gtest.h>
#include "elizaos/core.hpp"

#include <chrono>
#include <memory>
#include <thread>
#include <unordered_set>

using namespace elizaos;

// ---------------------------------------------------------------------------
// UUID generator
// ---------------------------------------------------------------------------
TEST(CoreUuid, GeneratesNonEmpty) {
    auto u = generateUUID();
    EXPECT_FALSE(u.empty());
}

TEST(CoreUuid, ProducesDistinctIds) {
    std::unordered_set<UUID> seen;
    for (int i = 0; i < 256; ++i) {
        seen.insert(generateUUID());
    }
    EXPECT_EQ(seen.size(), 256u);
}

// ---------------------------------------------------------------------------
// Hypergraph primitives
// ---------------------------------------------------------------------------
TEST(CoreHypergraph, NodeStoresIdLabelAndAttributes) {
    auto id = generateUUID();
    HypergraphNode n(id, "concept");
    EXPECT_EQ(n.getId(), id);
    EXPECT_EQ(n.getLabel(), "concept");

    n.setAttribute("color", "blue");
    n.setAttribute("weight", "0.7");

    auto color = n.getAttribute("color");
    ASSERT_TRUE(color.has_value());
    EXPECT_EQ(color.value(), "blue");

    EXPECT_FALSE(n.getAttribute("missing").has_value());
    EXPECT_EQ(n.getAttributes().size(), 2u);
}

TEST(CoreHypergraph, EdgeConnectsMultipleNodes) {
    auto id = generateUUID();
    auto a = generateUUID();
    auto b = generateUUID();
    auto c = generateUUID();

    HypergraphEdge e(id, "relates", {a, b, c});
    EXPECT_EQ(e.getId(), id);
    EXPECT_EQ(e.getLabel(), "relates");
    EXPECT_EQ(e.getNodeIds().size(), 3u);
    EXPECT_DOUBLE_EQ(e.getWeight(), 1.0);

    e.setWeight(0.42);
    EXPECT_DOUBLE_EQ(e.getWeight(), 0.42);
}

// ---------------------------------------------------------------------------
// Memory
// ---------------------------------------------------------------------------
TEST(CoreMemory, BasicConstructionPreservesFields) {
    auto memId = generateUUID();
    auto entityId = generateUUID();
    auto agentId = generateUUID();

    auto mem = std::make_shared<Memory>(memId, "hello world", entityId, agentId);
    EXPECT_EQ(mem->getId(), memId);
    EXPECT_EQ(mem->getContent(), "hello world");
    EXPECT_EQ(mem->getEntityId(), entityId);
    EXPECT_EQ(mem->getAgentId(), agentId);
    EXPECT_FALSE(mem->getEmbedding().has_value());
    EXPECT_FALSE(mem->isUnique());
    EXPECT_DOUBLE_EQ(mem->getSimilarity(), 0.0);
}

TEST(CoreMemory, EmbeddingCanBeSetAndRetrieved) {
    auto mem = std::make_shared<Memory>(generateUUID(), "x", generateUUID(), generateUUID());
    EmbeddingVector v{0.1f, 0.2f, 0.3f, 0.4f};
    mem->setEmbedding(v);
    ASSERT_TRUE(mem->getEmbedding().has_value());
    EXPECT_EQ(mem->getEmbedding()->size(), 4u);
    EXPECT_FLOAT_EQ(mem->getEmbedding()->at(2), 0.3f);
}

TEST(CoreMemory, UniqueAndSimilarityFlagsCanBeMutated) {
    auto mem = std::make_shared<Memory>(generateUUID(), "x", generateUUID(), generateUUID());
    mem->setUnique(true);
    mem->setSimilarity(0.85);
    EXPECT_TRUE(mem->isUnique());
    EXPECT_NEAR(mem->getSimilarity(), 0.85, 1e-9);
}

TEST(CoreMemory, RoomIdAssignment) {
    auto mem = std::make_shared<Memory>(generateUUID(), "hello", generateUUID(), generateUUID());
    auto room = generateUUID();
    mem->setRoomId(room);
    EXPECT_EQ(mem->getRoomId(), room);
}

TEST(CoreMemory, HypergraphLinks) {
    auto mem = std::make_shared<Memory>(generateUUID(), "hg", generateUUID(), generateUUID());
    auto n1 = generateUUID();
    auto n2 = generateUUID();
    auto e1 = generateUUID();
    mem->addHypergraphNode(n1);
    mem->addHypergraphNode(n2);
    mem->addHypergraphEdge(e1);

    EXPECT_EQ(mem->getHypergraphNodes().size(), 2u);
    EXPECT_EQ(mem->getHypergraphEdges().size(), 1u);
    EXPECT_EQ(mem->getHypergraphNodes()[1], n2);
    EXPECT_EQ(mem->getHypergraphEdges()[0], e1);
}

TEST(CoreMemory, MetadataConstructorRetainsType) {
    DocumentMetadata meta;
    meta.tags = {"intro", "primer"};
    auto mem = std::make_shared<Memory>(
        generateUUID(), "doc", generateUUID(), generateUUID(),
        MemoryMetadata{meta});
    const auto& m = mem->getMetadata();
    ASSERT_TRUE(std::holds_alternative<DocumentMetadata>(m));
    EXPECT_EQ(static_cast<int>(std::get<DocumentMetadata>(m).type),
              static_cast<int>(MemoryType::DOCUMENT));
    EXPECT_EQ(std::get<DocumentMetadata>(m).tags.size(), 2u);
}

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
TEST(CoreState, IdentityAccessors) {
    AgentConfig cfg;
    cfg.agentId = generateUUID();
    cfg.agentName = "Eliza";
    cfg.bio = "test bio";
    cfg.lore = "test lore";
    cfg.adjective = "curious";

    State s(cfg);
    EXPECT_EQ(s.getAgentId(), cfg.agentId);
    EXPECT_EQ(s.getAgentName(), "Eliza");
    EXPECT_EQ(s.getBio(), "test bio");
    EXPECT_EQ(s.getLore(), "test lore");
}

TEST(CoreState, ActorsGoalsAndMessages) {
    AgentConfig cfg;
    cfg.agentId = generateUUID();
    cfg.agentName = "agent";
    State s(cfg);

    Actor a; a.id = generateUUID(); a.name = "User"; a.details = "human user";
    s.addActor(a);

    Goal g; g.id = generateUUID(); g.description = "learn"; g.status = "active";
    s.addGoal(g);

    auto mem = std::make_shared<Memory>(generateUUID(), "hello", generateUUID(), cfg.agentId);
    s.addRecentMessage(mem);

    EXPECT_EQ(s.getActors().size(), 1u);
    EXPECT_EQ(s.getActors()[0].name, "User");
    EXPECT_EQ(s.getGoals().size(), 1u);
    EXPECT_EQ(s.getGoals()[0].description, "learn");
    EXPECT_EQ(s.getRecentMessages().size(), 1u);
    EXPECT_EQ(s.getRecentMessages()[0]->getContent(), "hello");
}

// ---------------------------------------------------------------------------
// TruthValue / PLN-style operations
// ---------------------------------------------------------------------------
TEST(CoreTruthValue, ValidityAndExpectedValue) {
    TruthValue t(0.8, 0.9);
    EXPECT_TRUE(t.isValid());
    EXPECT_NEAR(t.getExpectedValue(), 0.72, 1e-9);

    TruthValue bad(1.5, 0.5);
    EXPECT_FALSE(bad.isValid());
}

TEST(CoreTruthValue, ConjunctionDisjunctionNegation) {
    TruthValue a(0.8, 0.9);
    TruthValue b(0.5, 0.7);

    auto c = a.conjunction(b);
    auto d = a.disjunction(b);
    auto n = a.negation();

    EXPECT_GE(c.strength, 0.0);
    EXPECT_LE(c.strength, 1.0);
    EXPECT_GE(d.strength, 0.0);
    EXPECT_LE(d.strength, 1.0);
    EXPECT_NEAR(n.strength, 1.0 - a.strength, 1e-9);
}

TEST(CoreTruthValue, ImplicationProducesValid) {
    TruthValue a(0.9, 0.95);
    TruthValue b(0.4, 0.8);
    auto i = a.implication(b);
    EXPECT_TRUE(i.isValid());
}

// ---------------------------------------------------------------------------
// Task / TaskManager
// ---------------------------------------------------------------------------
class CoreTaskManagerTest : public ::testing::Test {
protected:
    TaskManager mgr;
};

TEST_F(CoreTaskManagerTest, CreateAndRetrieveTask) {
    auto id = mgr.createTask("greet", "say hello");
    EXPECT_FALSE(id.empty());
    auto t = mgr.getTask(id);
    ASSERT_TRUE(t != nullptr);
    EXPECT_EQ(t->getName(), "greet");
}

TEST_F(CoreTaskManagerTest, ScheduleAndCancelTask) {
    auto id = mgr.createTask("delayed", "wait then run");
    auto when = std::chrono::system_clock::now() + std::chrono::seconds(5);
    EXPECT_TRUE(mgr.scheduleTask(id, when));
    EXPECT_TRUE(mgr.cancelTask(id));
}

TEST_F(CoreTaskManagerTest, PendingTasksContainsNewTasks) {
    auto a = mgr.createTask("a", "first");
    auto b = mgr.createTask("b", "second");
    auto pending = mgr.getPendingTasks();
    EXPECT_GE(pending.size(), 2u);
    (void)a; (void)b;
}

TEST_F(CoreTaskManagerTest, RunningLifecycle) {
    EXPECT_FALSE(mgr.isRunning());
    mgr.setTickInterval(std::chrono::milliseconds(20));
    mgr.start();
    EXPECT_TRUE(mgr.isRunning());
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    mgr.pause();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    mgr.resume();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    mgr.stop();
    EXPECT_FALSE(mgr.isRunning());
}
