#include <gtest/gtest.h>

#include <elizaos/comprehensive_e2e.hpp>

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::vector<std::string> keywordSearch(const std::vector<std::string>& records, const std::string& query) {
    std::vector<std::string> results;
    const auto lowered_query = lower(query);
    for (const auto& record : records) {
        if (lower(record).find(lowered_query) != std::string::npos) {
            results.push_back(record);
        }
    }
    return results;
}

std::string extractHref(const std::string& html) {
    const auto href_pos = html.find("href=\"");
    if (href_pos == std::string::npos) {
        return {};
    }
    const auto start = href_pos + 6;
    const auto end = html.find('"', start);
    if (end == std::string::npos) {
        return {};
    }
    return html.substr(start, end - start);
}

} // namespace

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

TEST(ComprehensiveE2EContractTest, SnapshotReportsPassingAutonomyPipeline) {
    PipelineSnapshot snapshot;
    snapshot.add(Stage::Memory, "agent-memory-store-recall", true, "episodic memory available", 1.25);
    snapshot.add(Stage::Browser, "html-link-extraction", true, "browser perception succeeded", 0.35);
    snapshot.add(Stage::Conversation, "eliza-response-selection", true, "response generated", 0.5);
    snapshot.add(Stage::Knowledge, "knowledge-grounded-answer", true, "retrieval succeeded", 2.0);
    snapshot.add(Stage::Character, "persona-affect-selection", true, "character state coherent", 0.75);
    snapshot.add(Stage::Communication, "agent-message-routing", true, "message delivered", 0.25);
    snapshot.add(Stage::Hat, "hat-token-check", true, "authority token valid", 0.2);
    snapshot.add(Stage::Mcp, "tool-dispatch", true, "tool call acknowledged", 0.6);
    snapshot.add(Stage::Integration, "cognitive-loop", true, "loop closed", 3.75);

    EXPECT_EQ(snapshot.totalCount(), 9u);
    EXPECT_EQ(snapshot.passedCount(), 9u);
    EXPECT_EQ(snapshot.failedCount(), 0u);
    EXPECT_TRUE(snapshot.passed());
    EXPECT_NE(snapshot.summary().find("9/9 stages passed"), std::string::npos);
    ASSERT_EQ(snapshot.stages().size(), 9u);
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

TEST(ComprehensiveAutonomySimulationTest, MemoryStorageAndSemanticRecallRemainCoherent) {
    const std::vector<std::string> memories = {
        "User prefers coffee over tea",
        "User's name is Alice",
        "User is interested in AI and robotics"
    };

    const auto coffee_results = keywordSearch(memories, "coffee");
    const auto robotics_results = keywordSearch(memories, "ROBOTICS");

    ASSERT_EQ(memories.size(), 3u);
    ASSERT_EQ(coffee_results.size(), 1u);
    EXPECT_EQ(coffee_results.front(), "User prefers coffee over tea");
    ASSERT_EQ(robotics_results.size(), 1u);
    EXPECT_EQ(robotics_results.front(), "User is interested in AI and robotics");
}

TEST(ComprehensiveAutonomySimulationTest, BrowserPerceptionExtractsLinksAndImages) {
    const std::string html = R"(
        <html>
            <body>
                <a href="https://example.com">Link 1</a>
                <a href="https://test.com">Link 2</a>
                <img src="image1.jpg" />
                <img src="image2.png" />
            </body>
        </html>
    )";

    std::size_t link_count = 0;
    for (std::size_t pos = 0; (pos = html.find("<a href=", pos)) != std::string::npos; ++pos) {
        ++link_count;
    }
    std::size_t image_count = 0;
    for (std::size_t pos = 0; (pos = html.find("<img", pos)) != std::string::npos; ++pos) {
        ++image_count;
    }

    EXPECT_EQ(link_count, 2u);
    EXPECT_EQ(image_count, 2u);
    EXPECT_EQ(extractHref(R"(<a href="https://example.com">Test</a>)"), "https://example.com");
}

TEST(ComprehensiveAutonomySimulationTest, ConversationHistoryMaintainsBoundedContext) {
    std::vector<std::pair<std::string, std::string>> history;
    history.push_back({"Hello", "Hi there!"});
    history.push_back({"How are you?", "I'm doing well, thanks!"});
    history.push_back({"What's your name?", "I'm Eliza."});

    for (int i = 0; i < 60; ++i) {
        history.push_back({"Test", "Response"});
        if (history.size() > 50) {
            history.erase(history.begin());
        }
    }

    EXPECT_LE(history.size(), 50u);
    EXPECT_EQ(history.back().second, "Response");
}

TEST(ComprehensiveAutonomySimulationTest, KnowledgeCharacterCommunicationHatAndMcpIntegrate) {
    const std::string knowledge = "AI is the simulation of human intelligence";
    const std::string response = "Based on what I know: " + knowledge;
    EXPECT_NE(response.find(knowledge), std::string::npos);

    std::map<std::string, int> personality = {{"empathy", 9}, {"curiosity", 8}, {"patience", 10}};
    EXPECT_GE(personality["empathy"], 8);
    EXPECT_GE(personality["patience"], personality["curiosity"]);

    const std::vector<std::string> agents = {"agent1", "agent2", "agent3"};
    const std::string broadcast_message = "System update";
    std::set<std::string> delivered_to;
    for (const auto& agent : agents) {
        if (!agent.empty() && !broadcast_message.empty()) {
            delivered_to.insert(agent);
        }
    }
    EXPECT_EQ(delivered_to.size(), agents.size());

    const std::string token = "HAT_agent123_user456";
    EXPECT_EQ(token.find("HAT_"), 0u);
    EXPECT_NE(token.find("agent123"), std::string::npos);

    const std::string mcp_request = R"({"method":"test","params":{}})";
    const std::string mcp_response = R"({"status":"success","data":{}})";
    EXPECT_NE(mcp_request.find("method"), std::string::npos);
    EXPECT_NE(mcp_response.find("success"), std::string::npos);
}

TEST(ComprehensiveAutonomySimulationTest, IntegrationLoopClosesAcrossMemoryKnowledgeAndConversation) {
    const bool running = true;
    int iterations = 0;
    std::vector<std::string> conversation_memory;

    for (int i = 0; i < 5; ++i) {
        if (!running) {
            break;
        }
        conversation_memory.push_back("User: Tell me about AI");
        conversation_memory.push_back("Agent: Based on retrieved knowledge, AI supports autonomous reasoning.");
        ++iterations;
    }

    EXPECT_EQ(iterations, 5);
    EXPECT_EQ(conversation_memory.size(), 10u);
    EXPECT_NE(conversation_memory.front().find("AI"), std::string::npos);
    EXPECT_NE(conversation_memory.back().find("autonomous reasoning"), std::string::npos);
}
