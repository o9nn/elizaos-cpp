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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Learn more about light and dark modes:
 * https://docs.expo.dev/guides/color-schemes/
 */

void useThemeColor(std::optional<std::any> props, keyof typeof Colors::light colorName);

} // namespace elizaos
