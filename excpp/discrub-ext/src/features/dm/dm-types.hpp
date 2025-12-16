#include "..classes/channel.hpp"
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

using DmState = std::vector<{
  dms: Channel>;
  selectedDms: Channel[];
  isLoading: boolean | Maybe;
  preFilterUserId: Snowflake | Maybe;
  preFilterUsers: PreFilterUser[];
};

using PreFilterUser = std::variant<{
  name: string, Maybe>;
  id: Snowflake;
};

using SetSelectedDmsProps = std::vector<{
  dmIds: Snowflake>;
  preFilterUser: PreFilterUser;
};

} // namespace elizaos
