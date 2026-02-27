#pragma once
#include <algorithm>
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



// Configure parser with security settings
  // Security settings to prevent XXE attacks

// Sanitize XML input to prevent injection attacks
std: sanitizeXml(const std:& xmlString);

// Escape special XML characters
std: escapeXml(const std:& unsafe);

// Validate operation type

CharacterDiff parseCharacterDiff(const std:& xmlString);

std: buildCharacterDiffXml(CharacterDiff diff);

} // namespace elizaos
