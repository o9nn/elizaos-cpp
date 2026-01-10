#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_UTILS_CONFIDENCEDECAY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_UTILS_CONFIDENCEDECAY_H
#include "core.h"
#include "../types.h"

class DecayConfig;
class ConfidenceDecayManager;

class DecayConfig : public object, public std::enable_shared_from_this<DecayConfig> {
public:
    using std::enable_shared_from_this<DecayConfig>::shared_from_this;
    double halfLife;

    double minConfidence;

    double decayStartDelay;
};

extern std::shared_ptr<DecayConfig> DEFAULT_DECAY_CONFIG;
class ConfidenceDecayManager : public object, public std::enable_shared_from_this<ConfidenceDecayManager> {
public:
    using std::enable_shared_from_this<ConfidenceDecayManager>::shared_from_this;
    std::shared_ptr<DecayConfig> config;

    ConfidenceDecayManager(Partial<std::shared_ptr<DecayConfig>> config = object{});
    virtual double getDecayedConfidence(std::shared_ptr<Experience> experience);
    virtual array<std::shared_ptr<Experience>> getExperiencesNeedingReinforcement(array<std::shared_ptr<Experience>> experiences, double threshold = 0.3);
    virtual double calculateReinforcementBoost(std::shared_ptr<Experience> experience, double validationStrength = 1);
    virtual std::shared_ptr<DecayConfig> getDomainSpecificDecay(std::shared_ptr<Experience> experience);
    virtual array<object> getConfidenceTrend(std::shared_ptr<Experience> experience, double points = 10);
};

#endif
