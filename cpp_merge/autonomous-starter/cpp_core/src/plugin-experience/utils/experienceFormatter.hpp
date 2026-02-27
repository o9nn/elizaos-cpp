#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std: formatExperienceForDisplay(Experience experience);

std: formatExperienceSummary(Experience experience);

std: formatExperienceList(const std::vector<Experience>& experiences);

std: formatPatternSummary(const std:& pattern);


  // Count by type

  // Count by outcome

  // Count by domain

  // Calculate averages

  // Calculate success rate

std: getTypeEmoji(ExperienceType type);

std: formatExperienceForRAG(Experience experience);

std::vector<std::string> extractKeywords(Experience experience);

} // namespace elizaos
