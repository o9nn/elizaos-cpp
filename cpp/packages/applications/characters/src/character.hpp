#pragma once

#include "elizaos/characters.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos::characters {

/**
 * Small, value-oriented seed used by demos, generated bindings, and compatibility
 * code that need to materialize a CharacterProfile without depending on the full
 * TypeScript character-file schema.  Empty fields are intentionally ignored by
 * applySeed so callers can layer partial updates safely.
 */
struct CharacterSeed {
    std::string name;
    std::string description;
    std::string archetype;
    PersonalityMatrix personality;
    std::vector<CharacterTrait> traits;
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> metadata;
};

/** Normalize a display name into a safe non-empty profile name. */
std::string sanitizeCharacterName(const std::string& name);

/** Convenience factory for constructing a validated CharacterProfile. */
CharacterProfile createCharacterProfile(const std::string& name,
                                        const std::string& description = {},
                                        const std::string& archetype = {});

/** Apply a seed onto an existing profile, replacing same-named traits. */
void applySeed(CharacterProfile& profile, const CharacterSeed& seed);

/** Convenience constructor for a clamped numeric trait. */
CharacterTrait makeNumericTrait(const std::string& name,
                                const std::string& description,
                                TraitCategory category,
                                float value,
                                float weight = 1.0f);

/** Produce a compact human-readable profile summary for logs and diagnostics. */
std::string summarizeCharacter(const CharacterProfile& profile);

/** True when the profile has the minimum identity fields required for safe use. */
bool hasCoreIdentity(const CharacterProfile& profile);

} // namespace elizaos::characters
