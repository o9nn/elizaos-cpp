#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"

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
    ConfidenceDecayManager(const std::optional<DecayConfig>& config);
    double getDecayedConfidence(Experience experience);
    std::vector<Experience> getExperiencesNeedingReinforcement(const std::vector<Experience>& experiences, number = 0::3 threshold);
    double calculateReinforcementBoost(Experience experience, number = 1::0 validationStrength);
    DecayConfig getDomainSpecificDecay(Experience experience);
    Array< getConfidenceTrend(Experience experience, number = 10 points);
    void for(auto let i = 0; i < points; i++);

private:
    DecayConfig config_;
};


} // namespace elizaos
