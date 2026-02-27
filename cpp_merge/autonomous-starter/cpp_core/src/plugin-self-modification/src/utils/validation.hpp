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
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



ValidationResult validateCharacterDiff(CharacterDiff diff);

bool validateDataType(const std:& value, const std:& expectedType);

bool validateModificationRate(const std::vector<std::string>& recentModifications, double maxPerHour = 5, double maxPerDay = 20);

} // namespace elizaos
