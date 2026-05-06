#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct DecayConfig {
    double halfLife;
    double minConfidence;
    double decayStartDelay;
};

class ConfidenceDecayManager {
public:
    ConfidenceDecayManager(std::optional<DecayConfig> config = {});
    double getDecayedConfidence(Experience experience);
    std::vector<Experience> getExperiencesNeedingReinforcement(const std::vector<Experience>& experiences, double threshold = 0.3);
    double calculateReinforcementBoost(Experience experience, double validationStrength = 1.0);
    DecayConfig getDomainSpecificDecay(Experience experience);


} // namespace elizaos
