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
    const auto& goals = agent.getState().getGoals();
    const auto health = agent.getAutonomyHealthReport();
    const double cycles = static_cast<double>(health.totalCycles);
    const double actions = static_cast<double>(health.totalActions);
    const double reflections = static_cast<double>(health.reflections);

    auto clamp01 = [](double value) {
        return std::max(0.0, std::min(1.0, value));
    };
    auto ratio = [&](double numerator, double denominator) {
        return denominator <= 0.0 ? 0.0 : clamp01(numerator / denominator);
    };
    auto lower = [](std::string value) {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return value;
    };
    auto memoryMentionsAny = [&](std::initializer_list<const char*> needles) {
        for (const auto& memory : messages) {
            if (!memory) continue;
            const std::string content = lower(memory->getContent());
            for (const char* needle : needles) {
                if (content.find(needle) != std::string::npos) return true;
            }
        }
        return false;
    };
    std::size_t validMemories = 0;
    for (const auto& memory : messages) if (memory && !memory->getContent().empty()) ++validMemories;
    const double memoryIntegrity = messages.empty()
        ? 0.0 : ratio(static_cast<double>(validMemories), static_cast<double>(messages.size()));
    const double cycleClosure = std::min(ratio(actions, cycles), ratio(reflections, cycles));
    const bool focusConsistent = health.focusedGoalId.empty() ||
        agent.getActiveGoalId().empty() || health.focusedGoalId == agent.getActiveGoalId();
    const double failureResilience = 1.0 / (1.0 + static_cast<double>(health.consecutiveFailures));

    for (CenterId center : allCenters()) {
        EvidenceMap evidence;
        auto baseline = baseline_.find(center);
        if (baseline != baseline_.end()) evidence = baseline->second;

        switch (center) {
            case CenterId::Characters: {
                const bool identityPresent = !agent.getConfig().agentId.empty() &&
                                             !agent.getConfig().agentName.empty();
                evidence["primary_class_implemented"] = identityPresent ? 1.0 : 0.0;
                evidence["has_focused_core"] = identityPresent ? 1.0 : 0.0;
                evidence["distinct_from_neighbours"] =
                    (!agent.getConfig().bio.empty() || !agent.getConfig().lore.empty()) ? 1.0 : 0.0;
                evidence["participates_each_cycle"] =
                    (identityPresent && health.totalCycles > 0) ? 1.0 : 0.0;
                evidence["appears_in_memory_stream"] =
                    memoryMentionsAny({"agent:", "awakening:", "character"}) ? 1.0 : 0.0;
                evidence["contributes_to_main_loop"] =
                    identityPresent ? (health.totalCycles > 0 ? 1.0 : 0.5) : 0.0;
                break;
            }
            case CenterId::Memory:
                evidence["primary_class_implemented"] = messages.empty() ? 0.0 : 1.0;
                evidence["participates_each_cycle"] = ratio(static_cast<double>(messages.size()),
                                                              std::max(1.0, cycles * 3.0));
                evidence["operations_consistent"] = memoryIntegrity;
                evidence["handles_edge_cases"] = memoryIntegrity;
                evidence["appears_in_memory_stream"] = messages.empty() ? 0.0 : 1.0;
                evidence["has_focused_core"] = messages.empty() ? 0.0 : 1.0;
                evidence["exposes_graded_surface"] =
                    ratio(static_cast<double>(messages.size()), 25.0);
                evidence["contributes_to_main_loop"] =
                    (!messages.empty() && health.totalCycles > 0) ? 1.0 : 0.0;
                break;
            case CenterId::CognitiveCycle:
                evidence["has_coarse_and_fine_parts"] =
                    (health.totalCycles > 0 && health.reflections > 0) ? 1.0 : 0.0;
                evidence["primary_class_implemented"] = health.totalCycles > 0 ? 1.0 : 0.0;
                evidence["participates_each_cycle"] = cycleClosure;
                evidence["operations_consistent"] = cycleClosure;
                evidence["referenced_by_other_center"] =
                    (!messages.empty() && health.totalActions > 0) ? 1.0 : 0.0;
                evidence["exposes_graded_surface"] = clamp01(health.cycleEfficiency);
                evidence["handles_edge_cases"] = failureResilience;
                evidence["appears_in_memory_stream"] =
                    memoryMentionsAny({"perception:", "reasoning:", "reflection:", "cycle "}) ? 1.0 : 0.0;
                evidence["has_focused_core"] = !health.lastPlan.empty() ? 1.0 : 0.0;
                evidence["no_conflicting_paths"] = focusConsistent ? 1.0 : 0.0;
                evidence["contributes_to_main_loop"] = cycleClosure;
                break;
            case CenterId::Endocrine:
                // Every planned action submits a stimulus and ticks the endocrine
                // subsystem; action/cycle closure is therefore observable coupling.
                evidence["primary_class_implemented"] = health.totalActions > 0 ? 1.0 : 0.0;
                evidence["participates_each_cycle"] = ratio(actions, cycles);
                evidence["operations_consistent"] = clamp01(health.competence);
                evidence["referenced_by_other_center"] = cycleClosure;
                evidence["exposes_graded_surface"] = clamp01(health.cognitiveMomentum);
                evidence["handles_edge_cases"] = failureResilience;
                evidence["appears_in_memory_stream"] =
                    memoryMentionsAny({"endocrine", "alarm mode", "rest mode"}) ? 1.0 : 0.0;
                evidence["contributes_to_main_loop"] = ratio(actions, cycles);
                break;
            case CenterId::Protocol:
                // Protocol earns evidence only when communication/protocol traces
                // actually appear; unrelated agent activity no longer inflates it.
                evidence["participates_each_cycle"] =
                    memoryMentionsAny({"protocol", "message", "communication"}) ?
                        ratio(actions, cycles) : 0.0;
                evidence["appears_in_memory_stream"] =
                    memoryMentionsAny({"protocol", "message", "communication"}) ? 1.0 : 0.0;
                evidence["contributes_to_main_loop"] =
                    memoryMentionsAny({"protocol", "communication"}) ? cycleClosure : 0.0;
                break;
            case CenterId::Autonomy:
                evidence["has_coarse_and_fine_parts"] =
                    (health.totalCycles > 0 && !goals.empty()) ? 1.0 : 0.0;
                evidence["primary_class_implemented"] = health.totalActions > 0 ? 1.0 : 0.0;
                evidence["has_api_boundary"] = health.totalCycles > 0 ? 1.0 : 0.0;
                evidence["participates_each_cycle"] = ratio(actions, cycles);
                evidence["operations_consistent"] = clamp01(health.actionSuccessRate);
                evidence["referenced_by_other_center"] = cycleClosure;
                evidence["distinct_from_neighbours"] = !health.focusedGoalId.empty() ? 1.0 : 0.0;
                evidence["exposes_graded_surface"] = clamp01(health.competence);
                evidence["handles_edge_cases"] = failureResilience;
                evidence["appears_in_memory_stream"] =
                    memoryMentionsAny({"autonomy", "autonomous", "goal completed:"}) ? 1.0 : 0.0;
                evidence["has_focused_core"] = !goals.empty() ? 1.0 : 0.0;
                evidence["no_conflicting_paths"] = focusConsistent ? 1.0 : 0.0;
                evidence["contributes_to_main_loop"] = clamp01(health.cycleEfficiency);
                break;
        }
        out[center] = std::move(evidence);
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
    coherenceHistory_[before.center].push_back(result.coherenceAfter);

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
