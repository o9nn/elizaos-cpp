#include "welcome-screen-channel-object.hpp"
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

// https://discord.com/developers/docs/resources/guild#welcome-screen-object
;

using WelcomeScreenObject = std::variant<{
  description: string, Maybe>;
  welcome_channels: WelcomeScreenChannelObject[];
};

} // namespace elizaos
