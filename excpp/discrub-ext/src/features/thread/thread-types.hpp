#include "..classes/channel.hpp"
#include "..classes/message.hpp"
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

using ThreadState = std::vector<{
  threads: Channel>;
};

using ArchivedThreadProps = {
  channelId: string;
  knownThreads: Channel[];
};

using LiftPermissionProps = {
  channelId: string;
  noPermissionThreadIds: string[];
};

using ThreadsFromMessagesProps = std::vector<{
  messages: Message>;
  knownThreads: Channel[];
};

} // namespace elizaos
