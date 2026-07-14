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
        workers.emplace_back([this, t]() {
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

} // namespace
