#pragma once

/**
 * ElizaOS C++ - Cognitive Curriculum
 *
 * Encodes the curriculum the agent has "learned": the six living centers, the 15
 * Christopher Alexander properties of living structure used to score them, and an
 * evidence-based, deterministic scorer that produces a Property Coherence score per
 * center.
 *
 * The curriculum is data-driven and dependency-free so it can be embedded in the
 * autonomy package. Evidence is supplied by the caller (the runtime/agent layer), never
 * read from wall-clock time or randomness, which keeps scoring reproducible and
 * unit-testable.
 *
 * This realises the eliza-homework-loop skill: it lets Eliza reason about its own
 * architecture in the same KSM grammar used during large human-led transformation phases,
 * so its operational self-improvement is a strict subset of the developmental cycle.
 */

#include <map>
#include <string>
#include <vector>
#include <cstddef>

namespace elizaos {

/// The six ElizaOS living centers, aligned to elizaos-cpp-ksm-creator.
enum class CenterId {
    Characters,
    Memory,
    CognitiveCycle,
    Endocrine,
    Protocol,
    Autonomy,
};

/// Stable string name for a center (used in goals, memory, and handoff signals).
std::string centerName(CenterId id);

/// All centers in a stable canonical order.
const std::vector<CenterId>& allCenters();

/**
 * EvidenceMap - the observable signals the scorer reads for a single center.
 *
 * Keys are the 15 property evidence keys (e.g. "primary_class_implemented",
 * "focused_test_passes", "appears_in_memory_stream"). Values are graded in [0,1]:
 * 1.0 = fully satisfied, 0.0 = absent. Missing keys are treated as 0.0, which enforces
 * the Generative Garden Rule (stubs cannot inflate the score).
 */
using EvidenceMap = std::map<std::string, double>;

/// The 15 Alexander property evidence keys, in canonical order.
const std::vector<std::string>& propertyEvidenceKeys();

/**
 * CenterScore - a center's coherence evaluation.
 */
struct CenterScore {
    CenterId center = CenterId::Autonomy;
    std::string name;
    double coherence = 0.0;  // PropertyCoherenceScore in [0,1]
    double loss = 1.0;       // 1 - coherence
};

/**
 * CognitiveCurriculum - holds the centers and scores them from evidence.
 *
 * The class owns no agent state. It is constructed once and asked to score centers given
 * an EvidenceMap per center. This separation makes the scorer a pure function of evidence
 * (testable in isolation from the live agent).
 */
class CognitiveCurriculum {
public:
    CognitiveCurriculum();

    /// Score a single center from its evidence. coherence = mean of the 15 property scores.
    CenterScore scoreCenter(CenterId center, const EvidenceMap& evidence) const;

    /// Score all centers given per-center evidence. Missing centers score 0 coherence.
    std::vector<CenterScore> scoreAll(const std::map<CenterId, EvidenceMap>& evidence) const;

    /// The weakest center by loss (argmax loss). Ties broken by canonical order.
    CenterScore weakest(const std::vector<CenterScore>& scores) const;

    /// Number of properties (always 15) - exposed for tests/diagnostics.
    std::size_t propertyCount() const;

private:
    std::vector<std::string> propertyKeys_;
};

}  // namespace elizaos
