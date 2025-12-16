#include ".classes/user.hpp"
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

// https://discord.com/developers/docs/resources/guild#guild-member-object
;

using GuildMemberObject = {
  user?: User;
  nick?: string | Maybe;
  avatar?: string | Maybe;
  roles: Snowflake[];
  joined_at: string;
  premium_since?: string | Maybe;
  deaf: boolean;
  mute: boolean;
  flags: number;
  pending?: boolean;
  permissions?: string;
  communication_disabled_until?: string | Maybe;
};

} // namespace elizaos
