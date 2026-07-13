/**
 * Cognitive pipeline end-to-end test.
 *
 * Validates the integration surface required by the eliza-os skill:
 *   - SWE-Agent autonomous solving and SWE-Bench benchmarking
 *   - MCP Gateway in-process discovery, payment, rate limiting, health
 *   - CognitiveBridge cognitive_state / sensory_input / speech_output channels
 *   - Echobeats 12-step heartbeat with three concurrent phased threads
 *
 * The test is fully deterministic: it never opens a network socket and never
 * calls an LLM. It is the contract this repository commits to keeping green.
 */

// Header order: pull the elizaos headers first (which define JsonValue =
// std::unordered_map) and then mcp_gateway.hpp (which defines its own
// MCPJsonValue and never collides). The local gtest shim is included last
// so the shim's macro-friendly version of `EXPECT_*` and `ASSERT_*` is in
// scope when our test bodies are macro-expanded.
#include "elizaos/sweagent.hpp"
#include "elizaos/mcp_gateway.hpp"
#include "elizaos/cognitive_bridge.hpp"
#include "elizaos/characterfile.hpp"
#include "elizaos/knowledge_helpers.hpp"

#include <nlohmann/json.hpp>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;
using namespace std::chrono_literals;

// ===========================================================================
// SWE-Agent depth tests
// ===========================================================================

TEST(SWEAgentDepth, SolvesBugFixIssueProducingFiles) {
    SWEAgent agent("a-1", "deterministic-llm");
    GitHubIssue issue;
    issue.repo = "elizaos/test";
    issue.issueNumber = 42;
    issue.title = "Fix crash in tokenize on empty input";
    issue.description = "tokenize(\"\") segfaults; please fix the bug.";
    issue.labels = {"bug", "cpp"};
    issue.status = "open";

    auto result = agent.solveIssue(issue);
    if (!result.success) {
        ADD_FAILURE();
    }
    ASSERT_TRUE(result.success);
    EXPECT_FALSE(result.filesChanged.empty());
    // Bug fixes should produce both source and header files for cpp issues.
    bool hasSrc = false, hasHdr = false;
    for (const auto& f : result.filesChanged) {
        if (f.find(".cpp") != std::string::npos) hasSrc = true;
        if (f.find(".hpp") != std::string::npos) hasHdr = true;
    }
    EXPECT_TRUE(hasSrc);
    EXPECT_TRUE(hasHdr);
    EXPECT_GE(result.testsRun.size(), 1u);
}

TEST(SWEAgentDepth, SolvesFromDescriptionFeature) {
    SWEAgent agent("a-2", "deterministic-llm");
    CodeContext ctx;
    ctx.language = "cpp";
    auto result = agent.solveFromDescription(
        "Add support for new resolveTokens() helper that normalises whitespace.", ctx);
    ASSERT_TRUE(result.success);
    EXPECT_FALSE(result.filesChanged.empty());
}

TEST(SWEAgentDepth, GeneratedCodeIsLanguageAware) {
    SWEAgent agent("a-3", "deterministic-llm");
    CodeContext py;
    py.language = "python";
    auto pyCode = agent.generateCode("Add utility processChunk", py);
    EXPECT_NE(pyCode.find("def "), std::string::npos);

    CodeContext cpp;
    cpp.language = "cpp";
    auto cppCode = agent.generateCode("Add utility processChunk", cpp);
    EXPECT_NE(cppCode.find("namespace elizaos"), std::string::npos);
}

TEST(SWEAgentDepth, ValidateSolutionRejectsUnbalancedBraces) {
    SWEAgent agent("a-4", "deterministic-llm");
    EXPECT_FALSE(agent.validateSolution("int main() { return 0;", {"test_basic", "test_empty"}));
    EXPECT_TRUE(agent.validateSolution("int main() { return 0; }", {"test_basic", "test_empty"}));
}

TEST(SWEAgentDepth, RepositoryHelpersValidateInputs) {
    SWEAgent agent("a-5", "deterministic-llm");
    EXPECT_FALSE(agent.cloneRepository("", "/tmp/x"));
    EXPECT_FALSE(agent.cloneRepository("not-a-git-url", "/tmp/x"));
    EXPECT_FALSE(agent.cloneRepository("https://github.com/foo/bar.git", ""));
    EXPECT_TRUE(agent.cloneRepository("https://github.com/foo/bar.git", "/tmp/x"));

    EXPECT_FALSE(agent.applyChanges({}));
    EXPECT_FALSE(agent.applyChanges({"a", ""}));
    EXPECT_TRUE(agent.applyChanges({"a", "b"}));

    EXPECT_FALSE(agent.runTests(""));
    EXPECT_TRUE(agent.runTests("ctest"));

    EXPECT_FALSE(agent.createPullRequest("", "x"));
    EXPECT_FALSE(agent.createPullRequest("x", ""));
    EXPECT_TRUE(agent.createPullRequest("Title", "Body"));
}

TEST(SWEBenchDepth, RunsSyntheticBenchmark) {
    SWEAgent agent("b-1", "deterministic-llm");
    SWEBench bench("");
    bench.runBenchmark(agent);
    EXPECT_GT(bench.getSuccessRate(), 0.0f);
    auto report = bench.generateReport();
    EXPECT_NE(report.find("SWE-Bench Report"), std::string::npos);
    EXPECT_NE(report.find("Success rate"), std::string::npos);
}

TEST(SWEAgentManagerDepth, SolvesIssuesInParallel) {
    auto a1 = std::make_shared<SWEAgent>("p-1", "model");
    auto a2 = std::make_shared<SWEAgent>("p-2", "model");
    SWEAgentManager mgr;
    mgr.addAgent(a1);
    mgr.addAgent(a2);
    mgr.setMaxParallelAgents(2);

    std::vector<GitHubIssue> issues;
    for (int i = 0; i < 8; ++i) {
        GitHubIssue issue;
        issue.repo = "demo/parallel";
        issue.issueNumber = 100 + i;
        issue.title = "Add helper" + std::to_string(i);
        issue.description = "Implement helper" + std::to_string(i) + " function.";
        issue.labels = {"feature", "cpp"};
        issues.push_back(issue);
    }

    const auto results = mgr.solveIssuesParallel(issues);
    EXPECT_EQ(results.size(), issues.size());
    int successes = 0;
    for (const auto& r : results) if (r.success) ++successes;
    EXPECT_GT(successes, 0);
}

TEST(SWEAgentManagerDepth, ParallelSolvingPreservesIssueOrderAndAgentHistory) {
    auto a1 = std::make_shared<SWEAgent>("ordered-1", "model");
    auto a2 = std::make_shared<SWEAgent>("ordered-2", "model");
    SWEAgentManager mgr;
    mgr.addAgent(a1);
    mgr.addAgent(a2);
    mgr.setMaxParallelAgents(2);

    std::vector<GitHubIssue> issues;
    for (int i = 0; i < 12; ++i) {
        GitHubIssue issue;
        issue.repo = "demo/ordered-parallel";
        issue.issueNumber = 200 + i;
        issue.title = "Add ordered helper" + std::to_string(i);
        issue.description = "Implement ordered helper" + std::to_string(i) + " function.";
        issue.labels = {"feature", "cpp"};
        issues.push_back(issue);
    }

    const auto results = mgr.solveIssuesParallel(issues);
    ASSERT_EQ(results.size(), issues.size());

    for (size_t i = 0; i < results.size(); ++i) {
        EXPECT_TRUE(results[i].success);
        EXPECT_NE(results[i].description.find(issues[i].title), std::string::npos);
        EXPECT_FALSE(results[i].filesChanged.empty());
        EXPECT_FALSE(results[i].testsRun.empty());
    }

    const auto h1 = a1->getHistory();
    const auto h2 = a2->getHistory();
    EXPECT_GT(h1.size(), 0u);
    EXPECT_GT(h2.size(), 0u);
    EXPECT_EQ(h1.size() + h2.size(), issues.size());

    const auto statuses = mgr.getAgentStatuses();
    ASSERT_EQ(statuses.size(), 2u);
    EXPECT_NE(statuses.at("ordered-1").find("solved"), std::string::npos);
    EXPECT_NE(statuses.at("ordered-2").find("solved"), std::string::npos);
}

// ===========================================================================
// MCP Gateway integration tests
// ===========================================================================

TEST(MCPGatewayDepth, ClientDiscoversToolsExposedByServer) {
    MCPGateway gateway("gw-1");
    MCPServer server("srv-1");

    server.registerTool("echo", "Echoes input back",
                        nlohmann::json::object(),
                        [](const nlohmann::json& in) -> nlohmann::json { return in; });
    server.registerResource("mcp://docs/readme", "text/markdown", "Sample doc");
    server.connectToGateway("gw-1");

    MCPClient client("gw-1", "");
    auto tools = client.discoverTools();
    auto resources = client.discoverResources();
    ASSERT_FALSE(tools.empty());
    ASSERT_FALSE(resources.empty());
    EXPECT_EQ(tools.front().name, "echo");
    EXPECT_EQ(resources.front().uri, "mcp://docs/readme");

    nlohmann::json input = {{"hello", "world"}};
    auto result = client.callTool("srv-1::echo", input);
    EXPECT_EQ(result["hello"], "world");

    server.disconnectFromGateway();
    EXPECT_TRUE(client.discoverTools().empty());
}

TEST(MCPGatewayDepth, SignedPaymentVerifiesAndPaysOut) {
    MCPGateway gateway("gw-2");

    MCPServer server("srv-2");
    server.registerTool("paid", "A paid tool", nlohmann::json::object(),
                        [](const nlohmann::json&) -> nlohmann::json { return {{"ok", true}}; });
    server.connectToGateway("gw-2");

    PaymentConfig pay;
    pay.enabled = true;
    pay.recipientAddress = "0xrecipient";
    pay.chainId = "8453";
    pay.pricePerCall = 0.01f;
    gateway.enablePayments(pay);

    nlohmann::json paidArgs = {{"a", 1}};

    // Path 1: a wallet-signed call uses the client's bound private key. The
    // gateway expects the canonical "recipient:chainId:proof" signature, so
    // the wallet-signed payload should be rejected, exercising the
    // negative-path verification logic.
    MCPClient client("gw-2", "");
    client.setWallet("0xprivate");
    auto rejected = client.callToolWithPayment("srv-2::paid", paidArgs, "0xwallet");
    ASSERT_TRUE(rejected.contains("error"));

    // Path 2: build a payload with the canonical signature the gateway
    // expects and confirm the tool executes and the gateway records revenue.
    const std::string proof = "merchant-trace-001";
    auto digest = [](const std::string& s) {
        constexpr std::uint64_t FNV_OFFSET = 1469598103934665603ULL;
        constexpr std::uint64_t FNV_PRIME  = 1099511628211ULL;
        std::uint64_t a = FNV_OFFSET, b = ~FNV_OFFSET;
        std::size_t i = 0;
        for (unsigned char c : s) {
            a ^= static_cast<std::uint64_t>(c) + (i++);
            a *= FNV_PRIME;
            b ^= static_cast<std::uint64_t>(c) << 1;
            b *= FNV_PRIME ^ 0x9E3779B97F4A7C15ULL;
        }
        std::ostringstream oss;
        oss << std::hex << std::setw(16) << std::setfill('0') << a
            << std::setw(16) << std::setfill('0') << b;
        return oss.str();
    };
    const std::string signature = digest(pay.recipientAddress + ":" + pay.chainId + ":" + proof);
    auto out = gateway.executeToolWithPayment("srv-2::paid", paidArgs, signature, proof);
    ASSERT_TRUE(out.contains("ok"));
    bool okFlag = out["ok"].get<bool>();
    EXPECT_TRUE(okFlag);

    auto stats = gateway.getStatistics();
    EXPECT_GT(stats.totalRevenue, 0.0f);
}

TEST(MCPGatewayDepth, RateLimitFiresPerAPIKey) {
    MCPGateway gateway("gw-3");

    MCPServer server("srv-3");
    server.registerTool("ping", "ping", nlohmann::json::object(),
                        [](const nlohmann::json&) -> nlohmann::json { return {{"pong", 1}}; });
    server.connectToGateway("gw-3");

    APIKeyTier tier;
    tier.tierName = "free";
    tier.rateLimit = 3; // 3 req per 60s
    gateway.createAPIKey("k-free", tier);

    int allowed = 0, denied = 0;
    nlohmann::json emptyArgs = nlohmann::json::object();
    for (int i = 0; i < 7; ++i) {
        auto out = gateway.executeTool("srv-3::ping", emptyArgs, "k-free");
        if (out.contains("error") && out["error"].is_string() &&
            out["error"].get<std::string>() == "Rate limit exceeded") {
            ++denied;
        } else {
            ++allowed;
        }
    }
    EXPECT_EQ(allowed, 3);
    EXPECT_EQ(denied, 4);
}

TEST(MCPGatewayDepth, HealthMonitoringRunsRealThread) {
    MCPGateway gateway("gw-4");
    MCPServerConfig cfg;
    cfg.name = "alpha";
    cfg.transport = "stdio";
    cfg.endpoint = "alpha-bin";
    cfg.autoReconnect = true;
    gateway.addServer(cfg);

    gateway.startHealthMonitoring();
    std::this_thread::sleep_for(150ms);
    auto health = gateway.getServerHealth();
    EXPECT_EQ(health["alpha"], "healthy");
    gateway.stopHealthMonitoring();
}

// ===========================================================================
// CognitiveBridge tests
// ===========================================================================

TEST(CognitiveBridgeBasics, PublishAndSubscribeRoundTrip) {
    CognitiveBridge bridge("br-1");

    std::atomic<int> stateCount{0};
    std::atomic<int> sensoryCount{0};
    std::atomic<int> speechCount{0};

    auto idA = bridge.subscribeCognitiveState([&](const CognitiveState&){ ++stateCount; });
    auto idB = bridge.subscribeSensoryInput([&](const SensoryInput&){ ++sensoryCount; });
    auto idC = bridge.subscribeSpeechOutput([&](const SpeechOutput&){ ++speechCount; });

    CognitiveState s; s.agentId = "a"; s.mood = "curious";
    SensoryInput  i; i.sourceId = "mic"; i.modality = "audio"; i.payload = "hello";
    SpeechOutput  p; p.agentId = "a"; p.text = "hi"; p.voice = "alloy";

    bridge.publishCognitiveState(s);
    bridge.publishSensoryInput(i);
    bridge.publishSpeechOutput(p);

    EXPECT_EQ(stateCount.load(), 1);
    EXPECT_EQ(sensoryCount.load(), 1);
    EXPECT_EQ(speechCount.load(), 1);

    bridge.unsubscribeCognitiveState(idA);
    bridge.unsubscribeSensoryInput(idB);
    bridge.unsubscribeSpeechOutput(idC);

    bridge.publishCognitiveState(s);
    // Unsubscribed handler must not fire — no <<-streaming because the local
    // gtest compatibility shim does not implement EXPECT_*'s ostream proxy.
    EXPECT_EQ(stateCount.load(), 1);
}

TEST(CognitiveBridgeBasics, PublishingUsesSnapshotSoHandlersCanUnsubscribeSafely) {
    CognitiveBridge bridge("br-snapshot");

    std::atomic<int> selfHits{0};
    std::atomic<int> peerHits{0};
    SubscriptionId selfSubscription = 0;

    selfSubscription = bridge.subscribeCognitiveState([&](const CognitiveState&) {
        ++selfHits;
        bridge.unsubscribeCognitiveState(selfSubscription);
    });
    auto peerSubscription = bridge.subscribeCognitiveState([&](const CognitiveState&) {
        ++peerHits;
    });

    CognitiveState state;
    state.agentId = "snapshot-agent";
    state.mood = "stable";

    bridge.publishCognitiveState(state);
    bridge.publishCognitiveState(state);
    bridge.unsubscribeCognitiveState(peerSubscription);
    bridge.publishCognitiveState(state);

    EXPECT_EQ(selfHits.load(), 1);
    EXPECT_EQ(peerHits.load(), 2);
    EXPECT_EQ(bridge.stats().cognitivePublished, 3u);
}

TEST(CognitiveBridgeBasics, RecentHistoryLimitZeroReturnsEmptyAcrossChannels) {
    CognitiveBridge bridge("br-zero-limit");

    CognitiveState state;
    state.agentId = "zero-agent";
    bridge.publishCognitiveState(state);

    SensoryInput sensory;
    sensory.sourceId = "sensor";
    sensory.modality = "telemetry";
    sensory.payload = "sample";
    bridge.publishSensoryInput(sensory);

    SpeechOutput speech;
    speech.agentId = "zero-agent";
    speech.text = "sample";
    bridge.publishSpeechOutput(speech);

    EXPECT_TRUE(bridge.recentCognitiveStates(0).empty());
    EXPECT_TRUE(bridge.recentSensoryInputs(0).empty());
    EXPECT_TRUE(bridge.recentSpeechOutputs(0).empty());
}

TEST(CognitiveBridgeBasics, HistoryIsBoundedAndOrdered) {
    CognitiveBridge bridge("br-2");
    for (int i = 0; i < 300; ++i) {
        CognitiveState s; s.agentId = "a"; s.echobeatsStep = (i % 12) + 1;
        s.metadata["seq"] = std::to_string(i);
        bridge.publishCognitiveState(s);
    }
    auto recent = bridge.recentCognitiveStates(10);
    ASSERT_EQ(recent.size(), 10u);
    EXPECT_EQ(recent.front().metadata.at("seq"), "290");
    EXPECT_EQ(recent.back().metadata.at("seq"), "299");

    EXPECT_EQ(bridge.stats().cognitivePublished, 300u);
}

TEST(CognitiveBridgeBasics, SensoryAndSpeechHistoriesAreBoundedAndOrderedIndependently) {
    CognitiveBridge bridge("br-history");

    for (int i = 0; i < 270; ++i) {
        SensoryInput sensory;
        sensory.sourceId = "camera";
        sensory.modality = "vision";
        sensory.payload = "frame-" + std::to_string(i);
        sensory.metadata["seq"] = std::to_string(i);
        bridge.publishSensoryInput(sensory);

        SpeechOutput speech;
        speech.agentId = "avatar";
        speech.text = "utterance-" + std::to_string(i);
        speech.voice = "alloy";
        speech.metadata["seq"] = std::to_string(i);
        bridge.publishSpeechOutput(speech);
    }

    auto sensoryRecent = bridge.recentSensoryInputs(6);
    auto speechRecent = bridge.recentSpeechOutputs(6);

    ASSERT_EQ(sensoryRecent.size(), 6u);
    ASSERT_EQ(speechRecent.size(), 6u);
    EXPECT_EQ(sensoryRecent.front().metadata.at("seq"), "264");
    EXPECT_EQ(sensoryRecent.back().metadata.at("seq"), "269");
    EXPECT_EQ(speechRecent.front().metadata.at("seq"), "264");
    EXPECT_EQ(speechRecent.back().metadata.at("seq"), "269");

    auto stats = bridge.stats();
    EXPECT_EQ(stats.sensoryPublished, 270u);
    EXPECT_EQ(stats.speechPublished, 270u);
    EXPECT_EQ(stats.cognitivePublished, 0u);
}

TEST(CognitiveBridgeEchobeats, EchobeatsRunsThroughTwelveSteps) {
    CognitiveBridge bridge("br-3");

    std::set<int> seenSteps;
    std::set<int> seenPhases;
    auto sub = bridge.subscribeCognitiveState([&](const CognitiveState& s) {
        if (s.echobeatsStep > 0) {
            seenSteps.insert(s.echobeatsStep);
            seenPhases.insert(s.echobeatsPhase);
        }
    });

    bridge.startEchobeats(20ms, "echo-1");
    std::this_thread::sleep_for(360ms);
    bridge.stopEchobeats();
    bridge.unsubscribeCognitiveState(sub);

    // We should have seen all 12 steps and all 4 phases at least once.
    EXPECT_EQ(seenSteps.size(), 12u);
    EXPECT_EQ(seenPhases.size(), 4u);
    EXPECT_FALSE(bridge.isEchobeatsRunning());

    auto stats = bridge.stats();
    EXPECT_GE(stats.echobeatsTicks, 12u);
}

TEST(CognitiveBridgeEchobeats, PhaseFormulaMatchesEchobeatsKnowledge) {
    // Echobeats phasing: {1,5,9}=phase0, {2,6,10}=phase1, {3,7,11}=phase2,
    // {4,8,12}=phase3. The bridge must respect that contract.
    using P = std::pair<int, int>;
    const std::vector<P> expected = {
        {1, 0}, {2, 1}, {3, 2}, {4, 3},
        {5, 0}, {6, 1}, {7, 2}, {8, 3},
        {9, 0}, {10, 1}, {11, 2}, {12, 3},
    };
    for (const auto& kv : expected) {
        const int step = kv.first;
        const int phase = kv.second;
        const int actual = CognitiveBridge::phaseForStep(step);
        EXPECT_EQ(actual, phase);
    }
}

TEST(CognitiveBridgeEchobeats, TickPayloadCarriesAgentFocusMoodAndPhase) {
    CognitiveBridge bridge("br-payload");

    bridge.startEchobeats(5ms, "echo-payload");
    std::this_thread::sleep_for(40ms);
    bridge.stopEchobeats();

    const auto recent = bridge.recentCognitiveStates(16);
    ASSERT_FALSE(recent.empty());

    // The Echobeats loop now emits phase-semantic labels mapping the four
    // cognitive phases (0=perception, 1=reasoning, 2=action, 3=reflection).
    auto expectedMoodForPhase = [](int phase) -> std::string {
        switch (phase) {
            case 0: return "perceiving";
            case 1: return "reasoning";
            case 2: return "acting";
            default: return "reflecting";
        }
    };
    auto expectedFocusForPhase = [](int phase) -> std::string {
        switch (phase) {
            case 0: return "perception";
            case 1: return "reasoning";
            case 2: return "action";
            default: return "reflection";
        }
    };

    bool sawPayload = false;
    for (const auto& state : recent) {
        if (state.agentId == "echo-payload") {
            sawPayload = true;
            EXPECT_EQ(state.echobeatsPhase, CognitiveBridge::phaseForStep(state.echobeatsStep));
            EXPECT_EQ(state.mood, expectedMoodForPhase(state.echobeatsPhase));
            EXPECT_EQ(state.focus, expectedFocusForPhase(state.echobeatsPhase));
        }
    }

    EXPECT_TRUE(sawPayload);
    EXPECT_FALSE(bridge.isEchobeatsRunning());
    EXPECT_GE(bridge.stats().echobeatsTicks, 1u);
}

TEST(CognitiveBridgeEchobeats, StartStopIsIdempotentAndRestartable) {
    CognitiveBridge bridge("br-restart");

    bridge.startEchobeats(5ms, "echo-restart");
    bridge.startEchobeats(5ms, "echo-restart");
    EXPECT_TRUE(bridge.isEchobeatsRunning());
    std::this_thread::sleep_for(45ms);
    bridge.stopEchobeats();
    bridge.stopEchobeats();
    EXPECT_FALSE(bridge.isEchobeatsRunning());

    const auto firstStats = bridge.stats();
    EXPECT_GT(firstStats.echobeatsTicks, 0u);
    EXPECT_GT(firstStats.cognitivePublished, 0u);

    bridge.startEchobeats(5ms, "echo-restart");
    std::this_thread::sleep_for(30ms);
    bridge.stopEchobeats();

    const auto secondStats = bridge.stats();
    EXPECT_GT(secondStats.echobeatsTicks, firstStats.echobeatsTicks);
    EXPECT_GT(secondStats.cognitivePublished, firstStats.cognitivePublished);
}

TEST(CognitiveBridgeEchobeats, ConcurrentPublishersAreThreadSafe) {
    CognitiveBridge bridge("br-4");
    std::atomic<int> received{0};
    auto sub = bridge.subscribeSensoryInput([&](const SensoryInput&) { ++received; });

    constexpr int kThreads = 4;
    constexpr int kMessagesPerThread = 250;
    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < kMessagesPerThread; ++i) {
                SensoryInput in;
                in.sourceId = "thread-" + std::to_string(t);
                in.modality = "telemetry";
                in.payload = std::to_string(i);
                bridge.publishSensoryInput(in);
            }
        });
    }
    for (auto& th : threads) th.join();
    bridge.unsubscribeSensoryInput(sub);

    EXPECT_EQ(received.load(), kThreads * kMessagesPerThread);
    EXPECT_EQ(bridge.stats().sensoryPublished,
              static_cast<std::uint64_t>(kThreads * kMessagesPerThread));
}

// ===========================================================================
// Full-stack pipeline test: SWE-Agent -> MCP Gateway -> CognitiveBridge
// ===========================================================================

TEST(CognitivePipelineE2E, CharacterKnowledgeAndBridgeRoundTripAutonomyContext) {
    // 1. A character profile is loaded from real JSON and round-tripped through
    //    the repaired CharacterFileLoader exporter/parser path.
    CharacterFileLoader loader;
    const std::string characterJson = R"json({
        "name": "Eliza Autonomous Repairer",
        "description": "A cognitive C++ agent that repairs repositories and narrates progress.",
        "version": "1.0",
        "creator": "ksm-cycle",
        "tags": ["autonomy", "cpp", "repair"],
        "personality": {
            "openness": 0.9,
            "conscientiousness": 0.82,
            "curiosity": 0.95,
            "empathy": 0.72
        },
        "background": {
            "occupation": "autonomous software repair agent",
            "goals": ["complete missing implementations", "preserve cognitive continuity"]
        },
        "communicationStyle": {
            "tone": "precise",
            "vocabulary": "technical",
            "verbosity": 0.65,
            "catchphrases": ["closing the loop"]
        }
    })json";

    auto profile = loader.loadFromJson(characterJson);
    ASSERT_TRUE(profile.has_value());
    EXPECT_EQ(profile->name, "Eliza Autonomous Repairer");
    EXPECT_NE(profile->description.find("repairs repositories"), std::string::npos);
    EXPECT_EQ(profile->communicationStyle.tone, "precise");

    const std::string exported = loader.exportToJson(*profile);
    auto reloaded = loader.loadFromJson(exported);
    ASSERT_TRUE(reloaded.has_value());
    EXPECT_EQ(reloaded->name, profile->name);
    EXPECT_EQ(reloaded->description, profile->description);
    EXPECT_EQ(reloaded->communicationStyle.vocabulary, "technical");

    // 2. The repaired knowledge helpers chunk, ingest, find, and reconstruct an
    //    autonomy context document without relying on external services.
    KnowledgeBase kb;
    const std::string autonomyContext =
        profile->name + " uses Echobeats, character memory, and repository tests "
        "to preserve autonomous repair intent across C++ implementation cycles.";
    const auto ids = knowledge::ingestText(
        kb, autonomyContext, "autonomy-context", KnowledgeType::CONCEPT,
        {"autonomy", "character", "knowledge"}, 48, 12);
    ASSERT_GE(ids.size(), 2u);
    EXPECT_EQ(kb.getKnowledgeCount(), ids.size());

    const auto chunks = knowledge::findChunksByDocument(kb, "autonomy-context");
    ASSERT_EQ(chunks.size(), ids.size());
    EXPECT_EQ(chunks.front().metadata.at("chunk_index"), "0");
    const auto reconstructed = knowledge::reconstructDocument(kb, "autonomy-context");
    EXPECT_EQ(reconstructed, autonomyContext);
    EXPECT_TRUE(knowledge::knowledge_helpers_self_check());

    // 3. The bridge publishes the resulting character-grounded knowledge state
    //    so downstream avatar/UI layers can subscribe to real cognitive state and
    //    speech output rather than placeholder status strings.
    CognitiveBridge bridge("char-knowledge-bridge");
    std::atomic<int> stateCount{0};
    std::atomic<int> speechCount{0};
    bridge.subscribeCognitiveState([&](const CognitiveState&) { ++stateCount; });
    bridge.subscribeSpeechOutput([&](const SpeechOutput&) { ++speechCount; });

    CognitiveState state;
    state.agentId = profile->name;
    state.mood = "focused";
    state.focus = "autonomy-context-loaded";
    state.echobeatsStep = 9;
    state.echobeatsPhase = CognitiveBridge::phaseForStep(state.echobeatsStep);
    state.metadata["document"] = "autonomy-context";
    state.metadata["chunks"] = std::to_string(chunks.size());
    bridge.publishCognitiveState(state);

    SpeechOutput speech;
    speech.agentId = profile->name;
    speech.text = "Loaded " + std::to_string(chunks.size()) + " autonomy context chunks.";
    speech.voice = "alloy";
    bridge.publishSpeechOutput(speech);

    EXPECT_EQ(stateCount.load(), 1);
    EXPECT_EQ(speechCount.load(), 1);
    auto recentState = bridge.recentCognitiveStates(1);
    ASSERT_FALSE(recentState.empty());
    EXPECT_EQ(recentState.front().metadata.at("document"), "autonomy-context");
    EXPECT_EQ(recentState.front().echobeatsPhase, CognitiveBridge::phaseForStep(9));
    EXPECT_NE(bridge.recentSpeechOutputs(1).front().text.find("autonomy context"),
              std::string::npos);
}

TEST(CognitivePipelineE2E, SolveIssueExposeViaMCPNarrateOnBridge) {
    // 1. The SWE-Agent solves a feature request.
    SWEAgent agent("e2e-agent", "deterministic-llm");
    GitHubIssue issue;
    issue.repo = "elizaos/cognitive-pipeline";
    issue.issueNumber = 7;
    issue.title = "Add greet() helper";
    issue.description = "Implement a greet helper that returns a friendly message.";
    issue.labels = {"feature", "cpp"};
    auto solution = agent.solveIssue(issue);
    ASSERT_TRUE(solution.success);

    // 2. The cognitive engine exposes the agent's status via an MCP tool.
    MCPGateway gateway("e2e-gw");
    MCPServer server("e2e-srv");
    server.registerTool(
        "agent_status", "Returns the SWE agent's textual status",
        nlohmann::json::object(),
        [&agent](const nlohmann::json&) -> nlohmann::json {
            nlohmann::json out;
            out["status"] = agent.getStatus();
            out["solved"] = agent.getHistory().size();
            return out;
        });
    server.connectToGateway("e2e-gw");

    MCPClient client("e2e-gw");
    auto tools = client.discoverTools();
    ASSERT_FALSE(tools.empty());

    nlohmann::json statusArgs = nlohmann::json::object();
    auto status = client.callTool("e2e-srv::agent_status", statusArgs);
    ASSERT_TRUE(status.contains("status"));
    const int solvedCount = status["solved"].get<int>();
    EXPECT_GT(solvedCount, 0);

    // 3. The CognitiveBridge narrates the outcome on the speech_output channel
    //    and emits a cognitive_state_update describing the new mood.
    CognitiveBridge bridge("e2e-bridge");
    std::atomic<int> stateCount{0};
    std::atomic<int> speechCount{0};
    bridge.subscribeCognitiveState([&](const CognitiveState&) { ++stateCount; });
    bridge.subscribeSpeechOutput([&](const SpeechOutput&) { ++speechCount; });

    CognitiveState mood;
    mood.agentId = "e2e";
    mood.mood = "satisfied";
    mood.focus = "task-complete";
    mood.echobeatsStep = 5;
    mood.echobeatsPhase = CognitiveBridge::phaseForStep(5);
    mood.valence = 0.7;
    mood.arousal = 0.4;
    bridge.publishCognitiveState(mood);

    SpeechOutput speech;
    speech.agentId = "e2e";
    speech.text = "I completed issue #" + std::to_string(issue.issueNumber);
    speech.voice = "alloy";
    bridge.publishSpeechOutput(speech);

    EXPECT_EQ(stateCount.load(), 1);
    EXPECT_EQ(speechCount.load(), 1);
    EXPECT_FALSE(bridge.recentSpeechOutputs(1).empty());
    EXPECT_NE(bridge.recentSpeechOutputs(1).front().text.find("issue #7"),
              std::string::npos);
}


// gtest_main from FetchContent provides the entry point now that the shim has
// been removed. The previous custom main() was a workaround for the dead-code
// shim header and is no longer needed.
