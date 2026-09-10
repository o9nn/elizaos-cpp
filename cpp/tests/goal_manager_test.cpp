// goal_manager_test.cpp - Tests for the Goal Management System

#include <gtest/gtest.h>
#include "elizaos/goal_manager.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <future>
#include <limits>
#include <set>
#include <thread>
#include <vector>

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
    EXPECT_NEAR(goal->getProgress(), 0.5, 0.001);

    goal->incrementProgress(0.25);
    EXPECT_NEAR(goal->getProgress(), 0.75, 0.001);

    // Test clamping
    goal->setProgress(1.5);
    EXPECT_NEAR(goal->getProgress(), 1.0, 0.001);

    goal->setProgress(-0.5);
    EXPECT_NEAR(goal->getProgress(), 0.0, 0.001);
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
    EXPECT_NEAR(goal->getProgress(), 1.0, 0.001);
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

    EXPECT_THROW(goal->setDeadline(goal->getCreatedAt() - std::chrono::seconds(1)),
                 std::invalid_argument);

    // A deadline may pass after it was validly established.
    goal->setDeadline(std::chrono::system_clock::now() + std::chrono::milliseconds(5));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
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


TEST_F(GoalManagerTest, SerializeDeserializeRoundTripRestoresGoalState) {
    auto active = manager.createGoal("Investigate sensor drift", "Normalize IMU bias");
    active->setPriority(GoalPriority::HIGH);
    manager.activateGoal(active->getId());
    manager.updateProgress(active->getId(), 0.42);

    auto completed = manager.createGoal("Calibrate servo", "Verify motion envelope");
    completed->setPriority(GoalPriority::LOW);
    manager.completeGoal(completed->getId());

    const std::string snapshot = manager.serialize();

    GoalManager restored;
    ASSERT_TRUE(restored.deserialize(snapshot));
    EXPECT_EQ(restored.getTotalGoalCount(), 2);

    auto restoredActive = restored.getGoal(active->getId());
    ASSERT_NE(restoredActive, nullptr);
    EXPECT_EQ(restoredActive->getName(), "Investigate sensor drift");
    EXPECT_EQ(restoredActive->getStatus(), GoalStatus::IN_PROGRESS);
    EXPECT_EQ(restoredActive->getPriority(), GoalPriority::HIGH);
    EXPECT_NEAR(restoredActive->getProgress(), 0.42, 1e-9);

    auto restoredCompleted = restored.getGoal(completed->getId());
    ASSERT_NE(restoredCompleted, nullptr);
    EXPECT_EQ(restoredCompleted->getStatus(), GoalStatus::COMPLETED);
    EXPECT_EQ(restoredCompleted->getPriority(), GoalPriority::LOW);
    EXPECT_NEAR(restoredCompleted->getProgress(), 1.0, 1e-9);
}

TEST_F(GoalManagerTest, DeserializeRejectsMalformedInputWithoutChangingExistingGoals) {
    auto existing = manager.createGoal("Keep me", "Existing state should survive failed loads");
    const UUID existingId = existing->getId();

    EXPECT_FALSE(manager.deserialize(""));
    EXPECT_FALSE(manager.deserialize("GOALS:not_a_count\n"));
    EXPECT_FALSE(manager.deserialize("GOALS:1\nGOAL|id|name|ACTIVE|HIGH|not_a_number\n"));
    EXPECT_FALSE(manager.deserialize("GOALS:2\nGOAL|id|name|ACTIVE|HIGH|0.1\n"));

    EXPECT_EQ(manager.getTotalGoalCount(), 1);
    EXPECT_NE(manager.getGoal(existingId), nullptr);
}


TEST_F(GoalManagerTest, RejectsInvalidFieldsAndNonFiniteNumbers) {
    EXPECT_THROW(manager.createGoal("", "description"), std::invalid_argument);
    EXPECT_THROW(manager.createGoal(std::string(Goal::MAX_NAME_LENGTH + 1, 'n'), "description"),
                 std::invalid_argument);
    auto goal = manager.createGoal("Validated", "bounded fields");
    EXPECT_THROW(goal->setProgress(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
    EXPECT_THROW(goal->setPriority(static_cast<GoalPriority>(99)), std::invalid_argument);
    EXPECT_THROW(goal->setReward(std::numeric_limits<double>::infinity()), std::invalid_argument);
    EXPECT_THROW(goal->setEstimatedEffort(0.0), std::invalid_argument);
    EXPECT_THROW(goal->addActualEffort(-1.0), std::invalid_argument);
    EXPECT_THROW(goal->addTag(""), std::invalid_argument);
    EXPECT_THROW(goal->setMetadata("", "value"), std::invalid_argument);
    EXPECT_FALSE(manager.updateProgress(goal->getId(), std::numeric_limits<double>::infinity()));
}

TEST_F(GoalManagerTest, DeterministicMonotonicIdsAndSortedSnapshots) {
    auto first = manager.createGoal("First", "id order");
    auto second = manager.createGoal("Second", "id order");
    auto third = manager.createGoal("Third", "id order");
    EXPECT_LT(first->getId(), second->getId());
    EXPECT_LT(second->getId(), third->getId());
    const auto snapshots = manager.getAllGoalSnapshots();
    ASSERT_EQ(snapshots.size(), 3u);
    EXPECT_TRUE(std::is_sorted(snapshots.begin(), snapshots.end(),
        [](const GoalSnapshot& a, const GoalSnapshot& b) { return a.id < b.id; }));
    const auto retained = manager.getGoalSnapshot(second->getId());
    ASSERT_TRUE(retained.has_value());
    EXPECT_TRUE(manager.removeGoal(second->getId()));
    EXPECT_EQ(retained->name, "Second");
}

TEST_F(GoalManagerTest, DependenciesDriveSchedulingAndCompletionEvidence) {
    auto prerequisite = manager.createGoal("Evidence", "must complete first", GoalPriority::LOW,
                                           GoalType::ACHIEVEMENT);
    auto dependent = manager.createGoal("Dependent", "high but blocked", GoalPriority::CRITICAL,
                                        GoalType::ACHIEVEMENT);
    ASSERT_TRUE(manager.addDependency(dependent->getId(), prerequisite->getId()));
    EXPECT_EQ(manager.selectNextGoal()->getId(), prerequisite->getId());
    EXPECT_FALSE(manager.completeGoal(dependent->getId()));
    EXPECT_FALSE(manager.updateProgress(dependent->getId(), 0.5));
    ASSERT_TRUE(manager.completeGoal(prerequisite->getId()));
    EXPECT_EQ(manager.selectNextGoal()->getId(), dependent->getId());
    EXPECT_TRUE(manager.updateProgress(dependent->getId(), 0.5));
    EXPECT_EQ(dependent->getStatus(), GoalStatus::PENDING);
    EXPECT_TRUE(manager.activateGoal(dependent->getId()));
    EXPECT_TRUE(manager.updateProgress(dependent->getId(), 0.75));
    EXPECT_EQ(dependent->getStatus(), GoalStatus::IN_PROGRESS);
}

TEST_F(GoalManagerTest, MissingDuplicateSelfAndCyclicDependenciesAreRejected) {
    auto a = manager.createGoal("A", "dependency");
    auto b = manager.createGoal("B", "dependency");
    auto c = manager.createGoal("C", "dependency");
    EXPECT_FALSE(manager.addDependency(a->getId(), "missing"));
    EXPECT_FALSE(manager.addDependency(a->getId(), a->getId()));
    ASSERT_TRUE(manager.addDependency(b->getId(), a->getId()));
    EXPECT_FALSE(manager.addDependency(b->getId(), a->getId()));
    ASSERT_TRUE(manager.addDependency(c->getId(), b->getId()));
    EXPECT_FALSE(manager.addDependency(a->getId(), c->getId()));
    EXPECT_FALSE(manager.removeDependency(c->getId(), a->getId()));
}

TEST_F(GoalManagerTest, InvalidTerminalTransitionsAndMutationsAreRejected) {
    auto goal = manager.createGoal("Terminal", "immutable when done");
    ASSERT_TRUE(manager.completeGoal(goal->getId()));
    EXPECT_FALSE(manager.completeGoal(goal->getId()));
    EXPECT_FALSE(manager.activateGoal(goal->getId()));
    EXPECT_FALSE(manager.failGoal(goal->getId()));
    EXPECT_FALSE(manager.cancelGoal(goal->getId()));
    EXPECT_FALSE(manager.blockGoal(goal->getId()));
    EXPECT_FALSE(manager.updateProgress(goal->getId(), 0.5));
    EXPECT_THROW(goal->setStatus(GoalStatus::ACTIVE), std::logic_error);
    EXPECT_THROW(goal->setProgress(0.5), std::logic_error);
}

TEST_F(GoalManagerTest, HierarchyRejectsCyclesAndRecursivelyCancelsAndRemoves) {
    auto root = manager.createGoal("Root", "hierarchy");
    auto child = manager.createGoal("Child", "hierarchy");
    auto leaf = manager.createGoal("Leaf", "hierarchy");
    ASSERT_TRUE(manager.addSubGoal(root->getId(), child->getId()));
    ASSERT_TRUE(manager.addSubGoal(child->getId(), leaf->getId()));
    EXPECT_FALSE(manager.addSubGoal(leaf->getId(), root->getId()));
    EXPECT_FALSE(manager.completeGoal(root->getId()));
    EXPECT_TRUE(manager.cancelGoal(root->getId()));
    EXPECT_EQ(child->getStatus(), GoalStatus::CANCELLED);
    EXPECT_EQ(leaf->getStatus(), GoalStatus::CANCELLED);

    GoalManager removal;
    auto removalRoot = removal.createGoal("Removal root", "recursive");
    auto removalChild = removal.createGoal("Removal child", "recursive");
    auto dependent = removal.createGoal("Dependent", "cannot survive prerequisite removal");
    ASSERT_TRUE(removal.addSubGoal(removalRoot->getId(), removalChild->getId()));
    ASSERT_TRUE(removal.addDependency(dependent->getId(), removalChild->getId()));
    EXPECT_TRUE(removal.removeGoal(removalRoot->getId()));
    EXPECT_EQ(removal.getTotalGoalCount(), 1u);
    EXPECT_EQ(dependent->getStatus(), GoalStatus::CANCELLED);
}

TEST_F(GoalManagerTest, CallbackExceptionsAreIsolatedAndCallbacksMayReenter) {
    std::atomic<int> calls{0};
    manager.onGoalCreated([](const std::shared_ptr<Goal>&) { throw std::runtime_error("observer"); });
    manager.onGoalCreated([this, &calls](const std::shared_ptr<Goal>& goal) {
        EXPECT_TRUE(manager.hasGoal(goal->getId()));
        manager.onGoalProgress([&calls](const std::shared_ptr<Goal>&) { ++calls; });
        ++calls;
    });
    auto goal = manager.createGoal("Reentrant", "callbacks outside lock");
    EXPECT_EQ(calls.load(), 1);
    EXPECT_TRUE(manager.updateProgress(goal->getId(), 0.25));
    EXPECT_EQ(calls.load(), 2);
}

TEST_F(GoalManagerTest, FullStatePersistenceRoundTripsDeterministically) {
    auto parent = manager.createGoal("Parent with | delimiter", "quoted\ndescription",
                                     GoalPriority::HIGH, GoalType::IMPROVEMENT);
    auto child = manager.createGoal("Child", "state", GoalPriority::LOW, GoalType::EXPLORATION);
    auto prerequisite = manager.createGoal("Prerequisite", "state");
    parent->addTag("zeta");
    parent->addTag("alpha");
    parent->setMetadata("owner", "agent | one");
    parent->setMetadata("empty", "");
    parent->setReward(3.5);
    parent->setEstimatedEffort(2.0);
    parent->addActualEffort(0.75);
    parent->setDeadline(std::chrono::system_clock::now() + std::chrono::hours(2));
    ASSERT_TRUE(manager.addSubGoal(parent->getId(), child->getId()));
    ASSERT_TRUE(manager.addDependency(child->getId(), prerequisite->getId()));
    ASSERT_TRUE(manager.completeGoal(prerequisite->getId()));
    ASSERT_TRUE(manager.activateGoal(child->getId()));
    ASSERT_TRUE(manager.updateProgress(child->getId(), 0.4));

    const std::string serialized = manager.serialize();
    GoalManager restored;
    ASSERT_TRUE(restored.deserialize(serialized));
    EXPECT_EQ(restored.serialize(), serialized);
    const auto restoredParent = restored.getGoalSnapshot(parent->getId());
    ASSERT_TRUE(restoredParent.has_value());
    EXPECT_EQ(restoredParent->description, "quoted\ndescription");
    EXPECT_EQ(restoredParent->tags, (std::vector<std::string>{"alpha", "zeta"}));
    EXPECT_EQ(restoredParent->metadata,
              (std::vector<std::pair<std::string, std::string>>{{"empty", ""}, {"owner", "agent | one"}}));
    EXPECT_EQ(restoredParent->subGoalIds, (std::vector<UUID>{child->getId()}));
    EXPECT_DOUBLE_EQ(restoredParent->reward, 3.5);
    auto later = restored.createGoal("Later", "id must not collide");
    EXPECT_GT(later->getId(), prerequisite->getId());
}

TEST_F(GoalManagerTest, MalformedGraphPersistenceRollsBack) {
    auto existing = manager.createGoal("Keep", "rollback");
    std::string snapshot = manager.serialize();
    const auto position = snapshot.find("DEPS 0");
    ASSERT_NE(position, std::string::npos);
    snapshot.replace(position, 6, "DEPS 1\nDEP \"missing\"");
    EXPECT_FALSE(manager.deserialize(snapshot));
    EXPECT_TRUE(manager.hasGoal(existing->getId()));
    EXPECT_EQ(manager.getTotalGoalCount(), 1u);
}

TEST_F(GoalManagerTest, ConcurrentCreateUpdateQueryAndRemoveIsRaceSafe) {
    constexpr int threadCount = 8;
    constexpr int goalsPerThread = 40;
    std::vector<std::thread> creators;
    for (int thread = 0; thread < threadCount; ++thread) {
        creators.emplace_back([this, thread, goalsPerThread] {
            for (int index = 0; index < goalsPerThread; ++index) {
                manager.createGoal("goal-" + std::to_string(thread) + "-" + std::to_string(index),
                                   "concurrent");
            }
        });
    }
    for (auto& creator : creators) creator.join();
    ASSERT_EQ(manager.getTotalGoalCount(), static_cast<std::size_t>(threadCount * goalsPerThread));

    const auto goals = manager.getAllGoals();
    std::atomic<bool> stopQueries{false};
    std::thread query([this, &stopQueries] {
        while (!stopQueries.load()) {
            const auto snapshots = manager.getAllGoalSnapshots();
            EXPECT_TRUE(std::is_sorted(snapshots.begin(), snapshots.end(),
                [](const GoalSnapshot& a, const GoalSnapshot& b) { return a.id < b.id; }));
            (void)manager.getOverallProgress();
            (void)manager.selectNextGoal();
        }
    });
    std::vector<std::thread> workers;
    for (int thread = 0; thread < threadCount; ++thread) {
        workers.emplace_back([this, &goals, thread, threadCount] {
            for (std::size_t index = static_cast<std::size_t>(thread); index < goals.size();
                 index += threadCount) {
                EXPECT_TRUE(manager.updateProgress(goals[index]->getId(), 0.5));
                EXPECT_TRUE(manager.removeGoal(goals[index]->getId()));
            }
        });
    }
    for (auto& worker : workers) worker.join();
    stopQueries = true;
    query.join();
    EXPECT_EQ(manager.getTotalGoalCount(), 0u);
}

TEST(GoalManagerTeardownTest, RepeatedConstructionCallbacksAndDestructionComplete) {
    auto work = std::async(std::launch::async, [] {
        for (int iteration = 0; iteration < 500; ++iteration) {
            GoalManager local;
            local.onGoalCreated([&local](const std::shared_ptr<Goal>& goal) {
                EXPECT_TRUE(local.hasGoal(goal->getId()));
            });
            local.createGoal("teardown", "no detached work");
        }
    });
    EXPECT_EQ(work.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    work.get();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
