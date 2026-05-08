#include "character_evolution_evaluator.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_map>

namespace elizaos::characters {
namespace {

float clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

float averageAbsolutePersonalityDelta(const PersonalityMatrix& a, const PersonalityMatrix& b) {
    const float deltas[] = {
        std::fabs(a.openness - b.openness),
        std::fabs(a.conscientiousness - b.conscientiousness),
        std::fabs(a.extraversion - b.extraversion),
        std::fabs(a.agreeableness - b.agreeableness),
        std::fabs(a.neuroticism - b.neuroticism),
        std::fabs(a.creativity - b.creativity),
        std::fabs(a.empathy - b.empathy),
        std::fabs(a.assertiveness - b.assertiveness),
        std::fabs(a.curiosity - b.curiosity),
        std::fabs(a.loyalty - b.loyalty)
    };

    float total = 0.0f;
    for (float delta : deltas) {
        total += delta;
    }
    return total / static_cast<float>(sizeof(deltas) / sizeof(float));
}

float personalityStability(const PersonalityMatrix& matrix) {
    const float dimensions[] = {
        matrix.openness,
        matrix.conscientiousness,
        matrix.extraversion,
        matrix.agreeableness,
        matrix.neuroticism,
        matrix.creativity,
        matrix.empathy,
        matrix.assertiveness,
        matrix.curiosity,
        matrix.loyalty
    };

    float extremity = 0.0f;
    for (float value : dimensions) {
        extremity += std::fabs(clamp01(value) - 0.5f) * 2.0f;
    }
    return clamp01(1.0f - extremity / static_cast<float>(sizeof(dimensions) / sizeof(float)));
}

std::unordered_map<std::string, float> traitWeights(const CharacterProfile& profile) {
    std::unordered_map<std::string, float> weights;
    for (const auto& trait : profile.traits) {
        weights[trait.name] = trait.weight;
    }
    return weights;
}

CharacterTrait signalTrait(const CharacterEvolutionSignal& signal) {
    const std::string traitName = signal.interactionType.empty()
        ? "adaptive-learning"
        : "adaptive-" + signal.interactionType;

    CharacterTrait trait(traitName,
                         "Adaptive trait inferred from interaction outcome: " + signal.outcome,
                         TraitCategory::BEHAVIORAL,
                         TraitValueType::NUMERIC);
    trait.setNumericValue(clamp01(0.5f + signal.valence * 0.25f + signal.intensity * 0.25f));
    trait.weight = clamp01(0.35f + signal.intensity * 0.65f);
    trait.tags.push_back("evolution");
    trait.metadata["outcome"] = signal.outcome;
    return trait;
}

} // namespace

const char* evolutionAnalysisTemplate =
    "Compare the original and evolved ElizaOS character profiles. Focus on personality "
    "movement, stability, compatibility with the original identity, strengthened traits, "
    "and risks introduced by excessive drift.";

CharacterEvolutionReport evaluateCharacterEvolution(const CharacterProfile& original,
                                                    const CharacterProfile& evolved) {
    CharacterEvolutionReport report;
    report.personalityShift = averageAbsolutePersonalityDelta(original.personality, evolved.personality);
    report.stabilityDelta = personalityStability(evolved.personality) - personalityStability(original.personality);
    report.compatibilityWithOriginal = original.personality.calculateCompatibility(evolved.personality);

    const auto before = traitWeights(original);
    for (const auto& trait : evolved.traits) {
        auto it = before.find(trait.name);
        if (it == before.end()) {
            report.strengthenedTraits.push_back(trait.name);
        } else if (trait.weight > it->second + 0.05f) {
            report.strengthenedTraits.push_back(trait.name);
        }
    }

    if (report.personalityShift > 0.35f) {
        report.risks.push_back("high personality drift may weaken character continuity");
    }
    if (report.stabilityDelta < -0.2f) {
        report.risks.push_back("stability decreased substantially after evolution");
    }
    if (report.compatibilityWithOriginal < 0.55f) {
        report.risks.push_back("evolved profile is weakly compatible with the original identity");
    }

    std::ostringstream summary;
    summary << "shift=" << report.personalityShift
            << ", stability_delta=" << report.stabilityDelta
            << ", compatibility=" << report.compatibilityWithOriginal
            << ", strengthened_traits=" << report.strengthenedTraits.size()
            << ", risks=" << report.risks.size();
    report.summary = summary.str();
    return report;
}

std::vector<CharacterOperation> recommendEvolutionOperations(const CharacterProfile& profile,
                                                            const std::vector<CharacterEvolutionSignal>& signals) {
    std::vector<CharacterOperation> operations;

    float totalIntensity = 0.0f;
    float totalValence = 0.0f;
    int positiveSignals = 0;
    int challengingSignals = 0;

    for (const auto& signal : signals) {
        const float intensity = clamp01(signal.intensity);
        const float valence = std::max(-1.0f, std::min(1.0f, signal.valence));
        totalIntensity += intensity;
        totalValence += valence;
        if (valence >= 0.0f) {
            ++positiveSignals;
        } else {
            ++challengingSignals;
        }
        operations.push_back(CharacterOperation::upsertTrait(signalTrait(signal)));
        if (!signal.outcome.empty()) {
            operations.push_back(CharacterOperation::addExperience(signal.interactionType + ": " + signal.outcome));
        }
    }

    if (!signals.empty()) {
        const float avgIntensity = totalIntensity / static_cast<float>(signals.size());
        const float avgValence = totalValence / static_cast<float>(signals.size());

        operations.push_back(CharacterOperation::modifyValue(
            "personality.curiosity",
            clamp01(profile.personality.curiosity + avgIntensity * 0.05f)));
        operations.push_back(CharacterOperation::modifyValue(
            "personality.empathy",
            clamp01(profile.personality.empathy + std::max(0.0f, avgValence) * 0.05f)));

        if (challengingSignals > positiveSignals) {
            operations.push_back(CharacterOperation::modifyValue(
                "personality.neuroticism",
                clamp01(profile.personality.neuroticism + avgIntensity * 0.03f)));
            operations.push_back(CharacterOperation::addTag("requires-grounding"));
        } else {
            operations.push_back(CharacterOperation::modifyValue(
                "personality.loyalty",
                clamp01(profile.personality.loyalty + avgIntensity * 0.04f)));
            operations.push_back(CharacterOperation::addTag("positively-reinforced"));
        }
    }

    return operations;
}

CharacterProfile evolveCharacterFromSignals(const CharacterProfile& profile,
                                            const std::vector<CharacterEvolutionSignal>& signals,
                                            CharacterEvolutionReport* report) {
    const auto operations = recommendEvolutionOperations(profile, signals);
    const auto update = applyOperationsToCharacter(profile, operations);
    CharacterEvolutionReport localReport = evaluateCharacterEvolution(profile, update.profile);
    localReport.recommendedOperations = operations;
    if (!update.success) {
        localReport.risks.insert(localReport.risks.end(), update.warnings.begin(), update.warnings.end());
    }
    if (report != nullptr) {
        *report = localReport;
    }
    return update.profile;
}

} // namespace elizaos::characters
