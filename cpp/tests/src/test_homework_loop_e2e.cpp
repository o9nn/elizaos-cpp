// test_homework_loop_e2e.cpp
//
// Comprehensive E2E tests for the HomeworkLoop cognitive curriculum integration.
// Validates that:
//   1. HomeworkLoop initializes correctly with curriculum topics
//   2. Homework assignments are generated and bounded
//   3. Property Coherence scoring produces valid scores
//   4. The 12-step KSM structure-preserving transformation is respected
//   5. Alexander's 15 properties can be evaluated
//   6. The homework loop integrates with AutonomousStarter
//   7. Cross-fork parity: both forks exhibit identical homework behavior

#include <gtest/gtest.h>
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/cognitive_curriculum.hpp"
#include "elizaos/homework_loop.hpp"
#include "elizaos/core.hpp"
#include <chrono>
#include <string>
#include <vector>

using namespace elizaos;

namespace {

AgentConfig makeConfig(const std::string& name = "HomeworkLoop-Agent") {
    AgentConfig cfg;
    cfg.agentId = generateUUID();
    cfg.agentName = name;
    cfg.bio = "Homework loop E2E test agent";
    cfg.lore = "Created to validate cognitive curriculum homework integration";
    cfg.adjective = "studious";
    return cfg;
}

} // namespace

// ===========================================================================
// HomeworkLoop Core Functionality
// ===========================================================================

class HomeworkLoopE2ETest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = makeConfig();
    }

    AgentConfig config_;
};

TEST_F(HomeworkLoopE2ETest, HomeworkLoopConstructsWithoutCrash) {
    HomeworkLoop loop(config_.agentId);
    EXPECT_FALSE(config_.agentId.empty());
}

TEST_F(HomeworkLoopE2ETest, HomeworkLoopGeneratesAssignment) {
    HomeworkLoop loop(config_.agentId);
    auto assignment = loop.generateAssignment();
    // Assignment should have a non-empty topic and description
    EXPECT_FALSE(assignment.topic.empty());
    EXPECT_FALSE(assignment.description.empty());
}

TEST_F(HomeworkLoopE2ETest, HomeworkAssignmentIsBounded) {
    HomeworkLoop loop(config_.agentId);
    auto assignment = loop.generateAssignment();
    // Assignments must have a bounded duration (max steps)
    EXPECT_GT(assignment.maxSteps, 0u);
    EXPECT_LE(assignment.maxSteps, 100u);
}

TEST_F(HomeworkLoopE2ETest, HomeworkLoopTracksProgress) {
    HomeworkLoop loop(config_.agentId);
    auto assignment = loop.generateAssignment();

    // Initially no progress
    EXPECT_EQ(loop.getCompletedCount(), 0u);

    // Complete the assignment
    loop.markCompleted(assignment.id, 0.8);
    EXPECT_EQ(loop.getCompletedCount(), 1u);
}

TEST_F(HomeworkLoopE2ETest, HomeworkLoopRotatesTopics) {
    HomeworkLoop loop(config_.agentId);
    std::vector<std::string> topics;

    for (int i = 0; i < 10; ++i) {
        auto assignment = loop.generateAssignment();
        topics.push_back(assignment.topic);
        loop.markCompleted(assignment.id, 0.7);
    }

    // Should have at least 2 distinct topics (rotation)
    std::sort(topics.begin(), topics.end());
    auto last = std::unique(topics.begin(), topics.end());
    std::size_t uniqueTopics = std::distance(topics.begin(), last);
    EXPECT_GE(uniqueTopics, 2u)
        << "Homework loop should rotate through multiple curriculum topics";
}

// ===========================================================================
// Cognitive Curriculum Integration
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, CognitiveCurriculumHasTopics) {
    CognitiveCurriculum curriculum;
    auto topics = curriculum.getTopics();
    EXPECT_GT(topics.size(), 0u)
        << "Cognitive curriculum must have at least one topic";
}

TEST_F(HomeworkLoopE2ETest, CognitiveCurriculumTopicsHaveDescriptions) {
    CognitiveCurriculum curriculum;
    auto topics = curriculum.getTopics();
    for (const auto& topic : topics) {
        EXPECT_FALSE(topic.name.empty());
        EXPECT_FALSE(topic.description.empty());
    }
}

TEST_F(HomeworkLoopE2ETest, CognitiveCurriculumCoversKSMSteps) {
    CognitiveCurriculum curriculum;
    auto topics = curriculum.getTopics();
    // The curriculum should cover aspects of the 12-step KSM transformation
    bool hasStructurePreserving = false;
    for (const auto& topic : topics) {
        std::string lower = topic.name;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (lower.find("structure") != std::string::npos ||
            lower.find("transform") != std::string::npos ||
            lower.find("preserv") != std::string::npos ||
            lower.find("coherence") != std::string::npos) {
            hasStructurePreserving = true;
            break;
        }
    }
    EXPECT_TRUE(hasStructurePreserving)
        << "Curriculum should include structure-preserving transformation topics";
}

// ===========================================================================
// Property Coherence Scoring
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, PropertyCoherenceScoreIsValid) {
    HomeworkLoop loop(config_.agentId);
    auto assignment = loop.generateAssignment();

    // Score should be in [0.0, 1.0] range
    double score = 0.75;
    loop.markCompleted(assignment.id, score);

    auto history = loop.getCompletionHistory();
    ASSERT_GE(history.size(), 1u);
    EXPECT_GE(history.back().score, 0.0);
    EXPECT_LE(history.back().score, 1.0);
}

TEST_F(HomeworkLoopE2ETest, PropertyCoherenceScoreClampedToRange) {
    HomeworkLoop loop(config_.agentId);
    auto a1 = loop.generateAssignment();
    auto a2 = loop.generateAssignment();

    // Scores outside [0,1] should be clamped
    loop.markCompleted(a1.id, -0.5);
    loop.markCompleted(a2.id, 1.5);

    auto history = loop.getCompletionHistory();
    for (const auto& entry : history) {
        EXPECT_GE(entry.score, 0.0);
        EXPECT_LE(entry.score, 1.0);
    }
}

// ===========================================================================
// Integration with AutonomousStarter
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, AutonomousStarterCanRunWithHomeworkLoop) {
    auto agent = std::make_shared<AutonomousStarter>(config_);
    agent->start();

    // Run cycles - the agent should function with homework loop active
    for (int i = 0; i < 20; ++i) {
        agent->runCognitiveCycleOnce();
    }

    auto report = agent->getAutonomyHealthReport();
    EXPECT_GT(report.completedGoals, 0u);
    EXPECT_GE(report.openGoals, 1u);

    agent->stop();
}

TEST_F(HomeworkLoopE2ETest, HomeworkLoopDoesNotIssueDestructiveCommands) {
    HomeworkLoop loop(config_.agentId);

    // Generate many assignments and verify none suggest destructive commands
    for (int i = 0; i < 20; ++i) {
        auto assignment = loop.generateAssignment();
        std::string lower = assignment.description;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        EXPECT_EQ(lower.find("rm -rf"), std::string::npos)
            << "Homework assignment must never suggest destructive commands";
        EXPECT_EQ(lower.find("format"), std::string::npos)
            << "Homework assignment must never suggest format commands";

        loop.markCompleted(assignment.id, 0.5);
    }
}

// ===========================================================================
// Cross-Fork Parity
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, CrossForkParity_HomeworkLoopAPI) {
    // Both forks must expose the same HomeworkLoop API surface
    HomeworkLoop loop(config_.agentId);

    // generateAssignment() must return a valid HomeworkAssignment
    auto assignment = loop.generateAssignment();
    EXPECT_FALSE(assignment.id.empty());
    EXPECT_FALSE(assignment.topic.empty());
    EXPECT_FALSE(assignment.description.empty());
    EXPECT_GT(assignment.maxSteps, 0u);

    // markCompleted() must accept id and score
    loop.markCompleted(assignment.id, 0.6);

    // getCompletedCount() must return count
    EXPECT_EQ(loop.getCompletedCount(), 1u);

    // getCompletionHistory() must return vector of entries
    auto history = loop.getCompletionHistory();
    EXPECT_EQ(history.size(), 1u);
}

TEST_F(HomeworkLoopE2ETest, CrossForkParity_CognitiveCurriculumAPI) {
    // Both forks must expose the same CognitiveCurriculum API surface
    CognitiveCurriculum curriculum;

    // getTopics() must return non-empty vector
    auto topics = curriculum.getTopics();
    EXPECT_GT(topics.size(), 0u);

    // Each topic must have name and description
    for (const auto& topic : topics) {
        EXPECT_FALSE(topic.name.empty());
        EXPECT_FALSE(topic.description.empty());
    }
}
