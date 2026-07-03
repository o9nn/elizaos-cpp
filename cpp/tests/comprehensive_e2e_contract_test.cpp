#include <gtest/gtest.h>

#include "elizaos/comprehensive_e2e.hpp"

using elizaos::comprehensive_e2e::PipelineSnapshot;
using elizaos::comprehensive_e2e::Stage;
using elizaos::comprehensive_e2e::makeStageResult;
using elizaos::comprehensive_e2e::toString;

TEST(ComprehensiveE2EContractTest, StageNamesAreStableForDiagnostics) {
    EXPECT_STREQ(toString(Stage::Memory), "memory");
    EXPECT_STREQ(toString(Stage::Browser), "browser");
    EXPECT_STREQ(toString(Stage::Conversation), "conversation");
    EXPECT_STREQ(toString(Stage::Knowledge), "knowledge");
    EXPECT_STREQ(toString(Stage::Character), "character");
    EXPECT_STREQ(toString(Stage::Communication), "communication");
    EXPECT_STREQ(toString(Stage::Hat), "hat");
    EXPECT_STREQ(toString(Stage::Mcp), "mcp");
    EXPECT_STREQ(toString(Stage::Integration), "integration");
}

TEST(ComprehensiveE2EContractTest, StageResultNormalizesNegativeLatency) {
    const auto result = makeStageResult(Stage::Memory, "memory-retention", true, "ok", -5.0);

    EXPECT_EQ(result.stage, Stage::Memory);
    EXPECT_EQ(result.name, "memory-retention");
    EXPECT_TRUE(result.passed);
    EXPECT_EQ(result.detail, "ok");
    EXPECT_DOUBLE_EQ(result.latency_ms, 0.0);
}

TEST(ComprehensiveE2EContractTest, EmptySnapshotDoesNotPassAccidentally) {
    PipelineSnapshot snapshot;

    EXPECT_EQ(snapshot.totalCount(), 0u);
    EXPECT_EQ(snapshot.passedCount(), 0u);
    EXPECT_EQ(snapshot.failedCount(), 0u);
    EXPECT_FALSE(snapshot.passed());
    EXPECT_NE(snapshot.summary().find("0/0 stages passed"), std::string::npos);
}

TEST(ComprehensiveE2EContractTest, SnapshotReportsPassingAutonomyPipeline) {
    PipelineSnapshot snapshot;
    snapshot.add(Stage::Memory, "agent-memory-store-recall", true, "episodic memory available", 1.25);
    snapshot.add(Stage::Conversation, "eliza-response-selection", true, "response generated", 0.5);
    snapshot.add(Stage::Knowledge, "knowledge-grounded-answer", true, "retrieval succeeded", 2.0);
    snapshot.add(Stage::Integration, "cognitive-loop", true, "loop closed", 3.75);

    EXPECT_EQ(snapshot.totalCount(), 4u);
    EXPECT_EQ(snapshot.passedCount(), 4u);
    EXPECT_EQ(snapshot.failedCount(), 0u);
    EXPECT_TRUE(snapshot.passed());
    EXPECT_NE(snapshot.summary().find("4/4 stages passed"), std::string::npos);
    ASSERT_EQ(snapshot.stages().size(), 4u);
    EXPECT_EQ(snapshot.stages().front().name, "agent-memory-store-recall");
}

TEST(ComprehensiveE2EContractTest, SnapshotSummarizesFailedStageNames) {
    PipelineSnapshot snapshot;
    snapshot.add(Stage::Memory, "agent-memory-store-recall", true);
    snapshot.add(Stage::Mcp, "tool-dispatch", false, "transport unavailable");
    snapshot.add(Stage::Integration, "cognitive-loop", false, "loop did not converge");

    EXPECT_EQ(snapshot.totalCount(), 3u);
    EXPECT_EQ(snapshot.passedCount(), 1u);
    EXPECT_EQ(snapshot.failedCount(), 2u);
    EXPECT_FALSE(snapshot.passed());

    const auto summary = snapshot.summary();
    EXPECT_NE(summary.find("1/3 stages passed"), std::string::npos);
    EXPECT_NE(summary.find("mcp:tool-dispatch"), std::string::npos);
    EXPECT_NE(summary.find("integration:cognitive-loop"), std::string::npos);
}
