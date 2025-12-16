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

using Maybe = std::variant<nullptr, std::nullopt>;
using Snowflake = std::string;
type DiscordApiResponse<T = void> = {
  success: boolean;
  data?: T;
};

} // namespace elizaos
