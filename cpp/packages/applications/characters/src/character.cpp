#include "character.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace elizaos::characters {
namespace {

float clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

std::string trimAndCollapseWhitespace(const std::string& input) {
    std::string out;
    out.reserve(input.size());

    bool previousSpace = true;
    for (unsigned char ch : input) {
        if (std::iscntrl(ch)) {
            continue;
        }
        if (std::isspace(ch)) {
            if (!previousSpace) {
                out.push_back(' ');
                previousSpace = true;
            }
            continue;
        }
        out.push_back(static_cast<char>(ch));
        previousSpace = false;
    }

    while (!out.empty() && out.back() == ' ') {
        out.pop_back();
    }
    return out;
}

std::string lowerAscii(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

void upsertTrait(CharacterProfile& profile, const CharacterTrait& trait) {
    auto it = std::find_if(profile.traits.begin(), profile.traits.end(), [&](const CharacterTrait& existing) {
        return existing.name == trait.name;
    });
    if (it == profile.traits.end()) {
        profile.traits.push_back(trait);
    } else {
        *it = trait;
    }
}

void addUniqueTag(CharacterProfile& profile, const std::string& tag) {
    const std::string cleaned = trimAndCollapseWhitespace(tag);
    if (cleaned.empty()) {
        return;
    }
    if (std::find(profile.tags.begin(), profile.tags.end(), cleaned) == profile.tags.end()) {
        profile.tags.push_back(cleaned);
    }
}

} // namespace

std::string sanitizeCharacterName(const std::string& name) {
    const std::string cleaned = trimAndCollapseWhitespace(name);
    return cleaned.empty() ? "Unnamed Character" : cleaned;
}

CharacterProfile createCharacterProfile(const std::string& name,
                                        const std::string& description,
                                        const std::string& archetype) {
    const std::string normalized = lowerAscii(trimAndCollapseWhitespace(archetype));

    CharacterProfile profile;
    if (normalized == "assistant" || normalized == "helper" || normalized == "companion") {
        profile = CharacterArchetypes::createHelper().instantiate(sanitizeCharacterName(name));
    } else if (normalized == "creative" || normalized == "artist") {
        profile = CharacterArchetypes::createArtist().instantiate(sanitizeCharacterName(name));
    } else if (normalized == "analytical" || normalized == "analyst" || normalized == "scientist") {
        profile = CharacterArchetypes::createScientist().instantiate(sanitizeCharacterName(name));
    } else if (normalized == "empathetic" || normalized == "mentor") {
        profile = CharacterArchetypes::createMentor().instantiate(sanitizeCharacterName(name));
    } else if (normalized == "leader") {
        profile = CharacterArchetypes::createLeader().instantiate(sanitizeCharacterName(name));
    } else if (normalized == "explorer") {
        profile = CharacterArchetypes::createExplorer().instantiate(sanitizeCharacterName(name));
    } else if (normalized == "guardian") {
        profile = CharacterArchetypes::createGuardian().instantiate(sanitizeCharacterName(name));
    } else if (normalized == "innovator") {
        profile = CharacterArchetypes::createInnovator().instantiate(sanitizeCharacterName(name));
    } else {
        profile = CharacterProfile(sanitizeCharacterName(name), trimAndCollapseWhitespace(description));
    }

    profile.name = sanitizeCharacterName(name);
    profile.description = trimAndCollapseWhitespace(description);
    if (!normalized.empty()) {
        addUniqueTag(profile, normalized);
    }
    return profile;
}

void applySeed(CharacterProfile& profile, const CharacterSeed& seed) {
    if (!seed.name.empty()) {
        profile.name = sanitizeCharacterName(seed.name);
    }

    const std::string cleanedDescription = trimAndCollapseWhitespace(seed.description);
    if (!cleanedDescription.empty()) {
        profile.description = cleanedDescription;
    }

    profile.personality = seed.personality;

    for (const auto& trait : seed.traits) {
        if (!trait.name.empty()) {
            upsertTrait(profile, trait);
        }
    }

    for (const auto& tag : seed.tags) {
        addUniqueTag(profile, tag);
    }

    for (const auto& [key, value] : seed.metadata) {
        if (!key.empty()) {
            profile.metadata[key] = value;
        }
    }

    const std::string archetype = trimAndCollapseWhitespace(seed.archetype);
    if (!archetype.empty()) {
        addUniqueTag(profile, lowerAscii(archetype));
    }
}

CharacterTrait makeNumericTrait(const std::string& name,
                                const std::string& description,
                                TraitCategory category,
                                float value,
                                float weight) {
    CharacterTrait trait(sanitizeCharacterName(name), description, category, TraitValueType::NUMERIC);
    trait.setNumericValue(clamp01(value));
    trait.weight = clamp01(weight);
    return trait;
}

std::string summarizeCharacter(const CharacterProfile& profile) {
    std::ostringstream summary;
    summary << sanitizeCharacterName(profile.name);
    if (!profile.description.empty()) {
        summary << " — " << profile.description;
    }
    summary << " [" << profile.personality.getPersonalityType() << "]";

    const auto dominantTraits = profile.personality.getDominantTraits();
    if (!dominantTraits.empty()) {
        summary << " dominant=";
        for (std::size_t i = 0; i < dominantTraits.size(); ++i) {
            if (i != 0) {
                summary << ',';
            }
            summary << dominantTraits[i];
        }
    }

    summary << " traits=" << profile.traits.size();
    summary << " experiences=" << profile.background.experiences.size();
    summary << " examples=" << profile.communicationStyle.speakingPatterns.size();
    return summary.str();
}

bool hasCoreIdentity(const CharacterProfile& profile) {
    return !sanitizeCharacterName(profile.name).empty()
        && !profile.personality.getPersonalityType().empty()
        && profile.personality.openness >= 0.0f
        && profile.personality.conscientiousness >= 0.0f
        && profile.personality.extraversion >= 0.0f
        && profile.personality.agreeableness >= 0.0f
        && profile.personality.neuroticism >= 0.0f;
}

} // namespace elizaos::characters
