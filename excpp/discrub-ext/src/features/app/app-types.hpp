#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "classes/attachment.hpp"
#include "classes/channel.hpp"
#include "classes/embed.hpp"
#include "classes/emoji.hpp"
#include "classes/guild.hpp"
#include "classes/message.hpp"
#include "classes/reaction.hpp"
#include "classes/role.hpp"
#include "classes/user.hpp"
#include "enum/discrub-setting.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using AppState = {

using AppTask = {

using Timeout = {

using AppSettings = std::unordered_map<DiscrubSetting, std::string>;

} // namespace elizaos
