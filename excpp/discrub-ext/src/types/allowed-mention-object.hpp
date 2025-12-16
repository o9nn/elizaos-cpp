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

// https://discord.com/developers/docs/resources/channel#allowed-mentions-object
using AllowedMentionObject = std::vector<{
  parse: string>;
  roles: Snowflake[];
  users: Snowflake[];
  replied_user: boolean;
};

} // namespace elizaos
