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

/**
 * Learn more about light and dark modes:
 * https://docs.expo.dev/guides/color-schemes/
 */

void useThemeColor(std::optional<{ light: string; dark: string }> props, keyof typeof Colors.light & keyof typeof Colors.dark colorName);

} // namespace elizaos
