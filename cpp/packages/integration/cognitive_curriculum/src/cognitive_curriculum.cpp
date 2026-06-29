/**
 * ElizaOS C++ - Cognitive Curriculum implementation.
 *
 * Deterministic, evidence-based scorer for the six living centers using Christopher
 * Alexander's 15 properties of living structure. See include/cognitive_curriculum.hpp and
 * the eliza-homework-loop skill for the design rationale.
 */

#include "elizaos/cognitive_curriculum.hpp"

#include <algorithm>

namespace elizaos {

std::string centerName(CenterId id) {
    switch (id) {
        case CenterId::Characters:     return "characters";
        case CenterId::Memory:         return "memory";
        case CenterId::CognitiveCycle: return "cognitive_cycle";
        case CenterId::Endocrine:      return "endocrine";
        case CenterId::Protocol:       return "protocol";
        case CenterId::Autonomy:       return "autonomy";
    }
    return "unknown";
}

const std::vector<CenterId>& allCenters() {
    static const std::vector<CenterId> kCenters = {
        CenterId::Characters,
        CenterId::Memory,
        CenterId::CognitiveCycle,
        CenterId::Endocrine,
        CenterId::Protocol,
        CenterId::Autonomy,
    };
    return kCenters;
}

const std::vector<std::string>& propertyEvidenceKeys() {
    // The 15 Alexander properties bound to observable evidence keys, in canonical order.
    static const std::vector<std::string> kKeys = {
        "has_coarse_and_fine_parts",   // 1  Levels of Scale
        "primary_class_implemented",   // 2  Strong Centers
        "has_api_boundary",            // 3  Boundaries
        "participates_each_cycle",     // 4  Alternating Repetition
        "has_build_target",            // 5  Positive Space
        "focused_test_passes",         // 6  Good Shape
        "operations_consistent",       // 7  Local Symmetries
        "referenced_by_other_center",  // 8  Deep Interlock & Ambiguity
        "distinct_from_neighbours",    // 9  Contrast
        "exposes_graded_surface",      // 10 Gradients
        "handles_edge_cases",          // 11 Roughness
        "appears_in_memory_stream",    // 12 Echoes
        "has_focused_core",            // 13 The Void
        "no_conflicting_paths",        // 14 Simplicity & Inner Calm
        "contributes_to_main_loop",    // 15 Not-Separateness
    };
    return kKeys;
}

CognitiveCurriculum::CognitiveCurriculum()
    : propertyKeys_(propertyEvidenceKeys()) {}

std::size_t CognitiveCurriculum::propertyCount() const {
    return propertyKeys_.size();
}

CenterScore CognitiveCurriculum::scoreCenter(CenterId center,
                                             const EvidenceMap& evidence) const {
    CenterScore score;
    score.center = center;
    score.name = centerName(center);

    if (propertyKeys_.empty()) {
        score.coherence = 0.0;
        score.loss = 1.0;
        return score;
    }

    double total = 0.0;
    for (const auto& key : propertyKeys_) {
        auto it = evidence.find(key);
        double v = (it == evidence.end()) ? 0.0 : it->second;
        // Clamp to [0,1] so malformed evidence cannot distort the score.
        v = std::max(0.0, std::min(1.0, v));
        total += v;
    }

    score.coherence = total / static_cast<double>(propertyKeys_.size());
    score.loss = 1.0 - score.coherence;
    return score;
}

std::vector<CenterScore> CognitiveCurriculum::scoreAll(
    const std::map<CenterId, EvidenceMap>& evidence) const {
    std::vector<CenterScore> scores;
    scores.reserve(allCenters().size());
    for (CenterId c : allCenters()) {
        auto it = evidence.find(c);
        if (it == evidence.end()) {
            scores.push_back(scoreCenter(c, EvidenceMap{}));
        } else {
            scores.push_back(scoreCenter(c, it->second));
        }
    }
    return scores;
}

CenterScore CognitiveCurriculum::weakest(const std::vector<CenterScore>& scores) const {
    CenterScore worst;
    worst.loss = -1.0;  // sentinel so any real score wins
    for (const auto& s : scores) {
        if (s.loss > worst.loss) {
            worst = s;
        }
    }
    if (worst.loss < 0.0) {
        // Empty input: return a default autonomy score with full loss.
        worst = CenterScore{};
        worst.name = centerName(worst.center);
    }
    return worst;
}

}  // namespace elizaos
