#include <gtest/gtest.h>
#include "elizaos/attention.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/core.hpp"
#include <memory>
#include <thread>
#include <chrono>

using namespace elizaos;

class AttentionAllocationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test data
        testElementId1 = "attention-element-1";
        testElementId2 = "attention-element-2";
        testElementId3 = "attention-element-3";
        
        testMemoryId1 = "attention-memory-1";
        testMemoryId2 = "attention-memory-2";
        
        testAgentId = "attention-agent-test";
        testEntityId = "attention-entity-test";
        testRoomId = "attention-room-test";
        
        // Create attention allocator
        attentionAllocator = std::make_unique<AttentionAllocator>(100.0);
        
        // Clear global memory manager
        getGlobalMemoryManager().clear();
    }
    
    void TearDown() override {
        getGlobalMemoryManager().clear();
    }
    
    std::shared_ptr<Memory> createTestMemory(const UUID& id, const std::string& content) {
        return std::make_shared<Memory>(id, content, testEntityId, testAgentId);
    }
    
    AttentionValue createTestAttentionValue(double importance = 0.5, double urgency = 0.3, 
                                           double novelty = 0.2, double activation = 0.1) {
        AttentionValue value;
        value.importance = importance;
        value.urgency = urgency;
        value.novelty = novelty;
        value.activation = activation;
        return value;
    }
    
    // Test data
    UUID testElementId1, testElementId2, testElementId3;
    UUID testMemoryId1, testMemoryId2;
    UUID testAgentId, testEntityId, testRoomId;
    
    std::unique_ptr<AttentionAllocator> attentionAllocator;
};

// Test AttentionValue composite score calculation
TEST_F(AttentionAllocationTest, AttentionValueCompositeScore) {
    AttentionValue value = createTestAttentionValue(0.8, 0.6, 0.4, 0.2);
    
    double expectedScore = (0.8 * 0.4) + (0.6 * 0.3) + (0.4 * 0.2) + (0.2 * 0.1);
    EXPECT_NEAR(value.getCompositeScore(), expectedScore, 0.001);
}

// Test AttentionValue decay functions
TEST_F(AttentionAllocationTest, AttentionValueDecay) {
    AttentionValue value = createTestAttentionValue(0.5, 0.8, 0.7, 0.6);
    
    double originalUrgency = value.urgency;
    double originalNovelty = value.novelty;
    double originalActivation = value.activation;
    
    value.decayUrgency(0.9);
    value.decayNovelty(0.8);
    value.decayActivation(0.7);
    
    EXPECT_NEAR(value.urgency, originalUrgency * 0.9, 0.001);
    EXPECT_NEAR(value.novelty, originalNovelty * 0.8, 0.001);
    EXPECT_NEAR(value.activation, originalActivation * 0.7, 0.001);
    EXPECT_EQ(value.importance, 0.5); // Should remain unchanged
}

// Test AttentionBudget allocation and deallocation
TEST_F(AttentionAllocationTest, AttentionBudgetAllocation) {
    AttentionBudget budget(100.0);
    
    // Test allocation
    EXPECT_TRUE(budget.allocateAttention(testElementId1, 30.0));
    EXPECT_TRUE(budget.allocateAttention(testElementId2, 20.0));
    EXPECT_EQ(budget.getAvailableBudget(), 50.0);
    EXPECT_EQ(budget.getAllocatedBudget(), 50.0);
    
    // Test over-allocation
    EXPECT_FALSE(budget.allocateAttention(testElementId3, 60.0));
    EXPECT_EQ(budget.getAvailableBudget(), 50.0);
    
    // Test deallocation
    EXPECT_TRUE(budget.deallocateAttention(testElementId1, 10.0));
    EXPECT_EQ(budget.getAvailableBudget(), 60.0);
    EXPECT_EQ(budget.getAllocation(testElementId1), 20.0);
    
    // Test complete deallocation
    EXPECT_TRUE(budget.deallocateAttention(testElementId1, 20.0));
    EXPECT_EQ(budget.getAllocation(testElementId1), 0.0);
    EXPECT_EQ(budget.getAvailableBudget(), 80.0);
}

// Test AttentionBudget redistribution
TEST_F(AttentionAllocationTest, AttentionBudgetRedistribution) {
    AttentionBudget budget(100.0);
    
    budget.allocateAttention(testElementId1, 30.0);
    budget.allocateAttention(testElementId2, 20.0);
    
    // Adjust budget size
    budget.adjustTotalBudget(200.0);
    
    // Check that allocations were scaled
    EXPECT_NEAR(budget.getAllocation(testElementId1), 60.0, 0.001);
    EXPECT_NEAR(budget.getAllocation(testElementId2), 40.0, 0.001);
    EXPECT_EQ(budget.getTotalBudget(), 200.0);
}

// Test ActivationSpreadingNetwork node and edge management
TEST_F(AttentionAllocationTest, ActivationSpreadingNetworkBasics) {
    ActivationSpreadingNetwork network;
    
    // Add nodes
    network.addNode(testElementId1, 0.8);
    network.addNode(testElementId2, 0.3);
    network.addNode(testElementId3, 0.1);
    
    // Check initial activations
    EXPECT_NEAR(network.getActivation(testElementId1), 0.8, 0.001);
    EXPECT_NEAR(network.getActivation(testElementId2), 0.3, 0.001);
    EXPECT_NEAR(network.getActivation(testElementId3), 0.1, 0.001);
    
    // Add edges
    network.addEdge(testElementId1, testElementId2, 0.5);
    network.addEdge(testElementId1, testElementId3, 0.3);
    
    // Check edge weights
    EXPECT_NEAR(network.getEdgeWeight(testElementId1, testElementId2), 0.5, 0.001);
    EXPECT_NEAR(network.getEdgeWeight(testElementId1, testElementId3), 0.3, 0.001);
    
    // Check neighbors
    auto neighbors = network.getNeighbors(testElementId1);
    EXPECT_EQ(neighbors.size(), 2);
    
    // Test activation spreading
    network.spreadActivation(1);
    
    // testElementId2 and testElementId3 should have increased activation
    EXPECT_GT(network.getActivation(testElementId2), 0.3);
    EXPECT_GT(network.getActivation(testElementId3), 0.1);
    
    // testElementId1 should have decreased activation
    EXPECT_LT(network.getActivation(testElementId1), 0.8);
}

// Test ActivationSpreadingNetwork top activated nodes
TEST_F(AttentionAllocationTest, ActivationSpreadingNetworkTopNodes) {
    ActivationSpreadingNetwork network;
    
    network.addNode(testElementId1, 0.9);
    network.addNode(testElementId2, 0.5);
    network.addNode(testElementId3, 0.2);
    
    auto topNodes = network.getTopActivatedNodes(2);
    EXPECT_EQ(topNodes.size(), 2);
    EXPECT_EQ(topNodes[0], testElementId1);
    EXPECT_EQ(topNodes[1], testElementId2);
}

// Test ActivationSpreadingNetwork decay
TEST_F(AttentionAllocationTest, ActivationSpreadingNetworkDecay) {
    ActivationSpreadingNetwork network;
    
    network.addNode(testElementId1, 0.8);
    network.addNode(testElementId2, 0.6);
    
    network.decayActivations(0.9);
    
    EXPECT_NEAR(network.getActivation(testElementId1), 0.72, 0.001);
    EXPECT_NEAR(network.getActivation(testElementId2), 0.54, 0.001);
}

// Test AttentionAllocator basic functionality
TEST_F(AttentionAllocationTest, AttentionAllocatorBasics) {
    AttentionValue value1 = createTestAttentionValue(0.8, 0.6, 0.4, 0.2);
    AttentionValue value2 = createTestAttentionValue(0.5, 0.3, 0.2, 0.1);
    
    attentionAllocator->updateAttentionValue(testElementId1, value1);
    attentionAllocator->updateAttentionValue(testElementId2, value2);
    
    // Test retrieval
    auto retrieved1 = attentionAllocator->getAttentionValue(testElementId1);
    auto retrieved2 = attentionAllocator->getAttentionValue(testElementId2);
    
    EXPECT_NEAR(retrieved1.importance, 0.8, 0.001);
    EXPECT_NEAR(retrieved1.urgency, 0.6, 0.001);
    EXPECT_NEAR(retrieved2.importance, 0.5, 0.001);
    EXPECT_NEAR(retrieved2.urgency, 0.3, 0.001);
    
    // Test existence check
    EXPECT_TRUE(attentionAllocator->hasAttentionValue(testElementId1));
    EXPECT_TRUE(attentionAllocator->hasAttentionValue(testElementId2));
    EXPECT_FALSE(attentionAllocator->hasAttentionValue(testElementId3));
}

// Test AttentionAllocator element prioritization
TEST_F(AttentionAllocationTest, AttentionAllocatorPrioritization) {
    AttentionValue highValue = createTestAttentionValue(0.9, 0.8, 0.7, 0.6);
    AttentionValue mediumValue = createTestAttentionValue(0.6, 0.5, 0.4, 0.3);
    AttentionValue lowValue = createTestAttentionValue(0.3, 0.2, 0.1, 0.0);
    
    attentionAllocator->updateAttentionValue(testElementId1, mediumValue);
    attentionAllocator->updateAttentionValue(testElementId2, highValue);
    attentionAllocator->updateAttentionValue(testElementId3, lowValue);
    
    std::vector<UUID> elements = {testElementId1, testElementId2, testElementId3};
    auto prioritized = attentionAllocator->prioritizeElements(elements);
    
    EXPECT_EQ(prioritized.size(), 3);
    EXPECT_EQ(prioritized[0].elementId, testElementId2); // Highest priority
    EXPECT_EQ(prioritized[1].elementId, testElementId1); // Medium priority
    EXPECT_EQ(prioritized[2].elementId, testElementId3); // Lowest priority
    
    // Check that priorities are in descending order
    EXPECT_GT(prioritized[0].priority, prioritized[1].priority);
    EXPECT_GT(prioritized[1].priority, prioritized[2].priority);
}

// Test AttentionAllocator resource allocation
TEST_F(AttentionAllocationTest, AttentionAllocatorResourceAllocation) {
    AttentionValue highValue = createTestAttentionValue(0.9, 0.8, 0.7, 0.6);
    AttentionValue mediumValue = createTestAttentionValue(0.6, 0.5, 0.4, 0.3);
    AttentionValue lowValue = createTestAttentionValue(0.3, 0.2, 0.1, 0.0);
    
    attentionAllocator->updateAttentionValue(testElementId1, mediumValue);
    attentionAllocator->updateAttentionValue(testElementId2, highValue);
    attentionAllocator->updateAttentionValue(testElementId3, lowValue);
    
    std::vector<UUID> candidates = {testElementId1, testElementId2, testElementId3};
    auto allocated = attentionAllocator->allocateAttention(candidates, 2);
    
    // Should allocate to highest priority elements first
    EXPECT_EQ(allocated.size(), 2);
    EXPECT_EQ(allocated[0], testElementId2); // Highest priority
    EXPECT_EQ(allocated[1], testElementId1); // Medium priority
}

// Test AttentionAllocator memory prioritization
TEST_F(AttentionAllocationTest, AttentionAllocatorMemoryPrioritization) {
    auto memory1 = createTestMemory(testMemoryId1, "Low priority memory");
    auto memory2 = createTestMemory(testMemoryId2, "High priority important urgent memory");
    
    memory1->setRoomId(testRoomId);
    memory2->setRoomId(testRoomId);
    
    // Set attention values
    AttentionValue lowValue = createTestAttentionValue(0.3, 0.2, 0.1, 0.0);
    AttentionValue highValue = createTestAttentionValue(0.9, 0.8, 0.7, 0.6);
    
    attentionAllocator->updateAttentionValue(testMemoryId1, lowValue);
    attentionAllocator->updateAttentionValue(testMemoryId2, highValue);
    
    std::vector<std::shared_ptr<Memory>> memories = {memory1, memory2};
    auto prioritized = attentionAllocator->prioritizeMemories(memories);
    
    EXPECT_EQ(prioritized.size(), 2);
    EXPECT_EQ(prioritized[0]->getId(), testMemoryId2); // High priority first
    EXPECT_EQ(prioritized[1]->getId(), testMemoryId1); // Low priority second
}

// Test AttentionAllocator novelty calculation
TEST_F(AttentionAllocationTest, AttentionAllocatorNoveltyCalculation) {
    std::string content1 = "This is a completely new and novel content";
    std::string content2 = "This is a repeated content";
    std::string content3 = "This is a repeated content"; // Same as content2
    
    double novelty1 = attentionAllocator->calculateNovelty(content1, {});
    double novelty2 = attentionAllocator->calculateNovelty(content2, {});
    double novelty3 = attentionAllocator->calculateNovelty(content3, {});
    
    // First content should be most novel
    EXPECT_GT(novelty1, novelty2);
    // Repeated content should be less novel
    EXPECT_GT(novelty2, novelty3);
}

// Test AttentionAllocator activation spreading
TEST_F(AttentionAllocationTest, AttentionAllocatorActivationSpreading) {
    AttentionValue value1 = createTestAttentionValue(0.5, 0.3, 0.2, 0.8);
    AttentionValue value2 = createTestAttentionValue(0.4, 0.2, 0.1, 0.2);
    
    attentionAllocator->updateAttentionValue(testElementId1, value1);
    attentionAllocator->updateAttentionValue(testElementId2, value2);
    
    // Add attention link
    attentionAllocator->addAttentionLink(testElementId1, testElementId2, 0.5);
    
    // Get initial activation
    double initialActivation2 = attentionAllocator->getAttentionValue(testElementId2).activation;
    
    // Spread activation
    attentionAllocator->spreadActivation(1);
    
    // testElementId2 should have increased activation
    double finalActivation2 = attentionAllocator->getAttentionValue(testElementId2).activation;
    EXPECT_GT(finalActivation2, initialActivation2);
}

// Test AttentionAllocator decay
TEST_F(AttentionAllocationTest, AttentionAllocatorDecay) {
    AttentionValue value = createTestAttentionValue(0.5, 0.8, 0.7, 0.6);
    attentionAllocator->updateAttentionValue(testElementId1, value);
    
    // Get initial values
    auto initialValue = attentionAllocator->getAttentionValue(testElementId1);
    
    // Decay attention values
    attentionAllocator->decayAttentionValues(0.9);
    
    // Get decayed values
    auto decayedValue = attentionAllocator->getAttentionValue(testElementId1);
    
    // Values should be decayed
    EXPECT_LT(decayedValue.urgency, initialValue.urgency);
    EXPECT_LT(decayedValue.novelty, initialValue.novelty);
    EXPECT_LT(decayedValue.activation, initialValue.activation);
    EXPECT_EQ(decayedValue.importance, initialValue.importance); // Importance doesn't decay
}

// Test AttentionAllocator statistics
TEST_F(AttentionAllocationTest, AttentionAllocatorStatistics) {
    AttentionValue value1 = createTestAttentionValue(0.8, 0.6, 0.4, 0.2);
    AttentionValue value2 = createTestAttentionValue(0.5, 0.3, 0.2, 0.1);
    AttentionValue value3 = createTestAttentionValue(0.2, 0.1, 0.0, 0.0);
    
    attentionAllocator->updateAttentionValue(testElementId1, value1);
    attentionAllocator->updateAttentionValue(testElementId2, value2);
    attentionAllocator->updateAttentionValue(testElementId3, value3);
    
    auto stats = attentionAllocator->getStatistics();
    
    EXPECT_EQ(stats.totalElements, 3);
    EXPECT_GT(stats.averageAttention, 0.0);
    EXPECT_GT(stats.maxAttention, stats.averageAttention);
    EXPECT_LE(stats.minAttention, stats.averageAttention);
    EXPECT_GE(stats.budgetUtilization, 0.0);
    EXPECT_LE(stats.budgetUtilization, 1.0);
}

// Test AttentionAllocator top attention elements
TEST_F(AttentionAllocationTest, AttentionAllocatorTopElements) {
    AttentionValue highValue = createTestAttentionValue(0.9, 0.8, 0.7, 0.6);
    AttentionValue mediumValue = createTestAttentionValue(0.6, 0.5, 0.4, 0.3);
    AttentionValue lowValue = createTestAttentionValue(0.3, 0.2, 0.1, 0.0);
    
    attentionAllocator->updateAttentionValue(testElementId1, mediumValue);
    attentionAllocator->updateAttentionValue(testElementId2, highValue);
    attentionAllocator->updateAttentionValue(testElementId3, lowValue);
    
    auto topElements = attentionAllocator->getTopAttentionElements(2);
    
    EXPECT_EQ(topElements.size(), 2);
    EXPECT_EQ(topElements[0], testElementId2); // Highest
    EXPECT_EQ(topElements[1], testElementId1); // Medium
}

// Test AttentionAwareMemoryManager integration
TEST_F(AttentionAllocationTest, AttentionAwareMemoryManagerIntegration) {
    auto& attentionAwareManager = getGlobalAttentionAwareMemoryManager();
    auto& regularManager = getGlobalMemoryManager();
    
    // Set up the connection
    attentionAwareManager.setMemoryManager(std::shared_ptr<AgentMemoryManager>(&regularManager, [](AgentMemoryManager*){}));
    
    // Create test memories
    auto memory1 = createTestMemory(testMemoryId1, "Regular memory");
    auto memory2 = createTestMemory(testMemoryId2, "Important urgent memory");
    
    memory1->setRoomId(testRoomId);
    memory2->setRoomId(testRoomId);
    
    // Store memories
    regularManager.createMemory(memory1);
    regularManager.createMemory(memory2);
    
    // Set attention values
    AttentionValue lowValue = createTestAttentionValue(0.3, 0.2, 0.1, 0.0);
    AttentionValue highValue = createTestAttentionValue(0.9, 0.8, 0.7, 0.6);
    
    attentionAwareManager.updateMemoryAttention(testMemoryId1, lowValue);
    attentionAwareManager.updateMemoryAttention(testMemoryId2, highValue);
    
    // Test attention-aware memory retrieval
    MemorySearchParams params;
    params.roomId = testRoomId;
    params.tableName = "memories";
    
    auto memories = attentionAwareManager.getMemoriesWithAttention(params, 10);
    
    EXPECT_EQ(memories.size(), 2);
    EXPECT_EQ(memories[0]->getId(), testMemoryId2); // High attention first
    EXPECT_EQ(memories[1]->getId(), testMemoryId1); // Low attention second
}

// Test AttentionAwareMemoryManager low attention memory forgetting
TEST_F(AttentionAllocationTest, AttentionAwareMemoryManagerForgetting) {
    auto& attentionAwareManager = getGlobalAttentionAwareMemoryManager();
    auto& regularManager = getGlobalMemoryManager();
    
    // Set up the connection
    attentionAwareManager.setMemoryManager(std::shared_ptr<AgentMemoryManager>(&regularManager, [](AgentMemoryManager*){}));
    
    // Create test memories
    auto memory1 = createTestMemory(testMemoryId1, "Low attention memory");
    auto memory2 = createTestMemory(testMemoryId2, "High attention memory");
    
    // Store memories
    regularManager.createMemory(memory1);
    regularManager.createMemory(memory2);
    
    // Set attention values
    AttentionValue lowValue = createTestAttentionValue(0.005, 0.002, 0.001, 0.0); // Very low
    AttentionValue highValue = createTestAttentionValue(0.9, 0.8, 0.7, 0.6);
    
    attentionAwareManager.updateMemoryAttention(testMemoryId1, lowValue);
    attentionAwareManager.updateMemoryAttention(testMemoryId2, highValue);
    
    // Forget low attention memories
    attentionAwareManager.forgetLowAttentionMemories(0.01);
    
    // Check that low attention memory was forgotten
    EXPECT_EQ(regularManager.getMemoryById(testMemoryId1), nullptr);
    EXPECT_NE(regularManager.getMemoryById(testMemoryId2), nullptr);
}

// Test convenience functions
TEST_F(AttentionAllocationTest, ConvenienceFunctions) {
    // Clear std::any previous state
    getGlobalAttentionAwareMemoryManager().getAttentionAllocator()->setBudgetSize(100.0);
    
    // Set up attention aware memory manager
    auto& attentionAwareManager = getGlobalAttentionAwareMemoryManager();
    auto& regularManager = getGlobalMemoryManager();
    attentionAwareManager.setMemoryManager(std::shared_ptr<AgentMemoryManager>(&regularManager, [](AgentMemoryManager*){}));
    
    // Create test memories
    auto memory1 = createTestMemory(testMemoryId1, "Memory 1");
    auto memory2 = createTestMemory(testMemoryId2, "Memory 2");
    
    memory1->setRoomId(testRoomId);
    memory2->setRoomId(testRoomId);
    
    regularManager.createMemory(memory1);
    regularManager.createMemory(memory2);
    
    // Test attention convenience functions with explicit high and low values
    AttentionValue highValue = createTestAttentionValue(0.9, 0.8, 0.7, 0.6);
    AttentionValue lowValue = createTestAttentionValue(0.1, 0.1, 0.1, 0.0);
    
    attention::updateAttention(testMemoryId1, highValue);
    attention::updateAttention(testMemoryId2, lowValue);
    
    // Also update in the memory manager
    attentionAwareManager.updateMemoryAttention(testMemoryId1, highValue);
    attentionAwareManager.updateMemoryAttention(testMemoryId2, lowValue);
    
    auto retrievedValue = attention::getAttention(testMemoryId1);
    
    EXPECT_NEAR(retrievedValue.importance, 0.9, 0.001);
    EXPECT_NEAR(retrievedValue.urgency, 0.8, 0.001);
    
    // Test top memories - should get memory1 with high attention
    auto topMemories = attention::getTopMemories(2);
    EXPECT_GE(topMemories.size(), 1);
    
    // Find the memory with high attention score among the results
    bool foundHighAttentionMemory = false;
    for (const auto& mem : topMemories) {
        if (mem->getId() == testMemoryId1) {
            foundHighAttentionMemory = true;
            break;
        }
    }
    EXPECT_TRUE(foundHighAttentionMemory);
    
    // Test maintenance functions
    attention::performMaintenance();
    attention::decayAttentionValues(0.9);
    
    // Values should be decayed
    auto decayedValue = attention::getAttention(testMemoryId1);
    EXPECT_LT(decayedValue.urgency, 0.8); // Original was 0.8
}

// Test std::thread safety (basic test)
TEST_F(AttentionAllocationTest, ThreadSafety) {
    const int numThreads = 2;
    const int operationsPerThread = 10;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < operationsPerThread; ++i) {
                UUID elementId = "thread_" + std::to_string(t) + "_element_" + std::to_string(i);
                AttentionValue value = createTestAttentionValue(0.5, 0.3, 0.2, 0.1);
                
                attentionAllocator->updateAttentionValue(elementId, value);
                attentionAllocator->getAttentionValue(elementId);
            }
        });
    }
    
    for (auto& std::thread : threads) {
        thread.join();
    }
    
    // Check that all elements were added
    auto stats = attentionAllocator->getStatistics();
    EXPECT_EQ(stats.totalElements, numThreads * operationsPerThread);
}

// Test periodic maintenance
TEST_F(AttentionAllocationTest, PeriodicMaintenance) {
    AttentionValue value = createTestAttentionValue(0.5, 0.8, 0.7, 0.6);
    attentionAllocator->updateAttentionValue(testElementId1, value);
    
    // Allocate some attention
    attentionAllocator->allocateAttention({testElementId1}, 1);
    
    // Get initial state
    auto initialValue = attentionAllocator->getAttentionValue(testElementId1);
    
    // Perform maintenance
    attentionAllocator->periodicMaintenance();
    
    // Check that values were decayed
    auto maintainedValue = attentionAllocator->getAttentionValue(testElementId1);
    EXPECT_LT(maintainedValue.urgency, initialValue.urgency);
    EXPECT_LT(maintainedValue.novelty, initialValue.novelty);
    EXPECT_LT(maintainedValue.activation, initialValue.activation);
}