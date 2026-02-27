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
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



Character applyOperationsToCharacter(Character character, const std::vector<ModificationOperation>& operations);

void addValue(const std:& obj, const std:& path, const std:& value);

void modifyValue(const std:& obj, const std:& path, const std:& value);

void deleteValue(const std:& obj, const std:& path);

bool validateCharacterStructure(const std:& character);

} // namespace elizaos
