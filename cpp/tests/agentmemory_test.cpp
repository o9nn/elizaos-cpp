// agentmemory_test.cpp - E2E tests for elizaos::AgentMemoryManager.
#include <gtest/gtest.h>
#include "elizaos/agentmemory.hpp"
#include "elizaos/core.hpp"

using namespace elizaos;

namespace {
std::shared_ptr<Memory> makeMem(const std::string& content,
                                const UUID& room,
                                const UUID& entity,
                                const UUID& agent) {
    auto m = std::make_shared<Memory>(generateUUID(), content, entity, agent);
    m->setRoomId(room);
    return m;
}
}

class AgentMemoryTest : public ::testing::Test {
protected:
    AgentMemoryManager mgr;
    UUID room   = generateUUID();
    UUID entity = generateUUID();
    UUID agent  = generateUUID();

    void TearDown() override { mgr.clear(); }
};

TEST_F(AgentMemoryTest, CreateAndRetrieveMemory) {
    auto mem = makeMem("hello", room, entity, agent);
    auto id = mgr.createMemory(mem);
    EXPECT_FALSE(id.empty());

    auto got = mgr.getMemoryById(id);
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->getContent(), "hello");
}

TEST_F(AgentMemoryTest, GetMemoryByIdMissingReturnsNull) {
    auto got = mgr.getMemoryById(generateUUID());
    EXPECT_EQ(got, nullptr);
}

TEST_F(AgentMemoryTest, UpdateMemory) {
    auto mem = makeMem("v1", room, entity, agent);
    auto id = mgr.createMemory(mem);
    auto fetched = mgr.getMemoryById(id);
    ASSERT_NE(fetched, nullptr);

    fetched->setSimilarity(0.42);
    EXPECT_TRUE(mgr.updateMemory(fetched));

    auto reread = mgr.getMemoryById(id);
    ASSERT_NE(reread, nullptr);
    EXPECT_NEAR(reread->getSimilarity(), 0.42, 1e-9);
}

TEST_F(AgentMemoryTest, DeleteMemory) {
    auto mem = makeMem("doomed", room, entity, agent);
    auto id = mgr.createMemory(mem);
    EXPECT_NE(mgr.getMemoryById(id), nullptr);

    EXPECT_TRUE(mgr.deleteMemory(id));
    EXPECT_EQ(mgr.getMemoryById(id), nullptr);
}

TEST_F(AgentMemoryTest, DeleteManyMemories) {
    std::vector<UUID> ids;
    for (int i = 0; i < 5; ++i) {
        ids.push_back(mgr.createMemory(makeMem("m" + std::to_string(i),
                                               room, entity, agent)));
    }
    mgr.deleteManyMemories(ids);
    for (const auto& id : ids) {
        EXPECT_EQ(mgr.getMemoryById(id), nullptr);
    }
}

TEST_F(AgentMemoryTest, GetMemoriesByIds) {
    std::vector<UUID> ids;
    for (int i = 0; i < 3; ++i) {
        ids.push_back(mgr.createMemory(makeMem("m" + std::to_string(i),
                                               room, entity, agent)));
    }
    auto fetched = mgr.getMemoriesByIds(ids);
    EXPECT_EQ(fetched.size(), 3u);
}

TEST_F(AgentMemoryTest, GetMemoriesByRoomIds) {
    auto otherRoom = generateUUID();
    mgr.createMemory(makeMem("a", room, entity, agent));
    mgr.createMemory(makeMem("b", room, entity, agent));
    mgr.createMemory(makeMem("c", otherRoom, entity, agent));

    auto fetched = mgr.getMemoriesByRoomIds({room});
    EXPECT_EQ(fetched.size(), 2u);
}

TEST_F(AgentMemoryTest, SearchByCount) {
    for (int i = 0; i < 5; ++i) {
        mgr.createMemory(makeMem("m" + std::to_string(i), room, entity, agent));
    }
    MemorySearchParams p;
    p.roomId = room;
    p.count = 3;
    auto fetched = mgr.getMemories(p);
    EXPECT_LE(fetched.size(), 3u);
}

TEST_F(AgentMemoryTest, CountMemories) {
    for (int i = 0; i < 4; ++i) {
        mgr.createMemory(makeMem("c" + std::to_string(i), room, entity, agent));
    }
    EXPECT_EQ(mgr.countMemories(room), 4);
}

TEST_F(AgentMemoryTest, DeleteAllMemoriesForRoom) {
    for (int i = 0; i < 3; ++i) {
        mgr.createMemory(makeMem("x", room, entity, agent));
    }
    mgr.deleteAllMemories(room);
    EXPECT_EQ(mgr.countMemories(room), 0);
}

TEST_F(AgentMemoryTest, SearchByEmbedding) {
    auto mem = makeMem("vec mem", room, entity, agent);
    EmbeddingVector v{1.0f, 0.0f, 0.0f, 0.0f};
    mem->setEmbedding(v);
    mgr.createMemory(mem);

    MemorySearchByEmbeddingParams p;
    p.embedding = v;
    p.matchThreshold = 0.5;
    p.count = 5;
    p.roomId = room;
    auto results = mgr.searchMemories(p);
    EXPECT_GE(results.size(), 1u);
}

TEST(AgentMemoryGlobals, GlobalManagerExists) {
    auto& g = getGlobalMemoryManager();
    auto id = memory::store(std::make_shared<Memory>(
        generateUUID(), "global mem", generateUUID(), generateUUID()));
    EXPECT_FALSE(id.empty());
    auto got = memory::retrieve(id);
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->getContent(), "global mem");
    EXPECT_TRUE(memory::remove(id));
    (void)g;
}

TEST(AgentMemoryGlobals, ClearAllRemovesEverything) {
    memory::clearAll();
    auto id = memory::store(std::make_shared<Memory>(
        generateUUID(), "x", generateUUID(), generateUUID()));
    ASSERT_FALSE(id.empty());
    memory::clearAll();
    EXPECT_EQ(memory::retrieve(id), nullptr);
}
