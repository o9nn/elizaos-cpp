/**
 * test_goal_manager_e2e.cpp — Comprehensive E2E tests for the GoalManager module
 *
 * Covers:
 *   1. Goal lifecycle — creation, activation, completion, failure, cancellation
 *   2. Goal hierarchy — sub-goals, parent progress propagation
 *   3. Dependencies — blocking, unmet detection, auto-unblock
 *   4. Priority and scoring — selection, ranking
 *   5. Callbacks — event notifications
 *   6. Serialization — save/restore state
 *   7. Thread safety — concurrent goal operations
 *
 * Cross-fork parity: identical test file in both hurdcog and o9nn.
 */
#include <gtest/gtest.h>
#include "elizaos/goal_manager.hpp"
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

using namespace elizaos;

// ============================================================================
// Goal Creation and Basic Operations
// ============================================================================

class GoalManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager_ = std::make_unique<GoalManager>();
    }
    std::unique_ptr<GoalManager> manager_;
};

TEST_F(GoalManagerTest, CreateGoal) {
    auto goal = manager_->createGoal("KSM Cycle", "Complete a full KSM repair cycle");
    ASSERT_NE(goal, nullptr);
    EXPECT_EQ(goal->getName(), "KSM Cycle");
    EXPECT_EQ(goal->getDescription(), "Complete a full KSM repair cycle");
    EXPECT_EQ(goal->getStatus(), GoalStatus::PENDING);
}

TEST_F(GoalManagerTest, CreateGoalWithPriority) {
    auto goal = manager_->createGoal("Critical Fix", "Fix build failure",
                                      GoalPriority::CRITICAL, GoalType::MAINTENANCE);
    ASSERT_NE(goal, nullptr);
    EXPECT_EQ(goal->getPriority(), GoalPriority::CRITICAL);
    EXPECT_EQ(goal->getType(), GoalType::MAINTENANCE);
}

TEST_F(GoalManagerTest, GoalHasUniqueId) {
    auto g1 = manager_->createGoal("Goal A", "First");
    auto g2 = manager_->createGoal("Goal B", "Second");
    EXPECT_NE(g1->getId(), g2->getId());
}

TEST_F(GoalManagerTest, RetrieveGoal) {
    auto goal = manager_->createGoal("Test", "Description");
    auto retrieved = manager_->getGoal(goal->getId());
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getName(), "Test");
}

TEST_F(GoalManagerTest, HasGoal) {
    auto goal = manager_->createGoal("Exists", "Yes");
    EXPECT_TRUE(manager_->hasGoal(goal->getId()));
    EXPECT_FALSE(manager_->hasGoal("nonexistent-id"));
}

TEST_F(GoalManagerTest, RemoveGoal) {
    auto goal = manager_->createGoal("Remove Me", "Temporary");
    UUID id = goal->getId();
    EXPECT_TRUE(manager_->removeGoal(id));
    EXPECT_FALSE(manager_->hasGoal(id));
}

TEST_F(GoalManagerTest, RemoveNonexistent) {
    EXPECT_FALSE(manager_->removeGoal("nonexistent-id"));
}

// ============================================================================
// Goal Status Transitions
// ============================================================================

TEST_F(GoalManagerTest, ActivateGoal) {
    auto goal = manager_->createGoal("Activate", "Test");
    EXPECT_TRUE(manager_->activateGoal(goal->getId()));
    EXPECT_EQ(goal->getStatus(), GoalStatus::ACTIVE);
    EXPECT_TRUE(goal->isActive());
}

TEST_F(GoalManagerTest, CompleteGoal) {
    auto goal = manager_->createGoal("Complete", "Test");
    manager_->activateGoal(goal->getId());
    EXPECT_TRUE(manager_->completeGoal(goal->getId()));
    EXPECT_TRUE(goal->isCompleted());
}

TEST_F(GoalManagerTest, FailGoal) {
    auto goal = manager_->createGoal("Fail", "Test");
    manager_->activateGoal(goal->getId());
    EXPECT_TRUE(manager_->failGoal(goal->getId(), "optimizer non-convergence"));
    EXPECT_EQ(goal->getStatus(), GoalStatus::FAILED);
}

TEST_F(GoalManagerTest, CancelGoal) {
    auto goal = manager_->createGoal("Cancel", "Test");
    EXPECT_TRUE(manager_->cancelGoal(goal->getId()));
    EXPECT_EQ(goal->getStatus(), GoalStatus::CANCELLED);
}

TEST_F(GoalManagerTest, BlockGoal) {
    auto goal = manager_->createGoal("Block", "Test");
    EXPECT_TRUE(manager_->blockGoal(goal->getId()));
    EXPECT_TRUE(goal->isBlocked());
}

// ============================================================================
// Progress Tracking
// ============================================================================

TEST_F(GoalManagerTest, UpdateProgress) {
    auto goal = manager_->createGoal("Progress", "Test");
    manager_->activateGoal(goal->getId());
    EXPECT_TRUE(manager_->updateProgress(goal->getId(), 0.5));
    EXPECT_NEAR(goal->getProgress(), 0.5, 0.01);
}

TEST_F(GoalManagerTest, IncrementProgress) {
    auto goal = manager_->createGoal("Increment", "Test");
    manager_->activateGoal(goal->getId());
    manager_->incrementProgress(goal->getId(), 0.3);
    manager_->incrementProgress(goal->getId(), 0.2);
    EXPECT_NEAR(goal->getProgress(), 0.5, 0.01);
}

TEST_F(GoalManagerTest, ProgressClampedToOne) {
    auto goal = manager_->createGoal("Clamp", "Test");
    goal->setProgress(1.5);
    EXPECT_LE(goal->getProgress(), 1.0);
}

// ============================================================================
// Goal Queries
// ============================================================================

TEST_F(GoalManagerTest, GetAllGoals) {
    manager_->createGoal("A", "First");
    manager_->createGoal("B", "Second");
    manager_->createGoal("C", "Third");
    EXPECT_EQ(manager_->getAllGoals().size(), 3u);
}

TEST_F(GoalManagerTest, GetGoalsByStatus) {
    auto g1 = manager_->createGoal("Active1", "Test");
    auto g2 = manager_->createGoal("Active2", "Test");
    manager_->createGoal("Pending", "Test");
    manager_->activateGoal(g1->getId());
    manager_->activateGoal(g2->getId());
    auto active = manager_->getGoalsByStatus(GoalStatus::ACTIVE);
    EXPECT_EQ(active.size(), 2u);
}

TEST_F(GoalManagerTest, GetGoalsByPriority) {
    manager_->createGoal("Normal", "Test", GoalPriority::NORMAL, GoalType::ACHIEVEMENT);
    manager_->createGoal("Critical", "Test", GoalPriority::CRITICAL, GoalType::ACHIEVEMENT);
    auto critical = manager_->getGoalsByPriority(GoalPriority::CRITICAL);
    EXPECT_EQ(critical.size(), 1u);
}

TEST_F(GoalManagerTest, GetGoalsByTag) {
    auto goal = manager_->createGoal("Tagged", "Test");
    goal->addTag("ksm");
    goal->addTag("repair");
    auto tagged = manager_->getGoalsByTag("ksm");
    EXPECT_EQ(tagged.size(), 1u);
}

TEST_F(GoalManagerTest, GetActiveGoals) {
    auto g1 = manager_->createGoal("A", "Test");
    auto g2 = manager_->createGoal("B", "Test");
    manager_->activateGoal(g1->getId());
    manager_->activateGoal(g2->getId());
    EXPECT_EQ(manager_->getActiveGoals().size(), 2u);
}

TEST_F(GoalManagerTest, GetOverdueGoals) {
    auto goal = manager_->createGoal("Overdue", "Test");
    // Set deadline in the past
    goal->setDeadline(std::chrono::system_clock::now() - std::chrono::hours(1));
    manager_->activateGoal(goal->getId());
    auto overdue = manager_->getOverdueGoals();
    EXPECT_GE(overdue.size(), 1u);
}

// ============================================================================
// Goal Selection
// ============================================================================

TEST_F(GoalManagerTest, SelectNextGoal) {
    manager_->createGoal("Low", "Test", GoalPriority::LOW, GoalType::ACHIEVEMENT);
    auto high = manager_->createGoal("High", "Test", GoalPriority::HIGH, GoalType::ACHIEVEMENT);
    manager_->activateGoal(high->getId());
    auto next = manager_->selectNextGoal();
    ASSERT_NE(next, nullptr);
    EXPECT_EQ(next->getPriority(), GoalPriority::HIGH);
}

TEST_F(GoalManagerTest, GetTopGoals) {
    for (int i = 0; i < 10; ++i) {
        auto g = manager_->createGoal("Goal_" + std::to_string(i), "Test");
        manager_->activateGoal(g->getId());
    }
    auto top = manager_->getTopGoals(3);
    EXPECT_EQ(top.size(), 3u);
}

// ============================================================================
// Dependencies
// ============================================================================

TEST_F(GoalManagerTest, AddDependency) {
    auto g1 = manager_->createGoal("Dependent", "Needs g2");
    auto g2 = manager_->createGoal("Prerequisite", "Must complete first");
    EXPECT_TRUE(manager_->addDependency(g1->getId(), g2->getId()));
    EXPECT_TRUE(g1->hasDependencies());
}

TEST_F(GoalManagerTest, UnmetDependencies) {
    auto g1 = manager_->createGoal("Dependent", "Test");
    auto g2 = manager_->createGoal("Prereq", "Test");
    manager_->addDependency(g1->getId(), g2->getId());
    auto unmet = manager_->getUnmetDependencies(g1->getId());
    EXPECT_EQ(unmet.size(), 1u);
}

TEST_F(GoalManagerTest, DependenciesMet) {
    auto g1 = manager_->createGoal("Dependent", "Test");
    auto g2 = manager_->createGoal("Prereq", "Test");
    manager_->addDependency(g1->getId(), g2->getId());
    EXPECT_FALSE(manager_->areDependenciesMet(g1->getId()));
    manager_->activateGoal(g2->getId());
    manager_->completeGoal(g2->getId());
    EXPECT_TRUE(manager_->areDependenciesMet(g1->getId()));
}

TEST_F(GoalManagerTest, RemoveDependency) {
    auto g1 = manager_->createGoal("Dependent", "Test");
    auto g2 = manager_->createGoal("Prereq", "Test");
    manager_->addDependency(g1->getId(), g2->getId());
    EXPECT_TRUE(manager_->removeDependency(g1->getId(), g2->getId()));
    EXPECT_FALSE(g1->hasDependencies());
}

// ============================================================================
// Hierarchical Goals
// ============================================================================

TEST_F(GoalManagerTest, AddSubGoal) {
    auto parent = manager_->createGoal("Parent", "Main goal");
    auto child = manager_->createGoal("Child", "Sub-task");
    EXPECT_TRUE(manager_->addSubGoal(parent->getId(), child->getId()));
    EXPECT_TRUE(parent->hasSubGoals());
    EXPECT_EQ(parent->getSubGoals().size(), 1u);
}

TEST_F(GoalManagerTest, ParentProgressFromChildren) {
    auto parent = manager_->createGoal("Parent", "Main");
    auto c1 = manager_->createGoal("Child1", "Sub1");
    auto c2 = manager_->createGoal("Child2", "Sub2");
    manager_->addSubGoal(parent->getId(), c1->getId());
    manager_->addSubGoal(parent->getId(), c2->getId());
    manager_->activateGoal(c1->getId());
    manager_->completeGoal(c1->getId());
    manager_->updateParentProgress(c1->getId());
    // Parent should show ~50% progress (1 of 2 children complete)
    EXPECT_GT(parent->getProgress(), 0.0);
}

// ============================================================================
// Callbacks
// ============================================================================

TEST_F(GoalManagerTest, OnGoalCreatedCallback) {
    int callCount = 0;
    manager_->onGoalCreated([&callCount](std::shared_ptr<Goal>) { callCount++; });
    manager_->createGoal("Test", "Callback test");
    EXPECT_EQ(callCount, 1);
}

TEST_F(GoalManagerTest, OnGoalCompletedCallback) {
    int callCount = 0;
    manager_->onGoalCompleted([&callCount](std::shared_ptr<Goal>) { callCount++; });
    auto goal = manager_->createGoal("Complete", "Test");
    manager_->activateGoal(goal->getId());
    manager_->completeGoal(goal->getId());
    EXPECT_EQ(callCount, 1);
}

TEST_F(GoalManagerTest, OnGoalFailedCallback) {
    int callCount = 0;
    manager_->onGoalFailed([&callCount](std::shared_ptr<Goal>) { callCount++; });
    auto goal = manager_->createGoal("Fail", "Test");
    manager_->activateGoal(goal->getId());
    manager_->failGoal(goal->getId());
    EXPECT_EQ(callCount, 1);
}

// ============================================================================
// Statistics
// ============================================================================

TEST_F(GoalManagerTest, TotalGoalCount) {
    manager_->createGoal("A", "Test");
    manager_->createGoal("B", "Test");
    EXPECT_EQ(manager_->getTotalGoalCount(), 2u);
}

TEST_F(GoalManagerTest, ActiveGoalCount) {
    auto g1 = manager_->createGoal("Active", "Test");
    manager_->createGoal("Pending", "Test");
    manager_->activateGoal(g1->getId());
    EXPECT_EQ(manager_->getActiveGoalCount(), 1u);
}

TEST_F(GoalManagerTest, CompletedGoalCount) {
    auto g1 = manager_->createGoal("Done", "Test");
    manager_->activateGoal(g1->getId());
    manager_->completeGoal(g1->getId());
    EXPECT_EQ(manager_->getCompletedGoalCount(), 1u);
}

TEST_F(GoalManagerTest, GoalCountByStatus) {
    auto g1 = manager_->createGoal("A", "Test");
    auto g2 = manager_->createGoal("B", "Test");
    manager_->createGoal("C", "Test");
    manager_->activateGoal(g1->getId());
    manager_->activateGoal(g2->getId());
    manager_->completeGoal(g2->getId());
    auto counts = manager_->getGoalCountByStatus();
    EXPECT_GE(counts[GoalStatus::PENDING], 1u);
    EXPECT_GE(counts[GoalStatus::ACTIVE], 1u);
    EXPECT_GE(counts[GoalStatus::COMPLETED], 1u);
}

// ============================================================================
// Serialization
// ============================================================================

TEST_F(GoalManagerTest, SerializeAndDeserialize) {
    auto g1 = manager_->createGoal("Persistent", "Survives restart");
    g1->addTag("important");
    manager_->activateGoal(g1->getId());
    manager_->updateProgress(g1->getId(), 0.7);
    std::string serialized = manager_->serialize();
    EXPECT_FALSE(serialized.empty());
    // Create a new manager and restore
    GoalManager restored;
    EXPECT_TRUE(restored.deserialize(serialized));
    EXPECT_EQ(restored.getTotalGoalCount(), manager_->getTotalGoalCount());
}

// ============================================================================
// Goal Scoring
// ============================================================================

TEST_F(GoalManagerTest, GoalScoreComputation) {
    auto goal = manager_->createGoal("Score", "Test", GoalPriority::HIGH, GoalType::ACHIEVEMENT);
    goal->setProgress(0.5);
    double score = goal->calculateScore();
    EXPECT_GT(score, 0.0);
}

TEST_F(GoalManagerTest, HigherPriorityHigherScore) {
    auto low = manager_->createGoal("Low", "Test", GoalPriority::LOW, GoalType::ACHIEVEMENT);
    auto high = manager_->createGoal("High", "Test", GoalPriority::HIGH, GoalType::ACHIEVEMENT);
    EXPECT_GT(high->calculateScore(), low->calculateScore());
}

// ============================================================================
// Thread Safety
// ============================================================================

TEST_F(GoalManagerTest, ConcurrentGoalCreation) {
    std::atomic<int> created{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([this, &created, i]() {
            auto g = manager_->createGoal("Thread_" + std::to_string(i), "Concurrent");
            if (g) created++;
        });
    }
    for (auto& t : threads) t.join();
    EXPECT_EQ(created.load(), 10);
    EXPECT_EQ(manager_->getTotalGoalCount(), 10u);
}
