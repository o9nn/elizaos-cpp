#pragma once

#include "character_updater.hpp"

#include <string>
#include <vector>

namespace elizaos::characters {

struct CharacterEvolutionSignal {
    std::string interactionType;
    std::string outcome;
    float intensity{0.5f};
    float valence{0.0f};
};

struct CharacterEvolutionReport {
    float personalityShift{0.0f};
    float stabilityDelta{0.0f};
    float compatibilityWithOriginal{1.0f};
    std::vector<std::string> strengthenedTraits;
    std::vector<std::string> risks;
    std::vector<CharacterOperation> recommendedOperations;
    std::string summary;
};

extern const char* evolutionAnalysisTemplate;

CharacterEvolutionReport evaluateCharacterEvolution(const CharacterProfile& original,
                                                    const CharacterProfile& evolved);

std::vector<CharacterOperation> recommendEvolutionOperations(const CharacterProfile& profile,
                                                            const std::vector<CharacterEvolutionSignal>& signals);

CharacterProfile evolveCharacterFromSignals(const CharacterProfile& profile,
                                            const std::vector<CharacterEvolutionSignal>& signals,
                                            CharacterEvolutionReport* report = nullptr);

} // namespace elizaos::characters
