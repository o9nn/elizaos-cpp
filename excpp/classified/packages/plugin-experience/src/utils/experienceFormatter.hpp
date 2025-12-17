#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::string formatExperienceForDisplay(Experience experience);

std::string formatExperienceSummary(Experience experience);

std::string formatExperienceList(const std::vector<Experience>& experiences);

std::string formatPatternSummary(const std::any& pattern);


  // Count by type

  // Count by outcome

  // Count by domain

  // Calculate averages

  // Calculate success rate

std::string getTypeEmoji(ExperienceType type);

std::string formatExperienceForRAG(Experience experience);

std::vector<std::string> extractKeywords(Experience experience);

} // namespace elizaos
