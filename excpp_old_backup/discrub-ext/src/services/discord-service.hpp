#include ".classes/attachment.hpp"
#include ".classes/channel.hpp"
#include ".classes/embed.hpp"
#include ".classes/guild.hpp"
#include ".classes/message.hpp"
#include ".classes/role.hpp"
#include ".classes/user.hpp"
#include ".enum/query-string-param.hpp"
#include ".enum/reaction-type.hpp"
#include ".features/app/app-types.hpp"
#include ".features/message/message-types.hpp"
#include ".types/allowed-mention-object.hpp"
#include ".types/component-object.hpp"
#include ".types/default-reaction-object.hpp"
#include ".types/forum-tag-object.hpp"
#include ".types/guild-member-object.hpp"
#include ".types/overwrite-object.hpp"
#include ".utils.hpp"
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



using GuildChannelModify = {

using ThreadModify = {

using MessageModify = {

using SearchMessageResult = std::vector<{

class DiscordService {
  searchDelaySecs = 0;
  deleteDelaySecs = 0;
  DISCORD_API_URL = "https://discord.com/api/v10";
  DISCORD_USERS_ENDPOINT = `${this.DISCORD_API_URL}/users`;

      await wait(delay);

    return func();

      await wait(delay);

    return func();

          // Request was successful
            // Successful request has data
            // Successful request does not have data
          // Request must be re-attempted after x seconds
          await wait(json.retry_after);
          // Request failed for unknown reason


} // namespace elizaos
