// goal_manager_test.cpp - Tests for the Goal Management System

#include <gtest/gtest.h>
#include "elizaos/goal_manager.hpp"
#include <thread>
#include <chrono>

using namespace elizaos;

class GoalManagerTest : public ::testing::Test {
protected:
    GoalManager manager;
};

// ==============================================================================
// GOAL TESTS
// ==============================================================================

TEST_F(GoalManagerTest, CreateGoal) {
    auto goal = manager.createGoal("Test Goal", "A test goal description");

    EXPECT_FALSE(goal->getId().empty());
    EXPECT_EQ(goal->getName(), "Test Goal");
    EXPECT_EQ(goal->getDescription(), "A test goal description");
    EXPECT_EQ(goal->getStatus(), GoalStatus::PENDING);
    EXPECT_EQ(goal->getPriority(), GoalPriority::NORMAL);
}

TEST_F(GoalManagerTest, CreateGoalWithPriority) {
    auto goal = manager.createGoal("Critical Goal", "Critical priority",
                                   GoalPriority::CRITICAL, GoalType::ACHIEVEMENT);

    EXPECT_EQ(goal->getPriority(), GoalPriority::CRITICAL);
    EXPECT_EQ(goal->getType(), GoalType::ACHIEVEMENT);
}

TEST_F(GoalManagerTest, GoalProgress) {
    auto goal = manager.createGoal("Progress Test", "Test progress tracking");

    goal->setProgress(0.5);
    EXPECT_DOUBLE_EQ(goal->getProgress(), 0.5);

    goal->incrementProgress(0.25);
    EXPECT_DOUBLE_EQ(goal->getProgress(), 0.75);

    // Test clamping
    goal->setProgress(1.5);
    EXPECT_DOUBLE_EQ(goal->getProgress(), 1.0);

    goal->setProgress(-0.5);
    EXPECT_DOUBLE_EQ(goal->getProgress(), 0.0);
}

TEST_F(GoalManagerTest, GoalStatus) {
    auto goal = manager.createGoal("Status Test", "Test status transitions");

    EXPECT_FALSE(goal->isActive());
    EXPECT_FALSE(goal->isCompleted());
    EXPECT_FALSE(goal->isBlocked());

    goal->setStatus(GoalStatus::ACTIVE);
    EXPECT_TRUE(goal->isActive());

    goal->setStatus(GoalStatus::COMPLETED);
    EXPECT_TRUE(goal->isCompleted());
    EXPECT_DOUBLE_EQ(goal->getProgress(), 1.0);
}

TEST_F(GoalManagerTest, GoalTags) {
    auto goal = manager.createGoal("Tagged Goal", "Test tags");

    goal->addTag("important");
    goal->addTag("work");

    EXPECT_TRUE(goal->hasTag("important"));
    EXPECT_TRUE(goal->hasTag("work"));
    EXPECT_FALSE(goal->hasTag("personal"));

    goal->removeTag("work");
    EXPECT_FALSE(goal->hasTag("work"));
}

TEST_F(GoalManagerTest, GoalMetadata) {
    auto goal = manager.createGoal("Metadata Goal", "Test metadata");

    goal->setMetadata("category", "development");
    goal->setMetadata("owner", "team-a");

    auto category = goal->getMetadata("category");
    EXPECT_TRUE(category.has_value());
    EXPECT_EQ(category.value(), "development");

    auto nonexistent = goal->getMetadata("nonexistent");
    EXPECT_FALSE(nonexistent.has_value());
}

TEST_F(GoalManagerTest, GoalDeadline) {
    auto goal = manager.createGoal("Deadline Goal", "Test deadline");

    EXPECT_FALSE(goal->hasDeadline());
    EXPECT_FALSE(goal->isOverdue());

    // Set deadline in the past
    auto pastDeadline = std::chrono::system_clock::now() - std::chrono::hours(1);
    goal->setDeadline(pastDeadline);

    EXPECT_TRUE(goal->hasDeadline());
    EXPECT_TRUE(goal->isOverdue());

    // Set deadline in the future
    auto futureDeadline = std::chrono::system_clock::now() + std::chrono::hours(24);
    goal->setDeadline(futureDeadline);

    EXPECT_FALSE(goal->isOverdue());
    EXPECT_GT(goal->getTimeRemaining().count(), 0);
}

// ==============================================================================
// GOAL MANAGER TESTS
// ==============================================================================

TEST_F(GoalManagerTest, GetGoal) {
    auto created = manager.createGoal("Retrieve Test", "Test goal retrieval");

    auto retrieved = manager.getGoal(created->getId());
    EXPECT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getId(), created->getId());

    auto notFound = manager.getGoal("nonexistent-id");
    EXPECT_EQ(notFound, nullptr);
}

TEST_F(GoalManagerTest, RemoveGoal) {
    auto goal = manager.createGoal("Remove Test", "Test goal removal");
    std::string goalId = goal->getId();

    EXPECT_TRUE(manager.hasGoal(goalId));
    EXPECT_TRUE(manager.removeGoal(goalId));
    EXPECT_FALSE(manager.hasGoal(goalId));
    EXPECT_FALSE(manager.removeGoal(goalId)); // Already removed
}

TEST_F(GoalManagerTest, GetGoalsByStatus) {
    manager.createGoal("Pending 1", "Description")->setStatus(GoalStatus::PENDING);
    manager.createGoal("Pending 2", "Description")->setStatus(GoalStatus::PENDING);
    manager.createGoal("Active 1", "Description")->setStatus(GoalStatus::ACTIVE);

    auto pending = manager.getGoalsByStatus(GoalStatus::PENDING);
    EXPECT_EQ(pending.size(), 2);

    auto active = manager.getGoalsByStatus(GoalStatus::ACTIVE);
    EXPECT_EQ(active.size(), 1);
}

TEST_F(GoalManagerTest, GetGoalsByPriority) {
    manager.createGoal("Critical", "Description", GoalPriority::CRITICAL, GoalType::ACHIEVEMENT);
    manager.createGoal("High 1", "Description", GoalPriority::HIGH, GoalType::ACHIEVEMENT);
    manager.createGoal("High 2", "Description", GoalPriority::HIGH, GoalType::ACHIEVEMENT);

    auto critical = manager.getGoalsByPriority(GoalPriority::CRITICAL);
    EXPECT_EQ(critical.size(), 1);

    auto high = manager.getGoalsByPriority(GoalPriority::HIGH);
    EXPECT_EQ(high.size(), 2);
}

TEST_F(GoalManagerTest, GetGoalsByTag) {
    auto g1 = manager.createGoal("Goal 1", "Description");
    auto g2 = manager.createGoal("Goal 2", "Description");
    auto g3 = manager.createGoal("Goal 3", "Description");

    g1->addTag("work");
    g2->addTag("work");
    g3->addTag("personal");

    auto work = manager.getGoalsByTag("work");
    EXPECT_EQ(work.size(), 2);

    auto personal = manager.getGoalsByTag("personal");
    EXPECT_EQ(personal.size(), 1);
}

// ==============================================================================
// DEPENDENCY TESTS
// ==============================================================================

TEST_F(GoalManagerTest, AddDependency) {
    auto goal1 = manager.createGoal("Goal 1", "First goal");
    auto goal2 = manager.createGoal("Goal 2", "Second goal");

    EXPECT_TRUE(manager.addDependency(goal2->getId(), goal1->getId()));
    EXPECT_TRUE(goal2->hasDependencies());
    EXPECT_FALSE(goal1->hasDependencies());
}

TEST_F(GoalManagerTest, DependenciesMet) {
    auto goal1 = manager.createGoal("Goal 1", "First goal");
    auto goal2 = manager.createGoal("Goal 2", "Depends on goal 1");

    manager.addDependency(goal2->getId(), goal1->getId());

    EXPECT_FALSE(manager.areDependenciesMet(goal2->getId()));

    manager.completeGoal(goal1->getId());

    EXPECT_TRUE(manager.areDependenciesMet(goal2->getId()));
}

TEST_F(GoalManagerTest, CyclicDependencyPrevention) {
    auto goal1 = manager.createGoal("Goal 1", "First");
    auto goal2 = manager.createGoal("Goal 2", "Second");
    auto goal3 = manager.createGoal("Goal 3", "Third");

    EXPECT_TRUE(manager.addDependency(goal2->getId(), goal1->getId()));
    EXPECT_TRUE(manager.addDependency(goal3->getId(), goal2->getId()));

    // This would create a cycle: goal1 -> goal2 -> goal3 -> goal1
    EXPECT_FALSE(manager.addDependency(goal1->getId(), goal3->getId()));
}

TEST_F(GoalManagerTest, SelfDependencyPrevention) {
    auto goal = manager.createGoal("Self Goal", "Cannot depend on itself");

    EXPECT_FALSE(manager.addDependency(goal->getId(), goal->getId()));
}

// ==============================================================================
// STATUS TRANSITION TESTS
// ==============================================================================

TEST_F(GoalManagerTest, CompleteGoal) {
    auto goal = manager.createGoal("Complete Test", "Test completion");
    goal->setStatus(GoalStatus::ACTIVE);

    EXPECT_TRUE(manager.completeGoal(goal->getId()));
    EXPECT_EQ(goal->getStatus(), GoalStatus::COMPLETED);
    EXPECT_TRUE(goal->getCompletedAt().has_value());
}

TEST_F(GoalManagerTest, FailGoal) {
    auto goal = manager.createGoal("Fail Test", "Test failure");

    EXPECT_TRUE(manager.failGoal(goal->getId(), "Test reason"));
    EXPECT_EQ(goal->getStatus(), GoalStatus::FAILED);

    auto reason = goal->getMetadata("failure_reason");
    EXPECT_TRUE(reason.has_value());
    EXPECT_EQ(reason.value(), "Test reason");
}

TEST_F(GoalManagerTest, BlockedGoalUnblocking) {
    auto goal1 = manager.createGoal("Dependency", "Must complete first");
    auto goal2 = manager.createGoal("Blocked", "Waits for dependency");

    manager.addDependency(goal2->getId(), goal1->getId());
    goal2->setStatus(GoalStatus::BLOCKED);

    manager.completeGoal(goal1->getId());

    // goal2 should be unblocked now
    EXPECT_EQ(goal2->getStatus(), GoalStatus::PENDING);
}

// ==============================================================================
// GOAL SELECTION TESTS
// ==============================================================================

TEST_F(GoalManagerTest, SelectNextGoal) {
    auto lowPriority = manager.createGoal("Low", "Low priority",
                                          GoalPriority::LOW, GoalType::ACHIEVEMENT);
    auto highPriority = manager.createGoal("High", "High priority",
                                           GoalPriority::HIGH, GoalType::ACHIEVEMENT);
    auto critical = manager.createGoal("Critical", "Critical priority",
                                       GoalPriority::CRITICAL, GoalType::ACHIEVEMENT);

    auto next = manager.selectNextGoal();
    EXPECT_NE(next, nullptr);
    EXPECT_EQ(next->getPriority(), GoalPriority::CRITICAL);
}

TEST_F(GoalManagerTest, GetTopGoals) {
    for (int i = 0; i < 5; i++) {
        manager.createGoal("Goal " + std::to_string(i), "Description");
    }

    auto top3 = manager.getTopGoals(3);
    EXPECT_EQ(top3.size(), 3);
}

// ==============================================================================
// STATISTICS TESTS
// ==============================================================================

TEST_F(GoalManagerTest, GoalStatistics) {
    manager.createGoal("Goal 1", "Description")->setStatus(GoalStatus::COMPLETED);
    manager.createGoal("Goal 2", "Description")->setStatus(GoalStatus::ACTIVE);
    manager.createGoal("Goal 3", "Description")->setStatus(GoalStatus::PENDING);

    EXPECT_EQ(manager.getTotalGoalCount(), 3);
    EXPECT_EQ(manager.getActiveGoalCount(), 1);
    EXPECT_EQ(manager.getCompletedGoalCount(), 1);

    auto counts = manager.getGoalCountByStatus();
    EXPECT_EQ(counts[GoalStatus::COMPLETED], 1);
    EXPECT_EQ(counts[GoalStatus::ACTIVE], 1);
    EXPECT_EQ(counts[GoalStatus::PENDING], 1);
}

// ==============================================================================
// UTILITY FUNCTION TESTS
// ==============================================================================

TEST(GoalUtilityTest, StatusToString) {
    EXPECT_EQ(goalStatusToString(GoalStatus::PENDING), "PENDING");
    EXPECT_EQ(goalStatusToString(GoalStatus::COMPLETED), "COMPLETED");
    EXPECT_EQ(goalStatusToString(GoalStatus::FAILED), "FAILED");
}

TEST(GoalUtilityTest, StringToStatus) {
    EXPECT_EQ(stringToGoalStatus("PENDING"), GoalStatus::PENDING);
    EXPECT_EQ(stringToGoalStatus("COMPLETED"), GoalStatus::COMPLETED);
    EXPECT_EQ(stringToGoalStatus("INVALID"), GoalStatus::PENDING);
}

TEST(GoalUtilityTest, PriorityToString) {
    EXPECT_EQ(goalPriorityToString(GoalPriority::CRITICAL), "CRITICAL");
    EXPECT_EQ(goalPriorityToString(GoalPriority::NORMAL), "NORMAL");
}

TEST(GoalUtilityTest, TypeToString) {
    EXPECT_EQ(goalTypeToString(GoalType::ACHIEVEMENT), "ACHIEVEMENT");
    EXPECT_EQ(goalTypeToString(GoalType::MAINTENANCE), "MAINTENANCE");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
