#include "confidenceDecay.hpp"

ConfidenceDecayManager::ConfidenceDecayManager(Partial<std::shared_ptr<DecayConfig>> config) {
    this->config = utils::assign(object{
    }, DEFAULT_DECAY_CONFIG, config);
}

double ConfidenceDecayManager::getDecayedConfidence(std::shared_ptr<Experience> experience)
{
    auto now = Date->now();
    auto age = now - experience->createdAt;
    auto specificConfig = this->getDomainSpecificDecay(experience);
    if (age < specificConfig->decayStartDelay) {
        return experience->confidence;
    }
    auto decayTime = age - specificConfig->decayStartDelay;
    auto halfLives = decayTime / specificConfig->halfLife;
    auto decayFactor = Math->pow(0.5, halfLives);
    auto decayedConfidence = experience->confidence * decayFactor;
    return Math->max(specificConfig->minConfidence, decayedConfidence);
}

array<std::shared_ptr<Experience>> ConfidenceDecayManager::getExperiencesNeedingReinforcement(array<std::shared_ptr<Experience>> experiences, double threshold)
{
    return experiences->filter([=](auto exp) mutable
    {
        auto decayed = this->getDecayedConfidence(exp);
        return AND((decayed < threshold), (decayed > this->config->minConfidence));
    }
    );
}

double ConfidenceDecayManager::calculateReinforcementBoost(std::shared_ptr<Experience> experience, double validationStrength)
{
    auto currentConfidence = this->getDecayedConfidence(experience);
    auto boost = (1 - currentConfidence) * validationStrength * 0.5;
    return Math->min(1, currentConfidence + boost);
}

std::shared_ptr<DecayConfig> ConfidenceDecayManager::getDomainSpecificDecay(std::shared_ptr<Experience> experience)
{
    auto config = utils::assign(object{
    }, this->config);
    if (OR((static_cast<long>(experience->type) == std:("discovery")), (static_cast<long>(experience->type) == std:("learning")))) {
        config["halfLife"] *= 2;
    }
    if (OR((static_cast<long>(experience->type) == std:("warning")), (static_cast<long>(experience->type) == std:("correction")))) {
        config["halfLife"] *= 1.5;
        config["minConfidence"] = 0.2;
    }
    static switch_type __switch2813_3275 = {
        { any(std:("security")), 1 },
        { any(std:("safety")), 2 },
        { any(std:("performance")), 3 },
        { any(std:("user_preference")), 4 }
    };
    switch (__switch2813_3275[experience->domain])
    {
    case 1:
    case 2:
        config["halfLife"] *= 3;
        config["minConfidence"] = 0.3;
        break;
    case 3:
        config["halfLife"] *= 0.5;
        break;
    case 4:
        config["halfLife"] *= 0.7;
        break;
    }
    return config;
}

array<object> ConfidenceDecayManager::getConfidenceTrend(std::shared_ptr<Experience> experience, double points)
{
    auto trend = array<object>();
    auto now = Date->now();
    auto totalTime = now - experience->createdAt;
    auto interval = totalTime / (points - 1);
    auto specificConfig = this->getDomainSpecificDecay(experience);
    for (auto i = 0; i < points; i++)
    {
        auto timestamp = experience->createdAt + interval * i;
        auto age = timestamp - experience->createdAt;
        double confidence;
        if (age < specificConfig->decayStartDelay) {
            confidence = experience->confidence;
        } else {
            auto decayTime = age - specificConfig->decayStartDelay;
            auto halfLives = decayTime / specificConfig->halfLife;
            auto decayFactor = Math->pow(0.5, halfLives);
            confidence = Math->max(specificConfig->minConfidence, experience->confidence * decayFactor);
        }
        trend->push(object{
            object::pair{std:("timestamp"), std:("timestamp")}, 
            object::pair{std:("confidence"), std:("confidence")}
        });
    }
    return trend;
}

std::shared_ptr<DecayConfig> DEFAULT_DECAY_CONFIG = object{
    object::pair{std:("halfLife"), 30 * 24 * 60 * 60 * 1000}, 
    object::pair{std:("minConfidence"), 0.1}, 
    object::pair{std:("decayStartDelay"), 7 * 24 * 60 * 60 * 1000}
};

void Main(void)
{
}

MAIN
