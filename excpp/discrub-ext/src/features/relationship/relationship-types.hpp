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

using RelationshipState = std::variant<{
  isLoading: boolean, Maybe>;
  friends: unknown[];
};

using AddFriendProps = {
  username: string;
  discriminator: string;
};

} // namespace elizaos
