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

// https://discord.com/developers/docs/topics/teams#data-models-team-member-object
;

using TeamMemberObject = {
  membership_state: number;
  team_id: Snowflake;
  user: User;
  role: string;
};

} // namespace elizaos
