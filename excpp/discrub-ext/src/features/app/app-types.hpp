#include "..classes/attachment.hpp"
#include "..classes/channel.hpp"
#include "..classes/embed.hpp"
#include "..classes/emoji.hpp"
#include "..classes/guild.hpp"
#include "..classes/message.hpp"
#include "..classes/reaction.hpp"
#include "..classes/role.hpp"
#include "..classes/user.hpp"
#include "..enum/discrub-setting.hpp"
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
;
;
;
;
;
;
;
;

using AppState = {
  discrubPaused: boolean;
  discrubCancelled: boolean;
  task: AppTask;
  settings: AppSettings;
};

using AppTask = {
  active: boolean;
  entity:
    | (Message & { _index?: number; _total?: number })
    | Channel
    | User
    | Guild
    | Reaction
    | Emoji
    | Role
    | Attachment
    | Embed
    | Maybe;
  statusText: string | Maybe;
};

using Timeout = {
  message: string;
  timeout: number;
};

using AppSettings = std::unordered_map<DiscrubSetting, std::string>;

} // namespace elizaos
