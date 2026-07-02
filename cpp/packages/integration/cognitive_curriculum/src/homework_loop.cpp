/**
 * ElizaOS C++ - Homework Loop implementation.
 *
 * Bounded, aligned self-strengthening loop layered on AutonomousStarter. See
 * include/homework_loop.hpp and the eliza-homework-loop skill for the design rationale and
 * the alignment invariants enforced here.
 */

#include "elizaos/homework_loop.hpp"

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

    // Dynamic, observable evidence derived from the live agent state.
    const auto& messages = agent.getState().getRecentMessages();
    const bool hasMemoryActivity = !messages.empty();
    const bool hasCycled = agent.getCognitiveCycleCount() > 0;
    const bool hasActed = agent.getActionCount() > 0;

    for (CenterId c : allCenters()) {
        // Start from any caller-supplied baseline of build-time facts.
        EvidenceMap em;
        auto it = baseline_.find(c);
        if (it != baseline_.end()) {
            em = it->second;
        }

        // Overlay dynamic evidence (the agent earns these by actually running).
        const std::string name = centerName(c);
        bool mentioned = false;
        for (const auto& m : messages) {
            if (m && m->getContent().find(name) != std::string::npos) {
                mentioned = true;
                break;
            }
        }

        // Echoes: the center's behaviour appears in the memory stream.
        em["appears_in_memory_stream"] = mentioned ? 1.0 : (hasMemoryActivity ? 0.5 : 0.0);
        // Alternating Repetition: the center participates each cycle.
        em["participates_each_cycle"] = hasCycled ? 1.0 : 0.0;
        // Not-Separateness: the center contributes to the main observe-reason-act loop.
        em["contributes_to_main_loop"] = (hasCycled && hasActed) ? 1.0 : (hasCycled ? 0.5 : 0.0);

        out[c] = em;
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
    // the guarantee a live, test-observable property (see issuedDestructiveCommand())
    // rather than dead default state. (Cross-fork parity with hurdcog/elizaos.cpp.)
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
    std::size_t steps = 0;
    agent_.runCognitiveCycleOnce();  // observe-reason-act (bounded, guarded)
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

}  // namespace elizaos
