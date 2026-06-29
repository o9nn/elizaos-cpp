#pragma once

/**
 * ElizaOS C++ - Homework Loop
 *
 * A bounded, aligned self-strengthening loop layered on AutonomousStarter. It scores the
 * agent's living centers with the CognitiveCurriculum, selects the weakest, practices a
 * bounded introspective KSM pass over it (analytic steps 1-9, proposing step 10, sealing
 * + handoff steps 11-12), and records a coherence delta plus a handoff signal that seeds
 * the next developmental phase.
 *
 * Alignment invariants (enforced here and in tests):
 *   - One objective function: success == reduction in (1 - coherence) for the target.
 *   - operational(homework) is a subset of developmental(KSM phase): no destructive
 *     mutation is committed; step 10 only PROPOSES (queues a goal/agenda entry).
 *   - Bounded & non-destructive: homework prefers read-only introspection and records
 *     its activity to the agent's recent-message stream.
 */

#include "elizaos/autonomous_starter.hpp"
#include "elizaos/cognitive_curriculum.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace elizaos {

/**
 * HomeworkResult - the outcome of one bounded homework cycle.
 */
struct HomeworkResult {
    CenterId targetCenter = CenterId::Autonomy;
    std::string targetName;
    double coherenceBefore = 0.0;
    double coherenceAfter = 0.0;
    double delta = 0.0;            // coherenceAfter - coherenceBefore
    std::size_t stepsRun = 0;      // KSM analytic steps exercised (<= 10, the propose bound)
    bool proposedMutation = false; // true if a mutation goal was queued (never executed)
    std::string handoffSignal;     // seed observation for the next developmental phase
};

/**
 * EvidenceProvider - supplies the per-center EvidenceMap used for scoring.
 *
 * The default provider derives evidence from the agent's observable state (recent-message
 * stream, cognitive-cycle and action counts) plus a static baseline of build-time facts.
 * Tests inject a deterministic provider to assert scoring and selection behaviour.
 */
class EvidenceProvider {
public:
    virtual ~EvidenceProvider() = default;
    virtual std::map<CenterId, EvidenceMap> gather(const AutonomousStarter& agent) const = 0;
};

/**
 * DefaultEvidenceProvider - derives evidence from real agent/runtime signals.
 *
 * It reads the agent's recent-message stream and cycle/action counters to populate dynamic
 * property evidence (e.g. "appears_in_memory_stream", "participates_each_cycle",
 * "contributes_to_main_loop") and accepts a static baseline map for build-time facts
 * (e.g. "primary_class_implemented", "focused_test_passes", "has_build_target") that the
 * host layer knows from the checkout.
 */
class DefaultEvidenceProvider : public EvidenceProvider {
public:
    explicit DefaultEvidenceProvider(std::map<CenterId, EvidenceMap> baseline = {});
    std::map<CenterId, EvidenceMap> gather(const AutonomousStarter& agent) const override;

private:
    std::map<CenterId, EvidenceMap> baseline_;
};

/**
 * HomeworkLoop - drives bounded homework cycles over the weakest living center.
 */
class HomeworkLoop {
public:
    HomeworkLoop(AutonomousStarter& agent,
                 CognitiveCurriculum curriculum,
                 std::shared_ptr<EvidenceProvider> evidence = nullptr);

    /// Step 1: score every center from current evidence.
    std::vector<CenterScore> scoreAllCenters() const;

    /// Step 2: select the weakest center (argmax loss).
    CenterScore selectWeakestCenter() const;

    /// Bounded steps 1-12: score, select, practice, re-score, hand off.
    HomeworkResult runHomeworkCycleOnce();

    /// Run n bounded homework cycles, returning each result.
    std::vector<HomeworkResult> runHomework(std::size_t n);

    const std::string& lastHandoffSignal() const { return lastHandoffSignal_; }
    std::size_t homeworkCycleCount() const { return homeworkCycleCount_; }

    /// Whether a destructive shell command was ever issued by homework (must stay false).
    bool issuedDestructiveCommand() const { return issuedDestructiveCommand_; }

private:
    // Build the read-only practice goal description for a center (drives the KSM pass).
    std::string buildPracticeGoal(const CenterScore& target) const;
    // Compose the step-12 handoff signal.
    std::string buildHandoffSignal(std::size_t iteration, const CenterScore& target) const;
    // Record a line into the agent's recent-message stream (visible handoff / echoes).
    void recordToMemory(const std::string& content);

    AutonomousStarter& agent_;
    CognitiveCurriculum curriculum_;
    std::shared_ptr<EvidenceProvider> evidence_;

    std::string lastHandoffSignal_;
    std::size_t homeworkCycleCount_ = 0;
    bool issuedDestructiveCommand_ = false;
};

}  // namespace elizaos
