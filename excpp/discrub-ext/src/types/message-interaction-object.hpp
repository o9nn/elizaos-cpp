#include ".classes/user.hpp"
#include "guild-member-object.hpp"
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

// https://discord.com/developers/docs/interactions/receiving-and-responding#message-interaction-object-message-interaction-structure
;
;

using MessageInteractionObject = {
  id: Snowflake;
  type: string;
  name: string;
  user: User;
  member?: GuildMemberObject;
};

} // namespace elizaos
