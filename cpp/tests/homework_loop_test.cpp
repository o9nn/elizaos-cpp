/**
 * ElizaOS C++ - Homework Loop E2E Tests
 *
 * Comprehensive deterministic tests for the cognitive curriculum scorer and the bounded
 * homework loop. These assert both functional behaviour and the alignment invariants:
 *   - a stubbed center scores strictly lower than a live one (truthful aliveness),
 *   - the weakest center is the one selected for homework (gradient rule),
 *   - a homework cycle never issues a destructive command (operational subset),
 *   - step 10 only proposes a mutation; nothing destructive is committed,
 *   - every cycle records a well-formed handoff signal (visible handoff).
 */

#include "elizaos/cognitive_curriculum.hpp"
#include "elizaos/homework_loop.hpp"
#include "elizaos/autonomous_starter.hpp"

#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <string>

using namespace elizaos;

namespace {

// A deterministic evidence provider: one fully-live center, one fully-stubbed center,
// and the rest middling. Used to assert scoring, selection, and reproducibility.
class FixtureEvidenceProvider : public EvidenceProvider {
public:
    FixtureEvidenceProvider(CenterId live, CenterId stub)
        : live_(live), stub_(stub) {}

    std::map<CenterId, EvidenceMap> gather(const AutonomousStarter&) const override {
        std::map<CenterId, EvidenceMap> out;
        for (CenterId c : allCenters()) {
            EvidenceMap em;
            double v = 0.5;  // middling default
            if (c == live_) v = 1.0;
            if (c == stub_) v = 0.0;
            for (const auto& key : propertyEvidenceKeys()) {
                em[key] = v;
            }
            out[c] = em;
        }
        return out;
    }

private:
    CenterId live_;
    CenterId stub_;
};

AgentConfig makeConfig() {
    AgentConfig cfg;
    cfg.agentId = "homework-test-agent";
    cfg.agentName = "Autoliza";
    cfg.bio = "bounded homework test agent";
    return cfg;
}

}  // namespace

// ----------------------------------------------------------------------------
// CognitiveCurriculum - scorer
// ----------------------------------------------------------------------------

TEST(CognitiveCurriculum, HasFifteenProperties) {
    CognitiveCurriculum cur;
    EXPECT_EQ(cur.propertyCount(), 15u);
    EXPECT_EQ(propertyEvidenceKeys().size(), 15u);
}

TEST(CognitiveCurriculum, AllSixCentersNamed) {
    EXPECT_EQ(allCenters().size(), 6u);
    EXPECT_EQ(centerName(CenterId::Characters), "characters");
    EXPECT_EQ(centerName(CenterId::Memory), "memory");
    EXPECT_EQ(centerName(CenterId::CognitiveCycle), "cognitive_cycle");
    EXPECT_EQ(centerName(CenterId::Endocrine), "endocrine");
    EXPECT_EQ(centerName(CenterId::Protocol), "protocol");
    EXPECT_EQ(centerName(CenterId::Autonomy), "autonomy");
}

TEST(CognitiveCurriculum, FullEvidenceScoresOne) {
    CognitiveCurriculum cur;
    EvidenceMap em;
    for (const auto& k : propertyEvidenceKeys()) em[k] = 1.0;
    auto score = cur.scoreCenter(CenterId::Memory, em);
    EXPECT_DOUBLE_EQ(score.coherence, 1.0);
    EXPECT_DOUBLE_EQ(score.loss, 0.0);
}

TEST(CognitiveCurriculum, EmptyEvidenceScoresZero) {
    CognitiveCurriculum cur;
    EvidenceMap em;  // no keys -> all treated as 0
    auto score = cur.scoreCenter(CenterId::Memory, em);
    EXPECT_DOUBLE_EQ(score.coherence, 0.0);
    EXPECT_DOUBLE_EQ(score.loss, 1.0);
}

TEST(CognitiveCurriculum, ClampsOutOfRangeEvidence) {
    CognitiveCurriculum cur;
    EvidenceMap em;
    for (const auto& k : propertyEvidenceKeys()) em[k] = 5.0;  // out of range
    auto score = cur.scoreCenter(CenterId::Memory, em);
    EXPECT_DOUBLE_EQ(score.coherence, 1.0);  // clamped to 1.0
}

TEST(CognitiveCurriculum, StubScoresLowerThanLive) {
    CognitiveCurriculum cur;
    EvidenceMap live, stub;
    for (const auto& k : propertyEvidenceKeys()) { live[k] = 1.0; stub[k] = 0.0; }
    auto liveScore = cur.scoreCenter(CenterId::Characters, live);
    auto stubScore = cur.scoreCenter(CenterId::Protocol, stub);
    EXPECT_GT(liveScore.coherence, stubScore.coherence);
}

TEST(CognitiveCurriculum, ScorerIsDeterministic) {
    CognitiveCurriculum cur;
    EvidenceMap em;
    for (const auto& k : propertyEvidenceKeys()) em[k] = 0.5;
    auto a = cur.scoreCenter(CenterId::Endocrine, em);
    auto b = cur.scoreCenter(CenterId::Endocrine, em);
    EXPECT_DOUBLE_EQ(a.coherence, b.coherence);
}

TEST(CognitiveCurriculum, WeakestIsArgmaxLoss) {
    CognitiveCurriculum cur;
    std::map<CenterId, EvidenceMap> ev;
    for (CenterId c : allCenters()) {
        EvidenceMap em;
        for (const auto& k : propertyEvidenceKeys()) em[k] = 0.8;
        ev[c] = em;
    }
    for (auto& kv : ev[CenterId::Protocol]) kv.second = 0.1;  // make protocol weakest
    auto scores = cur.scoreAll(ev);
    auto weakest = cur.weakest(scores);
    EXPECT_EQ(weakest.center, CenterId::Protocol);
}

TEST(CognitiveCurriculum, ScoreAllFillsMissingCentersWithZero) {
    CognitiveCurriculum cur;
    std::map<CenterId, EvidenceMap> ev;  // empty -> all centers score 0
    auto scores = cur.scoreAll(ev);
    EXPECT_EQ(scores.size(), 6u);
    for (const auto& s : scores) {
        EXPECT_DOUBLE_EQ(s.coherence, 0.0);
    }
}

// ----------------------------------------------------------------------------
// HomeworkLoop - bounded cycle + alignment invariants
// ----------------------------------------------------------------------------

TEST(HomeworkLoop, SelectsWeakestCenter) {
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Memory, CenterId::Protocol);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    auto weakest = loop.selectWeakestCenter();
    EXPECT_EQ(weakest.center, CenterId::Protocol);
}

TEST(HomeworkLoop, CycleProducesHandoffSignal) {
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Memory, CenterId::Protocol);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    auto result = loop.runHomeworkCycleOnce();
    EXPECT_EQ(result.targetCenter, CenterId::Protocol);
    EXPECT_FALSE(result.handoffSignal.empty());
    EXPECT_NE(result.handoffSignal.find("protocol"), std::string::npos);
    EXPECT_EQ(loop.lastHandoffSignal(), result.handoffSignal);
}

TEST(HomeworkLoop, NeverIssuesDestructiveCommand) {
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Autonomy, CenterId::Endocrine);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    loop.runHomework(3);
    EXPECT_FALSE(loop.issuedDestructiveCommand());
}

TEST(HomeworkLoop, OnlyProposesMutationNeverCommits) {
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Memory, CenterId::Protocol);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    auto result = loop.runHomeworkCycleOnce();
    EXPECT_TRUE(result.proposedMutation);
    EXPECT_FALSE(loop.issuedDestructiveCommand());
    EXPECT_LE(result.stepsRun, 10u);  // the propose boundary
}

TEST(HomeworkLoop, ProposalIsQueuedAsGoal) {
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Memory, CenterId::Protocol);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    std::size_t goalsBefore = agent.getState().getGoals().size();
    loop.runHomeworkCycleOnce();
    std::size_t goalsAfter = agent.getState().getGoals().size();
    EXPECT_GT(goalsAfter, goalsBefore);

    // The newest proposal goal should reference the weakest center and be 'proposed'.
    bool foundProposal = false;
    for (const auto& g : agent.getState().getGoals()) {
        if (g.status == "proposed" &&
            g.description.find("protocol") != std::string::npos) {
            foundProposal = true;
        }
    }
    EXPECT_TRUE(foundProposal);
}

TEST(HomeworkLoop, HandoffRecordedToMemory) {
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Memory, CenterId::Protocol);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    loop.runHomeworkCycleOnce();
    const auto& messages = agent.getState().getRecentMessages();
    bool handoffInMemory = false;
    for (const auto& m : messages) {
        if (m && m->getContent().find("weakest=protocol") != std::string::npos) {
            handoffInMemory = true;
        }
    }
    EXPECT_TRUE(handoffInMemory);
}

TEST(HomeworkLoop, RunsRequestedCycleCount) {
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Memory, CenterId::Protocol);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    auto results = loop.runHomework(4);
    EXPECT_EQ(results.size(), 4u);
    EXPECT_EQ(loop.homeworkCycleCount(), 4u);
}

TEST(HomeworkLoop, DeltaTracksCoherenceChange) {
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Memory, CenterId::Protocol);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    auto result = loop.runHomeworkCycleOnce();
    EXPECT_DOUBLE_EQ(result.delta, result.coherenceAfter - result.coherenceBefore);
}

TEST(HomeworkLoop, DefaultProviderRewardsRunningAgent) {
    // With the default (real) evidence provider, an agent that has cycled should show
    // more living evidence than a fresh one -> coherence increases after running.
    AutonomousStarter agent(makeConfig());
    HomeworkLoop loop(agent, CognitiveCurriculum());  // default provider

    auto before = loop.scoreAllCenters();
    double sumBefore = 0.0;
    for (const auto& s : before) sumBefore += s.coherence;

    loop.runHomework(2);

    auto after = loop.scoreAllCenters();
    double sumAfter = 0.0;
    for (const auto& s : after) sumAfter += s.coherence;

    // Running homework records memories and cycles the agent, which the default provider
    // counts as living evidence; coherence must not decrease.
    EXPECT_GE(sumAfter, sumBefore);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


TEST(HomeworkLoop, DestructiveSignalStaysFalseEveryCycle) {
    // The non-destructive guarantee is now an explicitly-established per-cycle
    // signal (issuedDestructiveCommand_ is reset to false at the top of every
    // runHomeworkCycleOnce). Verify it is observably false after each individual
    // cycle, not merely after a batch -- so the guarantee is a live, checked
    // property rather than dead default state.
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Autonomy, CenterId::Protocol);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    for (int i = 0; i < 5; ++i) {
        loop.runHomeworkCycleOnce();
        EXPECT_FALSE(loop.issuedDestructiveCommand())
            << "homework loop issued a destructive command on cycle " << (i + 1);
    }
}

TEST(HomeworkLoop, HomeworkKeepsUnderlyingAgentAlive) {
    // The homework loop drives the real AutonomousStarter cognitive cycle. Because
    // the agent now enforces the never-dead-end invariant post-cycle, an agent
    // exercised purely through homework must still always have open work to do.
    AutonomousStarter agent(makeConfig());
    auto provider = std::make_shared<FixtureEvidenceProvider>(
        CenterId::Memory, CenterId::Endocrine);
    HomeworkLoop loop(agent, CognitiveCurriculum(), provider);

    loop.runHomework(8);
    EXPECT_GE(agent.getOpenGoalCount(), 1u);
}
