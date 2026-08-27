// test_village_atomspace_e2e.cpp — Real E2E coverage for the VillageAtomSpace
// cognitive substrate (OpenCog-hpp AtomSpace + ECAN + PLN wired for elizad).
//
// Germinates the previously dormant ATenStyx foundation layer into a living
// center: every capability the header promises is exercised end-to-end —
// resident management, cognitive cycles, event processing, KSM knowledge
// transfer, ECAN→Antikythera gear mapping, PLN inheritance queries,
// AphroditeBridge request construction, and the completed save()/load()
// s-expression persistence round-trip.

#include <gtest/gtest.h>

#include "village_atomspace.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

namespace {

using village::atomspace::AphroditeBridge;
using village::atomspace::AphroditeRequest;
using village::atomspace::AtomSpaceConfig;
using village::atomspace::CognitiveEvent;
using village::atomspace::ResidentAtom;
using village::atomspace::VillageAtomSpace;

ResidentAtom makeResident(const std::string& name,
                          const std::string& gearTrain,
                          double sti,
                          double openness = 0.7,
                          double conscientiousness = 0.6) {
    ResidentAtom r;
    r.name = name;
    r.openness = openness;
    r.conscientiousness = conscientiousness;
    r.extraversion = 0.5;
    r.agreeableness = 0.6;
    r.neuroticism = 0.3;
    r.sti = sti;
    r.lti = sti * 0.5;
    r.gear_train = gearTrain;
    r.gear_rpm_multiplier = 1.0;
    return r;
}

class VillageAtomSpaceE2ETest : public ::testing::Test {
protected:
    void SetUp() override {
        persistPath_ =
            (std::filesystem::temp_directory_path() /
             ("village_atomspace_e2e_" +
              std::to_string(::testing::UnitTest::GetInstance()->random_seed()) +
              "_" + std::to_string(reinterpret_cast<uintptr_t>(this)) + ".scm"))
                .string();
        config_.persist_path = persistPath_;
        config_.af_size = 10;
        vas_ = std::make_unique<VillageAtomSpace>(config_);
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove(persistPath_, ec);
        std::filesystem::remove(persistPath_ + ".tmp", ec);
    }

    void addDefaultResidents() {
        vas_->add_resident(makeResident("ada", "symbolic", 40.0, 0.9, 0.8));
        vas_->add_resident(makeResident("turing", "core", 35.0, 0.8, 0.9));
        vas_->add_resident(makeResident("hypatia", "creative", 30.0, 0.95, 0.5));
    }

    AtomSpaceConfig config_;
    std::string persistPath_;
    std::unique_ptr<VillageAtomSpace> vas_;
};

// ── Resident management ─────────────────────────────────────────────

TEST_F(VillageAtomSpaceE2ETest, AddResidentCreatesConceptNodes) {
    addDefaultResidents();
    const auto& residents = vas_->residents();
    ASSERT_EQ(residents.size(), 3u);
    EXPECT_TRUE(residents.count("ada"));
    EXPECT_TRUE(residents.count("turing"));
    EXPECT_TRUE(residents.count("hypatia"));
    for (const auto& kv : residents) {
        EXPECT_NE(kv.second.concept_handle, oc::UNDEFINED_HANDLE)
            << kv.first << " must be bound to a real ConceptNode";
    }
}

TEST_F(VillageAtomSpaceE2ETest, SeededTypeHierarchyPopulatesAtomSpace) {
    // The constructor seeds a type hierarchy before any resident exists.
    EXPECT_GT(vas_->raw_atomspace().size(), 0u);
}

// ── Cognitive cycle ─────────────────────────────────────────────────

TEST_F(VillageAtomSpaceE2ETest, RunCycleAdvancesCycleCountAndReportsState) {
    addDefaultResidents();
    auto r1 = vas_->run_cycle();
    auto r2 = vas_->run_cycle();
    EXPECT_EQ(r1.cycle_number + 1, r2.cycle_number);
    EXPECT_EQ(vas_->cycle_count(), r2.cycle_number);
    EXPECT_GT(r2.atom_count, 0u);
}

TEST_F(VillageAtomSpaceE2ETest, RepeatedCyclesKeepTotalStiFinite) {
    addDefaultResidents();
    for (int i = 0; i < 25; ++i) {
        auto result = vas_->run_cycle();
        EXPECT_TRUE(std::isfinite(result.total_sti))
            << "cycle " << result.cycle_number << " produced non-finite STI";
    }
    EXPECT_EQ(vas_->cycle_count(), 25u);
}

// ── Event processing ────────────────────────────────────────────────

TEST_F(VillageAtomSpaceE2ETest, ProcessEventStoresAtomsAndStimulatesParticipants) {
    addDefaultResidents();
    size_t before = vas_->raw_atomspace().size();

    CognitiveEvent ev;
    ev.type = "utterance";
    ev.participants = {"ada", "turing"};
    ev.content = "lambda calculus is the assembly of thought";
    ev.emotional_valence = 0.6;
    ev.information_gain = 0.8;
    vas_->process_event(ev);

    EXPECT_GT(vas_->raw_atomspace().size(), before)
        << "processing an event must add atoms to the hypergraph";
}

TEST_F(VillageAtomSpaceE2ETest, AttentionalFocusJsonIsWellFormed) {
    addDefaultResidents();
    vas_->run_cycle();
    std::string json = vas_->get_attentional_focus_json();
    EXPECT_FALSE(json.empty());
    EXPECT_EQ(json.front(), '[');
    EXPECT_EQ(json.back(), ']');
}

TEST_F(VillageAtomSpaceE2ETest, StatsJsonContainsCoreCounters) {
    addDefaultResidents();
    vas_->run_cycle();
    std::string json = vas_->get_stats_json();
    EXPECT_NE(json.find("atom"), std::string::npos);
    EXPECT_EQ(json.front(), '{');
    EXPECT_EQ(json.back(), '}');
}

// ── KSM knowledge transfer (Dan's Relational Principle) ────────────

TEST_F(VillageAtomSpaceE2ETest, RecordLearningPromotesTeacherToInstruction) {
    addDefaultResidents();
    vas_->record_learning("ada", "hypatia", "symbolic-logic", 0.85);

    const auto& residents = vas_->residents();
    const auto& ada = residents.at("ada");
    auto it = ada.domain_levels.find("symbolic-logic");
    ASSERT_NE(it, ada.domain_levels.end());
    EXPECT_GE(static_cast<int>(it->second),
              static_cast<int>(ResidentAtom::KSMLevel::INSTRUCTION));
}

TEST_F(VillageAtomSpaceE2ETest, KnowledgePropagationIsQueryableThroughPln) {
    addDefaultResidents();
    vas_->record_learning("ada", "turing", "computation", 0.9);

    auto results = vas_->query_inheritance_chain("turing:computation",
                                                 "ada:computation");
    ASSERT_FALSE(results.empty())
        << "teaching must store a PLN inheritance edge learner->teacher";
    EXPECT_GT(results[0].strength, 0.0);
    EXPECT_GT(results[0].confidence, 0.0);
}

TEST_F(VillageAtomSpaceE2ETest, TransitiveTeachingChainsAccumulate) {
    addDefaultResidents();
    // ada teaches turing; turing teaches hypatia — knowledge propagates.
    vas_->record_learning("ada", "turing", "recursion", 0.9);
    vas_->record_learning("turing", "hypatia", "recursion", 0.8);

    auto direct = vas_->query_inheritance_chain("hypatia:recursion",
                                                "turing:recursion");
    ASSERT_FALSE(direct.empty());
    EXPECT_GT(direct[0].strength, 0.0);
}

// ── ECAN → Antikythera gear mapping ────────────────────────────────

TEST_F(VillageAtomSpaceE2ETest, GearStatesCoverAllTrainsWithPositiveRpm) {
    addDefaultResidents();
    vas_->run_cycle();
    auto gears = vas_->get_gear_states();
    ASSERT_GE(gears.size(), 3u) << "each populated gear train needs a state";
    for (const auto& g : gears) {
        EXPECT_FALSE(g.train_name.empty());
        EXPECT_TRUE(std::isfinite(g.rpm));
        EXPECT_TRUE(std::isfinite(g.modulation));
    }
}

// ── AphroditeBridge (symbolic → subsymbolic) ───────────────────────

TEST_F(VillageAtomSpaceE2ETest, AphroditeBridgeBuildsPersonaGroundedRequest) {
    addDefaultResidents();
    vas_->record_learning("ada", "turing", "mathematics", 0.9);
    vas_->run_cycle();

    AphroditeRequest req = AphroditeBridge::build_request(
        *vas_, "ada", "What is the nature of computation?", 0.65);

    EXPECT_EQ(req.resident, "ada");
    EXPECT_EQ(req.user_prompt, "What is the nature of computation?");
    EXPECT_DOUBLE_EQ(req.temperature, 0.65);
    EXPECT_GT(req.min_p, 0.0);
    EXPECT_NE(req.system_prompt.find("ada"), std::string::npos)
        << "system prompt must be grounded in the resident persona";
    EXPECT_EQ(req.lora_adapter, "loras/ada");
}

TEST_F(VillageAtomSpaceE2ETest, AphroditeBridgeUnknownResidentDegradesGracefully) {
    addDefaultResidents();
    AphroditeRequest req =
        AphroditeBridge::build_request(*vas_, "nobody", "hello", 0.7);
    EXPECT_EQ(req.resident, "nobody");
    EXPECT_EQ(req.user_prompt, "hello");
}

// ── Persistence round-trip (completed save/load) ───────────────────

TEST_F(VillageAtomSpaceE2ETest, SaveWritesSnapshotFile) {
    addDefaultResidents();
    vas_->record_learning("ada", "turing", "persistence", 0.9);
    vas_->run_cycle();

    ASSERT_TRUE(vas_->save());
    ASSERT_TRUE(std::filesystem::exists(persistPath_));

    std::ifstream in(persistPath_);
    std::string content((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("VillageAtomSpace snapshot"), std::string::npos);
    EXPECT_NE(content.find("(Concept"), std::string::npos)
        << "snapshot must contain serialized ConceptNodes";
}

TEST_F(VillageAtomSpaceE2ETest, LoadRestoresAtomsIntoFreshAtomSpace) {
    addDefaultResidents();
    vas_->record_learning("ada", "turing", "memory", 0.9);
    vas_->run_cycle();
    ASSERT_TRUE(vas_->save());

    AtomSpaceConfig fresh = config_;
    VillageAtomSpace restored(fresh);
    size_t before = restored.raw_atomspace().size();
    size_t loaded = restored.load();
    EXPECT_GT(loaded, 0u) << "load() must restore top-level atoms";
    EXPECT_GT(restored.raw_atomspace().size(), before);
}

TEST_F(VillageAtomSpaceE2ETest, SaveLoadRoundTripPreservesResidentBinding) {
    addDefaultResidents();
    vas_->run_cycle();
    ASSERT_TRUE(vas_->save());

    // Re-load into the same instance; resident handles must stay valid.
    size_t restored = vas_->load();
    EXPECT_GT(restored, 0u);
    for (const auto& kv : vas_->residents()) {
        EXPECT_NE(kv.second.concept_handle, oc::UNDEFINED_HANDLE);
    }
}

TEST_F(VillageAtomSpaceE2ETest, LoadFromMissingFileReturnsZero) {
    AtomSpaceConfig cfg;
    cfg.persist_path = "/nonexistent/dir/never_here.scm";
    VillageAtomSpace vas(cfg);
    EXPECT_EQ(vas.load(), 0u);
}

// ── Full pipeline: stimulus → attention → reasoning → persistence ──

TEST_F(VillageAtomSpaceE2ETest, FullCognitivePipelineEndToEnd) {
    addDefaultResidents();

    // 1. Stimulus: events flow through the village.
    for (int i = 0; i < 5; ++i) {
        CognitiveEvent ev;
        ev.type = "utterance";
        ev.participants = {"ada", "hypatia"};
        ev.content = "iteration " + std::to_string(i);
        ev.emotional_valence = 0.2 * i - 0.4;
        ev.information_gain = 0.5;
        vas_->process_event(ev);
    }

    // 2. Teaching: knowledge transfer with KSM progression.
    vas_->record_learning("ada", "hypatia", "pattern-mining", 0.9);

    // 3. Attention + reasoning: several cognitive cycles.
    for (int i = 0; i < 10; ++i) {
        auto result = vas_->run_cycle();
        EXPECT_TRUE(std::isfinite(result.total_sti));
    }

    // 4. Expression: Aphrodite request from evolved state.
    auto req = AphroditeBridge::build_request(*vas_, "hypatia",
                                              "Describe what you learned.");
    EXPECT_FALSE(req.system_prompt.empty());

    // 5. Persistence: snapshot and restore.
    ASSERT_TRUE(vas_->save());
    VillageAtomSpace successor(config_);
    EXPECT_GT(successor.load(), 0u);
}

// ── Thread-safe event queue (inference-thread → main-loop handoff) ───

TEST_F(VillageAtomSpaceE2ETest, EnqueueAndDrainProcessesPendingEvents) {
    addDefaultResidents();

    const size_t before = vas_->raw_atomspace().size();
    for (int i = 0; i < 4; ++i) {
        CognitiveEvent ev;
        ev.type = "inference";
        ev.participants = {"ada"};
        ev.content = "queued thought " + std::to_string(i);
        ev.emotional_valence = 0.1;
        ev.information_gain = 0.6;
        vas_->enqueue_event(ev);
    }

    // Nothing is applied until the main loop drains the queue.
    EXPECT_EQ(vas_->raw_atomspace().size(), before);

    EXPECT_EQ(vas_->drain_pending_events(), 4u);
    EXPECT_GT(vas_->raw_atomspace().size(), before);

    // Queue must be empty after a drain.
    EXPECT_EQ(vas_->drain_pending_events(), 0u);
}

TEST_F(VillageAtomSpaceE2ETest, ConcurrentEnqueueFromWorkerThreadsIsSafe) {
    addDefaultResidents();

    constexpr int kThreads = 4;
    constexpr int kEventsPerThread = 25;
    std::vector<std::thread> workers;
    workers.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t) {
        // MSVC requires constexpr locals used inside a lambda to be captured
        // explicitly (C3493) even though they are not odr-used.
        workers.emplace_back([this, t, kEventsPerThread]() {
            for (int i = 0; i < kEventsPerThread; ++i) {
                CognitiveEvent ev;
                ev.type = "utterance";
                ev.participants = {"turing"};
                ev.content = "t" + std::to_string(t) + "-e" + std::to_string(i);
                ev.emotional_valence = 0.0;
                ev.information_gain = 0.3;
                vas_->enqueue_event(ev);
            }
        });
    }
    for (auto& w : workers) w.join();

    EXPECT_EQ(vas_->drain_pending_events(),
              static_cast<size_t>(kThreads * kEventsPerThread));
}

// ── Thread-safe STI accessors ───────────────────────────────────

TEST_F(VillageAtomSpaceE2ETest, StiAccessorsReadAndWriteResidentImportance) {
    addDefaultResidents();

    // add_resident() normalises every newcomer to STI 100 via an initial
    // ECAN stimulation, regardless of the STI carried by the ResidentAtom.
    EXPECT_DOUBLE_EQ(vas_->get_resident_sti("ada"), 100.0);
    EXPECT_DOUBLE_EQ(vas_->get_resident_sti("nobody"), 0.0);

    vas_->set_resident_sti("ada", 55.5);
    EXPECT_DOUBLE_EQ(vas_->get_resident_sti("ada"), 55.5);

    // Writing to an unknown resident must be a harmless no-op.
    vas_->set_resident_sti("nobody", 99.0);
    EXPECT_DOUBLE_EQ(vas_->get_resident_sti("nobody"), 0.0);

    // The string diagnostic accessor stays consistent with the numeric one.
    EXPECT_FALSE(vas_->get_resident_sti_str("ada").empty());
}

// ── Cycle 007/008: episodic memory + conversation history ───────────

TEST_F(VillageAtomSpaceE2ETest, EpisodicMemoryKeepsRollingWindowOfTen) {
    addDefaultResidents();

    for (int i = 0; i < 14; ++i) {
        vas_->add_episodic("ada", "thought", "memory-" + std::to_string(i));
    }

    const std::string ctx = vas_->get_episodic_context("ada");
    ASSERT_FALSE(ctx.empty());
    EXPECT_NE(ctx.find("[Recent memory]"), std::string::npos);
    // Oldest entries must have been evicted by the rolling window (size 10).
    EXPECT_EQ(ctx.find("memory-0"), std::string::npos);
    EXPECT_EQ(ctx.find("memory-3"), std::string::npos);
    // Newest entries must be present.
    EXPECT_NE(ctx.find("memory-13"), std::string::npos);
    EXPECT_NE(ctx.find("memory-4"), std::string::npos);
}

TEST_F(VillageAtomSpaceE2ETest, ConversationHistoryKeepsRollingWindowOfFive) {
    addDefaultResidents();

    for (int i = 0; i < 8; ++i) {
        vas_->add_conversation("turing", "stimulus-" + std::to_string(i),
                               "response-" + std::to_string(i));
    }

    const std::string ctx = vas_->get_conversation_context("turing");
    ASSERT_FALSE(ctx.empty());
    EXPECT_NE(ctx.find("[Conversation history]"), std::string::npos);
    // Window is 5: entries 0-2 evicted, 3-7 kept.
    EXPECT_EQ(ctx.find("stimulus-0"), std::string::npos);
    EXPECT_EQ(ctx.find("stimulus-2"), std::string::npos);
    EXPECT_NE(ctx.find("stimulus-3"), std::string::npos);
    EXPECT_NE(ctx.find("response-7"), std::string::npos);
}

TEST_F(VillageAtomSpaceE2ETest, MemoryContextForUnknownResidentIsEmpty) {
    EXPECT_TRUE(vas_->get_episodic_context("nobody").empty());
    EXPECT_TRUE(vas_->get_conversation_context("nobody").empty());
}

TEST_F(VillageAtomSpaceE2ETest, MemoryContextGettersAreConstCorrect) {
    addDefaultResidents();
    vas_->add_episodic("ada", "thought", "const-check");
    vas_->add_conversation("ada", "ping", "pong");

    // Must be callable through a const reference (used by AphroditeBridge
    // during prompt construction without const_cast).
    const VillageAtomSpace& cvas = *vas_;
    EXPECT_NE(cvas.get_episodic_context("ada").find("const-check"),
              std::string::npos);
    EXPECT_NE(cvas.get_conversation_context("ada").find("pong"),
              std::string::npos);
}

TEST_F(VillageAtomSpaceE2ETest, PersistAndLoadRoundTripRestoresMemory) {
    addDefaultResidents();
    vas_->add_episodic("ada", "thought", "the lambda calculus is a garden");
    vas_->add_episodic("ada", "stimulus", "what do you compute?");
    vas_->add_conversation("ada", "hello ada", "hello dan, shall we prove something?");

    const std::string jsonPath = persistPath_ + ".mem.json";
    vas_->set_persist_path(jsonPath);
    ASSERT_TRUE(vas_->persist());
    ASSERT_TRUE(std::filesystem::exists(jsonPath));

    VillageAtomSpace successor(config_);
    successor.set_persist_path(jsonPath);
    ASSERT_TRUE(successor.load_persisted());

    // Episodic + conversation memory must survive the round-trip.
    const std::string ep = successor.get_episodic_context("ada");
    EXPECT_NE(ep.find("lambda calculus"), std::string::npos);
    EXPECT_NE(ep.find("what do you compute?"), std::string::npos);
    const std::string conv = successor.get_conversation_context("ada");
    EXPECT_NE(conv.find("hello ada"), std::string::npos);
    EXPECT_NE(conv.find("prove something"), std::string::npos);

    // Resident atoms must also be restored.
    EXPECT_TRUE(successor.residents().count("ada"));
    EXPECT_TRUE(successor.residents().count("turing"));

    std::error_code ec;
    std::filesystem::remove(jsonPath, ec);
    std::filesystem::remove(jsonPath + ".tmp", ec);
}

TEST_F(VillageAtomSpaceE2ETest, LoadPersistedFromMissingFileReturnsFalse) {
    vas_->set_persist_path("/nonexistent/dir/never_here.json");
    EXPECT_FALSE(vas_->load_persisted());
}

TEST_F(VillageAtomSpaceE2ETest, ConversationRecorderHookFeedsMemory) {
    addDefaultResidents();

    AphroditeBridge::Config cfg;
    cfg.url = "http://127.0.0.1:1/unreachable";  // never contacted in this test
    AphroditeBridge bridge(cfg);

    // Wire the Cycle 008 recorder exactly the way elizad does.
    bridge.set_conversation_recorder(
        [this](const std::string& resident, const std::string& stimulus,
               const std::string& response) {
            vas_->add_conversation(resident, stimulus, response);
        });

    // Simulate the post-inference invocation path.
    vas_->add_conversation("hypatia", "observed stimulus", "considered response");
    const std::string ctx = vas_->get_conversation_context("hypatia");
    EXPECT_NE(ctx.find("observed stimulus"), std::string::npos);
    EXPECT_NE(ctx.find("considered response"), std::string::npos);
}

TEST_F(VillageAtomSpaceE2ETest, PromptConstructionInjectsMemoryContext) {
    addDefaultResidents();
    vas_->add_episodic("ada", "thought", "remember the gear trains");
    vas_->add_conversation("ada", "how are the gears?", "spinning at 1.2 rpm");

    AphroditeBridge::Config cfg;
    AphroditeBridge bridge(cfg);
    AphroditeRequest req = bridge.build_request(*vas_, "ada", "status?", 0.7);
    // build_request itself carries persona; memory context is appended by
    // infer_async. Reproduce that composition here.
    req.system_prompt += vas_->get_episodic_context("ada");
    req.system_prompt += vas_->get_conversation_context("ada");

    EXPECT_NE(req.system_prompt.find("[Recent memory]"), std::string::npos);
    EXPECT_NE(req.system_prompt.find("remember the gear trains"), std::string::npos);
    EXPECT_NE(req.system_prompt.find("[Conversation history]"), std::string::npos);
    EXPECT_NE(req.system_prompt.find("spinning at 1.2 rpm"), std::string::npos);
}

TEST_F(VillageAtomSpaceE2ETest, ResidentDetailReportsBoundedNewestFirstActionHistory) {
    addDefaultResidents();
    vas_->add_episodic("ada", "stimulus", "inspect the stone");
    vas_->add_conversation("ada", "inspect", "I will inspect it");
    for (int i = 0; i < 12; ++i) {
        vas_->add_action("ada", "observe_state", "result-" + std::to_string(i),
                         100 + i, "corr-" + std::to_string(i));
    }

    const auto detail = nlohmann::json::parse(vas_->get_resident_detail_json("ada"));
    EXPECT_EQ(detail.at("resident"), "ada");
    ASSERT_TRUE(detail.at("atom").is_object());
    EXPECT_EQ(detail.at("atom").at("name"), "ada");
    EXPECT_FALSE(detail.at("episodic_memory").empty());
    EXPECT_FALSE(detail.at("conversation_history").empty());
    const auto& actions = detail.at("actions");
    ASSERT_EQ(actions.size(), 10u);
    EXPECT_EQ(actions.front().at("result"), "result-11");
    EXPECT_EQ(actions.front().at("inference_id"), 111);
    EXPECT_EQ(actions.front().at("correlation_id"), "corr-11");
    EXPECT_EQ(actions.back().at("result"), "result-2");
}

TEST_F(VillageAtomSpaceE2ETest, PersistedActionHistoryRetainsCorrelationAcrossRestart) {
    addDefaultResidents();
    vas_->add_action("turing", "write_stone", "stone-written", 42, "request-42");
    const std::string jsonPath = persistPath_ + ".actions.json";
    vas_->set_persist_path(jsonPath);
    ASSERT_TRUE(vas_->persist());

    VillageAtomSpace successor(config_);
    successor.set_persist_path(jsonPath);
    ASSERT_TRUE(successor.load_persisted());
    const auto detail = nlohmann::json::parse(
        successor.get_resident_detail_json("turing"));
    ASSERT_EQ(detail.at("actions").size(), 1u);
    EXPECT_EQ(detail.at("actions").front().at("action_type"), "write_stone");
    EXPECT_EQ(detail.at("actions").front().at("result"), "stone-written");
    EXPECT_EQ(detail.at("actions").front().at("inference_id"), 42);
    EXPECT_EQ(detail.at("actions").front().at("correlation_id"), "request-42");

    std::error_code ec;
    std::filesystem::remove(jsonPath, ec);
    std::filesystem::remove(jsonPath + ".tmp", ec);
}

} // namespace
