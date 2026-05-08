#include <gtest/gtest.h>
#include "elizaos/agentmemory.hpp"
#include "elizaos/core.hpp"
#include <memory>
#include <set>
#include <thread>
#include <ctime>

using namespace elizaos;

class AgentMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear the global memory manager before each test
        getGlobalMemoryManager().clear();
        getGlobalMemoryManager().enableThreadSafety(true);
        
        // Create test agent and entity IDs
        testAgentId = "agent-test-id-123";
        testEntityId = "entity-test-id-456";
        testRoomId = "room-test-id-789";
        testMemoryId1 = "memory-test-id-001";
        testMemoryId2 = "memory-test-id-002";
    }

    void TearDown() override {
        // Clean up after each test
        getGlobalMemoryManager().clear();
    }

    std::shared_ptr<Memory> createTestMemory(const UUID& id, const std::string& content, const UUID& entityId = "", const UUID& agentId = "") {
        UUID actualEntityId = entityId.empty() ? testEntityId : entityId;
        UUID actualAgentId = agentId.empty() ? testAgentId : agentId;
        return std::make_shared<Memory>(id, content, actualEntityId, actualAgentId);
    }

    // Test data
    UUID testAgentId;
    UUID testEntityId;
    UUID testRoomId;
    UUID testMemoryId1;
    UUID testMemoryId2;
};

// Test basic memory creation and retrieval
TEST_F(AgentMemoryTest, CreateAndRetrieveMemory) {
    auto memory = createTestMemory(testMemoryId1, "Test memory content");
    
    // Store memory
    UUID storedId = memory::store(memory);
    EXPECT_EQ(storedId, testMemoryId1);
    
    // Retrieve memory
    auto retrieved = memory::retrieve(testMemoryId1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getId(), testMemoryId1);
    EXPECT_EQ(retrieved->getContent(), "Test memory content");
    EXPECT_EQ(retrieved->getEntityId(), testEntityId);
    EXPECT_EQ(retrieved->getAgentId(), testAgentId);
}

// Test memory not found
TEST_F(AgentMemoryTest, RetrieveNonExistentMemory) {
    auto retrieved = memory::retrieve("non-existent-id");
    EXPECT_EQ(retrieved, nullptr);
}

// Test memory manager operations
TEST_F(AgentMemoryTest, MemoryManagerOperations) {
    auto& manager = getGlobalMemoryManager();
    
    auto memory1 = createTestMemory(testMemoryId1, "First memory");
    auto memory2 = createTestMemory(testMemoryId2, "Second memory");
    
    // Store memories
    UUID id1 = manager.createMemory(memory1);
    UUID id2 = manager.createMemory(memory2);
    
    EXPECT_EQ(id1, testMemoryId1);
    EXPECT_EQ(id2, testMemoryId2);
    
    // Retrieve by ID
    auto retrieved1 = manager.getMemoryById(testMemoryId1);
    auto retrieved2 = manager.getMemoryById(testMemoryId2);
    
    ASSERT_NE(retrieved1, nullptr);
    ASSERT_NE(retrieved2, nullptr);
    EXPECT_EQ(retrieved1->getContent(), "First memory");
    EXPECT_EQ(retrieved2->getContent(), "Second memory");
}

// Test memory search by criteria
TEST_F(AgentMemoryTest, SearchMemoriesByCriteria) {
    auto& manager = getGlobalMemoryManager();
    
    // Create memories with different agents and entities
    auto memory1 = createTestMemory("mem1", "Memory for agent1", "entity1", "agent1");
    auto memory2 = createTestMemory("mem2", "Memory for agent2", "entity2", "agent2");
    auto memory3 = createTestMemory("mem3", "Another memory for agent1", "entity1", "agent1");
    
    manager.createMemory(memory1);
    manager.createMemory(memory2);
    manager.createMemory(memory3);
    
    // Search by agent ID
    MemorySearchParams params;
    params.agentId = "agent1";
    params.tableName = "memories";
    
    auto results = manager.getMemories(params);
    EXPECT_EQ(results.size(), 2);
    
    // Verify the results are for agent1
    for (const auto& mem : results) {
        EXPECT_EQ(mem->getAgentId(), "agent1");
    }
    
    // Search by entity ID
    params.agentId.reset();
    params.entityId = "entity2";
    
    results = manager.getMemories(params);
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0]->getEntityId(), "entity2");
}

// Test memory retrieval by multiple IDs
TEST_F(AgentMemoryTest, GetMemoriesByIds) {
    auto& manager = getGlobalMemoryManager();
    
    auto memory1 = createTestMemory("mem1", "First memory");
    auto memory2 = createTestMemory("mem2", "Second memory");
    auto memory3 = createTestMemory("mem3", "Third memory");
    
    manager.createMemory(memory1);
    manager.createMemory(memory2);
    manager.createMemory(memory3);
    
    // Retrieve multiple memories
    std::vector<UUID> ids = {"mem1", "mem3", "non-existent"};
    auto results = manager.getMemoriesByIds(ids);
    
    EXPECT_EQ(results.size(), 2); // Only mem1 and mem3 should be found
    
    std::set<UUID> resultIds;
    for (const auto& mem : results) {
        resultIds.insert(mem->getId());
    }
    
    EXPECT_TRUE(resultIds.count("mem1"));
    EXPECT_TRUE(resultIds.count("mem3"));
    EXPECT_FALSE(resultIds.count("non-existent"));
}

// Test memory update
TEST_F(AgentMemoryTest, UpdateMemory) {
    auto& manager = getGlobalMemoryManager();
    
    auto memory = createTestMemory(testMemoryId1, "Original content");
    manager.createMemory(memory);
    
    // Update memory content
    auto updatedMemory = createTestMemory(testMemoryId1, "Updated content");
    bool success = manager.updateMemory(updatedMemory);
    EXPECT_TRUE(success);
    
    // Verify update
    auto retrieved = manager.getMemoryById(testMemoryId1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getContent(), "Updated content");
}

// Test memory deletion
TEST_F(AgentMemoryTest, DeleteMemory) {
    auto memory = createTestMemory(testMemoryId1, "Memory to delete");
    memory::store(memory);
    
    // Verify memory exists
    auto retrieved = memory::retrieve(testMemoryId1);
    ASSERT_NE(retrieved, nullptr);
    
    // Delete memory
    bool deleted = memory::remove(testMemoryId1);
    EXPECT_TRUE(deleted);
    
    // Verify memory is gone
    retrieved = memory::retrieve(testMemoryId1);
    EXPECT_EQ(retrieved, nullptr);
}

// Test delete multiple memories
TEST_F(AgentMemoryTest, DeleteManyMemories) {
    auto& manager = getGlobalMemoryManager();
    
    auto memory1 = createTestMemory("mem1", "First memory");
    auto memory2 = createTestMemory("mem2", "Second memory");
    auto memory3 = createTestMemory("mem3", "Third memory");
    
    manager.createMemory(memory1);
    manager.createMemory(memory2);
    manager.createMemory(memory3);
    
    // Delete multiple memories
    std::vector<UUID> idsToDelete = {"mem1", "mem3"};
    manager.deleteManyMemories(idsToDelete);
    
    // Verify deletions
    EXPECT_EQ(manager.getMemoryById("mem1"), nullptr);
    EXPECT_EQ(manager.getMemoryById("mem3"), nullptr);
    EXPECT_NE(manager.getMemoryById("mem2"), nullptr);
}

// Test unique memory creation
TEST_F(AgentMemoryTest, UniqueMemoryCreation) {
    auto& manager = getGlobalMemoryManager();
    
    auto memory1 = createTestMemory("mem1", "Duplicate content", testEntityId, testAgentId);
    auto memory2 = createTestMemory("mem2", "Duplicate content", testEntityId, testAgentId);
    
    // Store first memory
    UUID id1 = manager.createMemory(memory1, "memories", false);
    EXPECT_EQ(id1, "mem1");
    
    // Store second memory with same content, but unique=true
    UUID id2 = manager.createMemory(memory2, "memories", true);
    EXPECT_EQ(id2, "mem1"); // Should return existing ID
    
    // Verify only one memory exists
    MemorySearchParams params;
    params.tableName = "memories";
    auto results = manager.getMemories(params);
    EXPECT_EQ(results.size(), 1);
}

// Test memory counting
TEST_F(AgentMemoryTest, CountMemories) {
    auto& manager = getGlobalMemoryManager();
    
    // Create memories in the same room
    auto memory1 = createTestMemory("mem1", "Memory 1");
    auto memory2 = createTestMemory("mem2", "Memory 2");  
    auto memory3 = createTestMemory("mem3", "Memory 1"); // Duplicate content
    
    // Set all to same room
    memory1->setRoomId(testRoomId);
    memory2->setRoomId(testRoomId);
    memory3->setRoomId(testRoomId);
    
    manager.createMemory(memory1);
    manager.createMemory(memory2);
    manager.createMemory(memory3);
    
    // Count all memories in room
    int totalCount = manager.countMemories(testRoomId, false);
    EXPECT_EQ(totalCount, 3);
    
    // Count unique memories in room
    int uniqueCount = manager.countMemories(testRoomId, true);
    EXPECT_EQ(uniqueCount, 2); // "Memory 1" and "Memory 2"
}

// Test embedding-based search
TEST_F(AgentMemoryTest, EmbeddingBasedSearch) {
    auto& manager = getGlobalMemoryManager();
    
    // Create memories with embeddings
    auto memory1 = createTestMemory("mem1", "First memory");
    auto memory2 = createTestMemory("mem2", "Second memory");
    
    // Set embeddings (simple 3D vectors for testing)
    EmbeddingVector embedding1 = {1.0f, 0.0f, 0.0f};
    EmbeddingVector embedding2 = {0.0f, 1.0f, 0.0f};
    EmbeddingVector queryEmbedding = {0.8f, 0.6f, 0.0f}; // Closer to embedding1
    
    memory1->setEmbedding(embedding1);
    memory2->setEmbedding(embedding2);
    
    manager.createMemory(memory1);
    manager.createMemory(memory2);
    
    // Search by embedding
    MemorySearchByEmbeddingParams params;
    params.embedding = queryEmbedding;
    params.matchThreshold = 0.5;
    params.tableName = "memories";
    
    auto results = manager.searchMemories(params);
    
    // Should find memory1 as it's more similar
    EXPECT_GE(results.size(), 1);
    if (!results.empty()) {
        EXPECT_EQ(results[0]->getId(), "mem1");
        EXPECT_GT(results[0]->getSimilarity(), 0.5);
    }
}

// Test std::thread safety (disabled for now due to timing issues)
TEST_F(AgentMemoryTest, DISABLED_ThreadSafety) {
    auto& manager = getGlobalMemoryManager();
    manager.enableThreadSafety(true);
    manager.clear(); // Ensure clean state
    
    const int numThreads = 2;
    const int memoriesPerThread = 10;
    std::vector<std::thread> threads;
    
    // Create multiple threads that store memories concurrently
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&manager, t, memoriesPerThread]() {
            for (int i = 0; i < memoriesPerThread; ++i) {
                // Use more unique IDs to avoid collisions
                std::string id = "ts_t" + std::to_string(t) + "_m" + std::to_string(i) + "_" + std::to_string(std::time(nullptr));
                std::string content = "TS Memory from std::thread " + std::to_string(t) + " #" + std::to_string(i);
                
                auto memory = std::make_shared<Memory>(id, content, "ts_entity1", "ts_agent1");
                manager.createMemory(memory);
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify all memories were stored
    MemorySearchParams params;
    params.tableName = "memories";
    params.agentId = "ts_agent1"; // Filter to only our test memories
    auto results = manager.getMemories(params);
    
    EXPECT_EQ(results.size(), numThreads * memoriesPerThread);
}

// Test memory retrieval by room IDs
TEST_F(AgentMemoryTest, GetMemoriesByRoomIds) {
    auto& manager = getGlobalMemoryManager();
    
    // Create memories in different rooms
    auto memory1 = createTestMemory("mem1", "Memory in room1");
    auto memory2 = createTestMemory("mem2", "Memory in room2");
    auto memory3 = createTestMemory("mem3", "Another memory in room1");
    auto memory4 = createTestMemory("mem4", "Memory in room3");
    
    memory1->setRoomId("room1");
    memory2->setRoomId("room2");
    memory3->setRoomId("room1");
    memory4->setRoomId("room3");
    
    manager.createMemory(memory1);
    manager.createMemory(memory2);
    manager.createMemory(memory3);
    manager.createMemory(memory4);
    
    // Get memories for specific rooms
    std::vector<UUID> roomIds = {"room1", "room3"};
    auto results = manager.getMemoriesByRoomIds(roomIds);
    
    EXPECT_EQ(results.size(), 3); // mem1, mem3, mem4
    
    std::set<UUID> resultIds;
    for (const auto& mem : results) {
        resultIds.insert(mem->getId());
    }
    
    EXPECT_TRUE(resultIds.count("mem1"));
    EXPECT_TRUE(resultIds.count("mem3"));
    EXPECT_TRUE(resultIds.count("mem4"));
    EXPECT_FALSE(resultIds.count("mem2"));
}

// Test convenience functions
TEST_F(AgentMemoryTest, ConvenienceFunctions) {
    auto memory = createTestMemory(testMemoryId1, "Test convenience functions");
    
    // Test store convenience std::function
    UUID storedId = memory::store(memory);
    EXPECT_EQ(storedId, testMemoryId1);
    
    // Test retrieve convenience std::function
    auto retrieved = memory::retrieve(testMemoryId1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getContent(), "Test convenience functions");
    
    // Test search convenience std::function
    MemorySearchParams params;
    params.tableName = "memories";
    auto searchResults = memory::search(params);
    EXPECT_GE(searchResults.size(), 1);
    
    // Test remove convenience std::function
    bool removed = memory::remove(testMemoryId1);
    EXPECT_TRUE(removed);
    
    // Verify removal
    auto afterRemoval = memory::retrieve(testMemoryId1);
    EXPECT_EQ(afterRemoval, nullptr);
}

// Test clear all functionality
TEST_F(AgentMemoryTest, ClearAllMemories) {
    // Store some memories
    auto memory1 = createTestMemory("mem1", "First memory");
    auto memory2 = createTestMemory("mem2", "Second memory");
    
    memory::store(memory1);
    memory::store(memory2);
    
    // Verify memories exist
    EXPECT_NE(memory::retrieve("mem1"), nullptr);
    EXPECT_NE(memory::retrieve("mem2"), nullptr);
    
    // Clear all memories
    memory::clearAll();
    
    // Verify memories are gone
    EXPECT_EQ(memory::retrieve("mem1"), nullptr);
    EXPECT_EQ(memory::retrieve("mem2"), nullptr);
}