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
    std::size_t practiceDepth = 1; // bounded cognitive cycles run for this pass (adaptive)
};

/**
 * CoherenceTrend - the longitudinal coherence trajectory of one living center.
 *
 * The homework loop accumulates one sample per cycle in which the center was the
 * homework target, so the developmental phase can read the direction of travel
 * (improving / flat / regressing) rather than a single-cycle snapshot. slope is the
 * least-squares gradient of coherence over the sample index; a stagnant center
 * (|slope| < 1e-6 over >= 3 samples) is a signal that homework alone cannot lift it
 * and a developmental (human-led) mutation is required.
 */
struct CoherenceTrend {
    CenterId center = CenterId::Autonomy;
    std::string name;
    std::vector<double> samples;   // coherenceAfter per targeting cycle, oldest first
    double slope = 0.0;            // least-squares gradient over sample index
    bool stagnant = false;         // >= 3 samples and |slope| below the stagnation floor
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

    /**
     * Longitudinal coherence trend for a center (empty samples if never targeted).
     * The trend is recomputed on demand from accumulated per-cycle samples.
     */
    CoherenceTrend coherenceTrend(CenterId center) const;

    /// Trends for every center that has been targeted at least once, canonical order.
    std::vector<CoherenceTrend> allCoherenceTrends() const;

    /**
     * Adaptive practice depth for the next cycle targeting `center`: starts at 1 bounded
     * cognitive cycle and deepens (up to maxPracticeDepth) while the center's trend is
     * flat or regressing, mirroring how a student spends longer on stubborn material.
     * Depth never exceeds the propose boundary semantics: all cycles remain analytic and
     * non-destructive.
     */
    std::size_t adaptivePracticeDepth(CenterId center) const;

    /// Upper bound for adaptive practice depth (default 3, small to stay bounded).
    void setMaxPracticeDepth(std::size_t depth) { maxPracticeDepth_ = depth == 0 ? 1 : depth; }
    std::size_t maxPracticeDepth() const { return maxPracticeDepth_; }

    /// Rolling per-center trend window; at least three samples are retained for
    /// stagnation detection while preventing unbounded long-running growth.
    void setMaxTrendSamples(std::size_t samples) {
        maxTrendSamples_ = samples < 3 ? 3 : samples;
    }
    std::size_t maxTrendSamples() const { return maxTrendSamples_; }

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

    // Longitudinal coherence samples per center (only cycles where the center was
    // the homework target append a sample). Drives coherenceTrend() and
    // adaptivePracticeDepth().
    std::map<CenterId, std::vector<double>> coherenceHistory_;
    std::size_t maxPracticeDepth_ = 3;
    std::size_t maxTrendSamples_ = 64;
};

}  // namespace elizaos
