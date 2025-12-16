#include ".types.hpp"
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



ValidationResult validateCharacterDiff(CharacterDiff diff);

bool validateDataType(const std::any& value, const std::string& expectedType);

bool validateModificationRate(const std::vector<{ timestamp: Date }>& recentModifications, number = 5 maxPerHour, number = 20 maxPerDay);

} // namespace elizaos
