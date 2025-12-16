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

using ChannelState = std::vector<{
  channels: Channel>;
  selectedChannel: Channel | Maybe;
  isLoading: boolean | Maybe;
  selectedExportChannels: Snowflake[];
};

} // namespace elizaos
