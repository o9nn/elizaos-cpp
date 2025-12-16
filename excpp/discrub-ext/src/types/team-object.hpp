#include "team-member-object.hpp"
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

// https://discord.com/developers/docs/topics/teams#data-models-team-object
;

using TeamObject = std::variant<{
  icon: string, Maybe>;
  id: Snowflake;
  members: TeamMemberObject[];
  name: string;
  owner_user_id: Snowflake;
};

} // namespace elizaos
