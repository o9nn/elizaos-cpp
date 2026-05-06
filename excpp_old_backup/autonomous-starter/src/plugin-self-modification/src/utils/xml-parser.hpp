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



// Configure parser with security settings
  // Security settings to prevent XXE attacks

// Sanitize XML input to prevent injection attacks
std::string sanitizeXml(const std::string& xmlString);

// Escape special XML characters
std::string escapeXml(const std::string& unsafe);

// Validate operation type

CharacterDiff parseCharacterDiff(const std::string& xmlString);

std::string buildCharacterDiffXml(CharacterDiff diff);

} // namespace elizaos
