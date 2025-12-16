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

// https://discord.com/developers/docs/resources/guild#welcome-screen-object-welcome-screen-channel-structure
using WelcomeScreenChannelObject = {
  channel_id: Snowflake;
  description: string;
  emoji_id: Snowflake | Maybe;
  emoji_name: string | Maybe;
};

} // namespace elizaos
