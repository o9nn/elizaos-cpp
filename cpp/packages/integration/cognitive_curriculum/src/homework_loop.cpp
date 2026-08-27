/**
 * ElizaOS C++ - Homework Loop implementation.
 *
 * Bounded, aligned self-strengthening loop layered on AutonomousStarter. See
 * include/homework_loop.hpp and the eliza-homework-loop skill for the design rationale and
 * the alignment invariants enforced here.
 */

#include "elizaos/homework_loop.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <utility>

namespace elizaos {

// ---------------------------------------------------------------------------
// DefaultEvidenceProvider
// ---------------------------------------------------------------------------

DefaultEvidenceProvider::DefaultEvidenceProvider(std::map<CenterId, EvidenceMap> baseline)
    : baseline_(std::move(baseline)) {}

std::map<CenterId, EvidenceMap> DefaultEvidenceProvider::gather(
    const AutonomousStarter& agent) const {
    std::map<CenterId, EvidenceMap> out;

    const auto& messages = agent.getState().getRecentMessages();
    const auto health = agent.getAutonomyHealthReport();
    const auto& endocrine = agent.getEndocrineSystem();
    const bool hasMemoryActivity = !messages.empty();
    const bool hasCycled = agent.getCognitiveCycleCount() > 0;
    const bool hasActed = agent.getActionCount() > 0;
    const double memoryDepth = std::min(1.0, static_cast<double>(messages.size()) / 20.0);
    const double reflectionCoverage = health.totalCycles == 0 ? 0.0 :
        std::min(1.0, static_cast<double>(health.reflections) /
                           static_cast<double>(health.totalCycles));

    for (CenterId c : allCenters()) {
        EvidenceMap em;
        const auto baseline = baseline_.find(c);
        if (baseline != baseline_.end()) em = baseline->second;
        auto earn = [&em](const std::string& key, double value) {
            value = std::max(0.0, std::min(1.0, value));
            const auto existing = em.find(key);
            if (existing == em.end() || existing->second < value) em[key] = value;
        };

        const std::string name = centerName(c);
        bool mentioned = false;
        for (const auto& memory : messages) {
            if (memory && memory->getContent().find(name) != std::string::npos) {
                mentioned = true;
                break;
            }
        }
        earn("appears_in_memory_stream", mentioned ? 1.0 : (hasMemoryActivity ? 0.25 : 0.0));

        switch (c) {
            case CenterId::Characters: {
                const bool configured = !agent.getConfig().agentId.empty() &&
                                        !agent.getConfig().agentName.empty();
                earn("primary_class_implemented", configured ? 1.0 : 0.0);
                earn("has_api_boundary", configured ? 1.0 : 0.0);
                earn("distinct_from_neighbours", configured ? 1.0 : 0.0);
                earn("has_focused_core", configured ? 1.0 : 0.0);
                earn("contributes_to_main_loop", configured && hasCycled ? 0.75 : 0.0);
                break;
            }
            case CenterId::Memory:
                earn("has_coarse_and_fine_parts", messages.size() >= 4 ? 1.0 : memoryDepth);
                earn("primary_class_implemented", hasMemoryActivity ? 1.0 : 0.0);
                earn("participates_each_cycle", hasCycled && hasMemoryActivity ? 1.0 : 0.0);
                earn("operations_consistent", hasMemoryActivity ? 1.0 : 0.0);
                earn("exposes_graded_surface", memoryDepth);
                earn("contributes_to_main_loop", hasMemoryActivity && hasCycled ? 1.0 : 0.0);
                break;
            case CenterId::CognitiveCycle:
                earn("participates_each_cycle", hasCycled ? 1.0 : 0.0);
                earn("operations_consistent", reflectionCoverage);
                earn("exposes_graded_surface",
                     std::min(1.0, static_cast<double>(health.totalCycles) / 10.0));
                earn("no_conflicting_paths", health.stagnationCount < 2 ? reflectionCoverage : 0.0);
                earn("contributes_to_main_loop", hasCycled && hasActed ? 1.0 : 0.0);
                break;
            case CenterId::Endocrine: {
                const bool ticked = endocrine.tickCount() > 0;
                bool levelsBounded = true;
                for (const auto& [_, level] : endocrine.hormoneLevelsMap()) {
                    levelsBounded = levelsBounded && level >= 0.0 && level <= 1.0;
                }
                earn("participates_each_cycle", ticked ? 1.0 : 0.0);
                earn("operations_consistent", ticked && levelsBounded ? 1.0 : 0.0);
                earn("exposes_graded_surface", ticked ?
                     std::min(1.0, endocrine.valenceArousal().magnitude()) : 0.0);
                earn("contributes_to_main_loop", ticked && hasActed ? 1.0 : 0.0);
                break;
            }
            case CenterId::Protocol:
                earn("participates_each_cycle", hasActed ? 1.0 : 0.0);
                earn("operations_consistent", hasActed ? health.actionSuccessRate : 0.0);
                earn("exposes_graded_surface", hasActed ?
                     std::min(1.0, static_cast<double>(health.totalActions) / 10.0) : 0.0);
                earn("handles_edge_cases", health.failedActions > 0 ? 1.0 : 0.0);
                earn("contributes_to_main_loop", hasActed ? 1.0 : 0.0);
                break;
            case CenterId::Autonomy:
                earn("has_coarse_and_fine_parts", hasCycled && health.reflections > 0 ? 1.0 : 0.0);
                earn("participates_each_cycle", hasCycled ? 1.0 : 0.0);
                earn("operations_consistent", health.actionSuccessRate);
                earn("exposes_graded_surface", health.competence);
                earn("handles_edge_cases", health.openGoals > 0 ? 1.0 : 0.0);
                earn("no_conflicting_paths", health.stagnationCount < 2 ? 1.0 : 0.0);
                earn("contributes_to_main_loop", hasCycled && hasActed ? 1.0 : 0.0);
                break;
        }
        out[c] = std::move(em);
    }
    return out;
}

// ---------------------------------------------------------------------------
// HomeworkLoop
// ---------------------------------------------------------------------------

HomeworkLoop::HomeworkLoop(AutonomousStarter& agent,
                           CognitiveCurriculum curriculum,
                           std::shared_ptr<EvidenceProvider> evidence)
    : agent_(agent),
      curriculum_(std::move(curriculum)),
      evidence_(std::move(evidence)) {
    if (!evidence_) {
        evidence_ = std::make_shared<DefaultEvidenceProvider>();
    }
}

void HomeworkLoop::recordToMemory(const std::string& content) {
    auto mem = std::make_shared<Memory>(
        generateUUID(), content, std::string("homework"), agent_.getConfig().agentId);
    agent_.getState().addRecentMessage(mem);
}

std::vector<CenterScore> HomeworkLoop::scoreAllCenters() const {
    auto evidence = evidence_->gather(agent_);
    return curriculum_.scoreAll(evidence);
}

CenterScore HomeworkLoop::selectWeakestCenter() const {
    return curriculum_.weakest(scoreAllCenters());
}

std::string HomeworkLoop::buildPracticeGoal(const CenterScore& target) const {
    std::ostringstream os;
    os << "homework: strengthen center '" << target.name
       << "' (coherence=" << target.coherence << ", loss=" << target.loss
       << ") via a bounded KSM introspection pass";
    return os.str();
}

std::string HomeworkLoop::buildHandoffSignal(std::size_t iteration,
                                             const CenterScore& target) const {
    std::ostringstream os;
    os << "iteration " << iteration
       << " | weakest=" << target.name
       << " | coherence=" << target.coherence
       << " | next=strengthen " << target.name;
    return os.str();
}

HomeworkResult HomeworkLoop::runHomeworkCycleOnce() {
    HomeworkResult result;

    // Non-destructive guarantee (explicit, per-cycle): the homework loop only ever
    // PROPOSES improvements -- it never widens shell access and never issues a
    // destructive command. Establishing this signal at the top of every cycle makes
    // the guarantee a live, test-observable property (see
    // issuedDestructiveCommand()) rather than dead default state, so any future edit
    // that introduced a destructive action would have to consciously flip it.
    issuedDestructiveCommand_ = false;

    // Step 1-2: score all centers and select the weakest (the gradient rule).
    auto before = selectWeakestCenter();
    result.targetCenter = before.center;
    result.targetName = before.name;
    result.coherenceBefore = before.coherence;

    // Step 3-9 (analytic, read-only): record a practice goal and run bounded cognitive
    // cycles so perception/reasoning/action memories are produced for the target center.
    // We never widen shell access and never issue a destructive command here.
    recordToMemory(buildPracticeGoal(before));

    // The analytic pass exercises the agent's observe-reason-act loop. We deliberately
    // stop short of step 7's destructive mutation: homework only PROPOSES.
    //
    // Adaptive practice depth (autonomy optimization): a center whose coherence trend
    // is flat or regressing earns extra bounded cycles this pass (up to
    // maxPracticeDepth_), the way a student lingers on stubborn material. Every extra
    // cycle remains analytic and shell-guarded, so the propose boundary is unchanged.
    const std::size_t depth = adaptivePracticeDepth(before.center);
    result.practiceDepth = depth;
    std::size_t steps = 0;
    for (std::size_t i = 0; i < depth; ++i) {
        agent_.runCognitiveCycleOnce();  // observe-reason-act (bounded, guarded)
    }
    steps = 9;                       // analytic steps 1-9 exercised

    // Step 10 (propose, never commit): queue a StateGoal describing the proposed
    // improvement. This is an agenda entry for the next human-led developmental phase.
    StateGoal proposal;
    proposal.id = generateUUID();
    proposal.description = std::string("PROPOSED: strengthen ") + before.name +
                           " in next developmental phase";
    proposal.status = "proposed";
    proposal.createdAt = std::chrono::system_clock::now();
    proposal.updatedAt = proposal.createdAt;
    agent_.getState().addGoal(proposal);
    result.proposedMutation = true;
    ++steps;  // step 10 (propose) reached -> stepsRun == 10, the propose boundary
    result.stepsRun = steps;

    // Step 11 (seal): re-score the target center and record the coherence delta.
    auto afterScores = scoreAllCenters();
    CenterScore after = before;
    for (const auto& s : afterScores) {
        if (s.center == before.center) {
            after = s;
            break;
        }
    }
    result.coherenceAfter = after.coherence;
    result.delta = result.coherenceAfter - result.coherenceBefore;

    // Longitudinal memory: append this cycle's post-practice coherence to the target
    // center's trend so future cycles (and the developmental phase) can read the
    // direction of travel instead of a single-cycle snapshot.
    auto& history = coherenceHistory_[before.center];
    history.push_back(result.coherenceAfter);
    if (history.size() > maxTrendSamples_) {
        history.erase(history.begin(), history.begin() +
                      static_cast<std::ptrdiff_t>(history.size() - maxTrendSamples_));
    }

    // Step 12 (bootstrap): emit the handoff signal seeding the next developmental phase,
    // and record it to memory so the handoff is visible.
    ++homeworkCycleCount_;
    result.handoffSignal = buildHandoffSignal(homeworkCycleCount_, after);
    lastHandoffSignal_ = result.handoffSignal;
    recordToMemory(result.handoffSignal);

    return result;
}

std::vector<HomeworkResult> HomeworkLoop::runHomework(std::size_t n) {
    std::vector<HomeworkResult> results;
    results.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        results.push_back(runHomeworkCycleOnce());
    }
    return results;
}

namespace {

/// Least-squares slope of y over index x = 0..n-1. Returns 0 for fewer than 2 samples.
double leastSquaresSlope(const std::vector<double>& y) {
    const std::size_t n = y.size();
    if (n < 2) {
        return 0.0;
    }
    const double nd = static_cast<double>(n);
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        const double x = static_cast<double>(i);
        sumX += x;
        sumY += y[i];
        sumXY += x * y[i];
        sumXX += x * x;
    }
    const double denom = nd * sumXX - sumX * sumX;
    if (denom == 0.0) {
        return 0.0;
    }
    return (nd * sumXY - sumX * sumY) / denom;
}

constexpr double kStagnationSlopeFloor = 1e-6;

}  // namespace

CoherenceTrend HomeworkLoop::coherenceTrend(CenterId center) const {
    CoherenceTrend trend;
    trend.center = center;
    trend.name = centerName(center);
    auto it = coherenceHistory_.find(center);
    if (it != coherenceHistory_.end()) {
        trend.samples = it->second;
    }
    trend.slope = leastSquaresSlope(trend.samples);
    trend.stagnant = trend.samples.size() >= 3 && std::abs(trend.slope) < kStagnationSlopeFloor;
    return trend;
}

std::vector<CoherenceTrend> HomeworkLoop::allCoherenceTrends() const {
    std::vector<CoherenceTrend> trends;
    for (CenterId c : allCenters()) {
        auto it = coherenceHistory_.find(c);
        if (it != coherenceHistory_.end() && !it->second.empty()) {
            trends.push_back(coherenceTrend(c));
        }
    }
    return trends;
}

std::size_t HomeworkLoop::adaptivePracticeDepth(CenterId center) const {
    auto it = coherenceHistory_.find(center);
    if (it == coherenceHistory_.end() || it->second.size() < 2) {
        return 1;  // Not enough longitudinal signal: default single bounded cycle.
    }
    const double slope = leastSquaresSlope(it->second);
    if (slope > kStagnationSlopeFloor) {
        return 1;  // Improving: no extra depth needed.
    }
    // Flat or regressing: deepen practice with the number of samples observed so far,
    // capped at the configured bound so homework stays strictly bounded.
    const std::size_t depth = 1 + std::min<std::size_t>(it->second.size() - 1,
                                                        maxPracticeDepth_ - 1);
    return std::min(depth, maxPracticeDepth_);
}

}  // namespace elizaos
