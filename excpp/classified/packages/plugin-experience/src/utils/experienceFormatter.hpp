#include ".types.js.hpp"
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



std::string formatExperienceForDisplay(Experience experience);

std::string formatExperienceSummary(Experience experience);

std::string formatExperienceList(const std::vector<Experience>& experiences);

void getExperienceStats(const std::vector<Experience>& experiences); {

  // Count by type

  // Count by outcome

  // Count by domain

  // Calculate averages

  // Calculate success rate

std::string getTypeEmoji(ExperienceType type);

std::string formatExperienceForRAG(Experience experience);

std::vector<std::string> extractKeywords(Experience experience);

} // namespace elizaos
