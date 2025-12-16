#include "..classes/guild.hpp"
#include ".dm/dm-types.hpp"
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

;
;

using GuildState = std::vector<{
  guilds: Guild>;
  selectedGuild: Guild | Maybe;
  preFilterUserId: Snowflake | Maybe;
  preFilterUsers: PreFilterUser[];
  isLoading: boolean | Maybe;
};

} // namespace elizaos
