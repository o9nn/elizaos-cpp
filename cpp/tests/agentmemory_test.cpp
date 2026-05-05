// agentmemory_test.cpp
// End-to-end unit tests for AgentMemoryManager + AttentionAllocator +
// AttentionAwareMemoryManager. Replaces the previous boilerplate stub.

#include "elizaos/agentmemory.hpp"
#include "elizaos/attention.hpp"
#include "elizaos/core.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <thread>
#include <unordered_set>

using namespace elizaos;

static std::shared_ptr<Memory> makeMemory(const std::string& id,
                                   const std::string& content,
                                   const std::string& room = "room-1",
                                   bool unique = false) {
    auto m = std::make_shared<Memory>(id, content,
                                      /*entityId=*/std::string("entity-A"),
                                      /*agentId=*/std::string("agent-A"));
    m->setRoomId(room);
    m->setUnique(unique);
    return m;
}

class AgentMemoryFixture : public ::testing::Test {
protected:
    void SetUp() override { mgr_ = std::make_shared<AgentMemoryManager>(); }
    std::shared_ptr<AgentMemoryManager> mgr_;
};

TEST_F(AgentMemoryFixture, CreateRetrieveDelete) {
    auto m = makeMemory("mem-1", "Hello world");
    auto id = mgr_->createMemory(m);
    EXPECT_EQ(id, "mem-1");
    auto got = mgr_->getMemoryById("mem-1");
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->getContent(), "Hello world");
    EXPECT_EQ(got->getRoomId(), "room-1");
    EXPECT_TRUE(mgr_->deleteMemory("mem-1"));
    EXPECT_EQ(mgr_->getMemoryById("mem-1"), nullptr);
    EXPECT_FALSE(mgr_->deleteMemory("does-not-exist"));
}

TEST_F(AgentMemoryFixture, UpdateMemory) {
    mgr_->createMemory(makeMemory("mem-2", "first"));
    auto m2 = makeMemory("mem-2", "second");
    EXPECT_TRUE(mgr_->updateMemory(m2));
    auto got = mgr_->getMemoryById("mem-2");
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->getContent(), "second");
}

TEST_F(AgentMemoryFixture, MultiTableIsolation) {
    mgr_->createMemory(makeMemory("a-1", "alpha"), "tableA");
    mgr_->createMemory(makeMemory("b-1", "beta"),  "tableB");
    MemorySearchParams pa; pa.tableName = "tableA"; pa.count = 50;
    MemorySearchParams pb; pb.tableName = "tableB"; pb.count = 50;
    EXPECT_EQ(mgr_->getMemories(pa).size(), 1u);
    EXPECT_EQ(mgr_->getMemories(pb).size(), 1u);
}

TEST_F(AgentMemoryFixture, GetMemoriesByIds) {
    for (int i = 0; i < 5; ++i) {
        mgr_->createMemory(makeMemory("m-" + std::to_string(i),
                                      "content-" + std::to_string(i)));
    }
    auto got = mgr_->getMemoriesByIds({"m-0", "m-2", "m-4"});
    EXPECT_EQ(got.size(), 3u);
}

TEST_F(AgentMemoryFixture, CountMemoriesPerRoom) {
    mgr_->createMemory(makeMemory("r1-a", "x", "room-X"));
    mgr_->createMemory(makeMemory("r1-b", "y", "room-X"));
    mgr_->createMemory(makeMemory("r2-a", "z", "room-Y"));
    EXPECT_EQ(mgr_->countMemories("room-X"), 2);
    EXPECT_EQ(mgr_->countMemories("room-Y"), 1);
    EXPECT_EQ(mgr_->countMemories("nonexistent"), 0);
}

TEST_F(AgentMemoryFixture, DeleteAllMemoriesByRoom) {
    mgr_->createMemory(makeMemory("d-1", "x", "kill"));
    mgr_->createMemory(makeMemory("d-2", "y", "kill"));
    mgr_->createMemory(makeMemory("k-1", "z", "keep"));
    mgr_->deleteAllMemories("kill");
    EXPECT_EQ(mgr_->countMemories("kill"), 0);
    EXPECT_EQ(mgr_->countMemories("keep"), 1);
}

TEST_F(AgentMemoryFixture, GetMemoriesByMultipleRooms) {
    mgr_->createMemory(makeMemory("a", "1", "rA"));
    mgr_->createMemory(makeMemory("b", "2", "rB"));
    mgr_->createMemory(makeMemory("c", "3", "rC"));
    EXPECT_EQ(mgr_->getMemoriesByRoomIds({"rA", "rC"}).size(), 2u);
}

TEST_F(AgentMemoryFixture, SearchByEmbeddingReturnsRanked) {
    auto a = makeMemory("emb-a", "vec a"); a->setEmbedding({1.0, 0.0, 0.0});
    auto b = makeMemory("emb-b", "vec b"); b->setEmbedding({0.0, 1.0, 0.0});
    auto c = makeMemory("emb-c", "vec c"); c->setEmbedding({0.95, 0.05, 0.0});
    mgr_->createMemory(a); mgr_->createMemory(b); mgr_->createMemory(c);
    MemorySearchByEmbeddingParams p;
    p.embedding = {1.0, 0.0, 0.0};
    p.matchThreshold = 0.5; p.count = 10;
    auto hits = mgr_->searchMemories(p);
    ASSERT_GE(hits.size(), 2u);
    EXPECT_TRUE(hits.front()->getId() == "emb-a" || hits.front()->getId() == "emb-c");
}

// ---------------- AttentionAllocator -----------------------------------------

TEST(AttentionAllocator, BudgetAllocationAndPriority) {
    AttentionAllocator alloc(10.0);
    alloc.updateAttentionValue("e1", AttentionValue{0.9, 0.1, 0.1, 0.5});
    alloc.updateAttentionValue("e2", AttentionValue{0.2, 0.1, 0.1, 0.1});
    auto top = alloc.getTopAttentionElements(2);
    ASSERT_EQ(top.size(), 2u);
    EXPECT_EQ(top.front(), "e1");
    EXPECT_EQ(alloc.getStatistics().totalElements, 2u);
}

TEST(AttentionAllocator, NoveltyDecreasesWithRepetition) {
    AttentionAllocator alloc;
    double n1 = alloc.calculateNovelty("the quick brown fox", {});
    for (int i = 0; i < 3; ++i) alloc.updateNoveltyModel("the quick brown fox");
    double n2 = alloc.calculateNovelty("the quick brown fox", {});
    EXPECT_GT(n1, n2);
    EXPECT_GE(n1, 0.0); EXPECT_LE(n1, 1.0);
}

TEST(AttentionAllocator, DecayReducesValuesOverTime) {
    AttentionAllocator alloc;
    alloc.updateAttentionValue("d", AttentionValue{0.8, 0.8, 0.8, 0.8});
    auto before = alloc.getAttentionValue("d");
    alloc.decayAttentionValues(0.5);
    auto after = alloc.getAttentionValue("d");
    EXPECT_LT(after.urgency, before.urgency);
    EXPECT_LT(after.novelty, before.novelty);
    EXPECT_LT(after.activation, before.activation);
}

// ---------------- AttentionAwareMemoryManager -------------------------------

TEST(AttentionAware, ConsolidatesDuplicateMemories) {
    auto mgr = std::make_shared<AgentMemoryManager>();
    AttentionAwareMemoryManager aware;
    aware.setMemoryManager(mgr);
    mgr->createMemory(makeMemory("dup-1", "the cat sat on the mat", "room-Z"));
    mgr->createMemory(makeMemory("dup-2", "the cat sat on the mat", "room-Z"));
    mgr->createMemory(makeMemory("uniq-3", "completely unrelated", "room-Z"));
    AttentionValue high{0.9, 0.5, 0.5, 0.5};
    aware.updateMemoryAttention("dup-1", high);
    aware.updateMemoryAttention("dup-2", high);
    aware.updateMemoryAttention("uniq-3", high);
    aware.consolidateMemories();
    int remaining = 0;
    for (const auto& id : {"dup-1", "dup-2", "uniq-3"})
        if (mgr->getMemoryById(id)) ++remaining;
    EXPECT_EQ(remaining, 2);
    EXPECT_NE(mgr->getMemoryById("uniq-3"), nullptr);
}

TEST(AttentionAware, ForgetsLowAttentionMemories) {
    auto mgr = std::make_shared<AgentMemoryManager>();
    AttentionAwareMemoryManager aware;
    aware.setMemoryManager(mgr);
    mgr->createMemory(makeMemory("keep", "important", "r"));
    mgr->createMemory(makeMemory("drop", "trivial",  "r"));
    aware.updateMemoryAttention("keep", AttentionValue{0.9, 0.9, 0.9, 0.9});
    aware.updateMemoryAttention("drop", AttentionValue{0.001, 0.001, 0.001, 0.001});
    aware.forgetLowAttentionMemories(0.05);
    EXPECT_NE(mgr->getMemoryById("keep"), nullptr);
    EXPECT_EQ(mgr->getMemoryById("drop"), nullptr);
}

TEST(AttentionAware, SpreadingActivationPropagates) {
    auto mgr = std::make_shared<AgentMemoryManager>();
    AttentionAwareMemoryManager aware;
    aware.setMemoryManager(mgr);
    mgr->createMemory(makeMemory("ha", "alpha", "r"));
    mgr->createMemory(makeMemory("hb", "beta",  "r"));
    aware.refreshMemoryAttention("ha");
    aware.refreshMemoryAttention("hb");
    auto allocator = aware.getAttentionAllocator();
    ASSERT_NE(allocator, nullptr);
    AttentionValue av = allocator->getAttentionValue("ha");
    av.activation = 1.0;
    allocator->updateAttentionValue("ha", av);
    allocator->addAttentionLink("ha", "hb", 1.0);
    allocator->spreadActivation(2);
    EXPECT_GT(allocator->getAttentionValue("hb").activation, 0.0);
}
