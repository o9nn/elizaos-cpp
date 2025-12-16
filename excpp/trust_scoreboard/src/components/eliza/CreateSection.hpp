#include "CreateSection.module.css.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CreateSectionProps {
    std::string characterName;
    (name: string) => void setCharacterName;
    std::string selectedPersonality;
    (personality: string) => void setSelectedPersonality;
    std::vector<std::string> selectedSkills;
    (skills: string[]) => void setSelectedSkills;
};


} // namespace elizaos
