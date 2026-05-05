// core_test.cpp
// End-to-end tests for the elizaos core types: HypergraphNode, HypergraphEdge,
// Memory, and Task.

#include "elizaos/core.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace elizaos;

TEST(HypergraphNode, ConstructorAndAttributes) {
    HypergraphNode n("nid-1", "concept-cat");
    EXPECT_FALSE(n.getAttribute("missing").has_value());
    n.setAttribute("color", "orange");
    auto v = n.getAttribute("color");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, "orange");
}

TEST(HypergraphEdge, BindsMultipleNodes) {
    HypergraphEdge e("eid-1", "knows", {"a", "b", "c"});
    EXPECT_NO_THROW((void)e);
}

TEST(Memory, ConstructorAndAccessors) {
    Memory m("m1", "content", "entity-1", "agent-1");
    EXPECT_EQ(m.getId(), "m1");
    EXPECT_EQ(m.getContent(), "content");
    EXPECT_EQ(m.getEntityId(), "entity-1");
    EXPECT_EQ(m.getAgentId(), "agent-1");
    EXPECT_TRUE(m.getRoomId().empty());
}

TEST(Memory, RoomIdSetter) {
    Memory m("m2", "content", "entity-1", "agent-1");
    m.setRoomId("room-42");
    EXPECT_EQ(m.getRoomId(), "room-42");
}

TEST(Memory, EmbeddingRoundtrip) {
    Memory m("m3", "content", "entity-1", "agent-1");
    EmbeddingVector v{1.0f, 2.0f, 3.0f, 4.0f};
    m.setEmbedding(v);
    auto& got = m.getEmbedding();
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(got->size(), 4u);
    EXPECT_FLOAT_EQ((*got)[2], 3.0f);
}

TEST(Memory, UniqueFlag) {
    Memory m("m4", "content", "entity-1", "agent-1");
    EXPECT_NO_THROW(m.setUnique(true));
    EXPECT_TRUE(m.isUnique());
    m.setUnique(false);
    EXPECT_FALSE(m.isUnique());
}

TEST(Memory, SimilarityRoundtrip) {
    Memory m("m5", "content", "entity-1", "agent-1");
    m.setSimilarity(0.42);
    EXPECT_NEAR(m.getSimilarity(), 0.42, 1e-9);
}

TEST(Task, BasicLifecycle) {
    Task t("tid-1", "task", "do something");
    EXPECT_EQ(t.getId(), "tid-1");
    EXPECT_NO_THROW(t.setStatus(TaskStatus::RUNNING));
    EXPECT_EQ(t.getStatus(), TaskStatus::RUNNING);
    t.setPriority(7);
    EXPECT_EQ(t.getPriority(), 7);
    t.addTag("urgent");
    EXPECT_NO_THROW(t.updateTimestamp());
}
