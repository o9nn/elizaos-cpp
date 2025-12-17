#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using GuildChannelModify = {

using ThreadModify = {

using MessageModify = {

using SearchMessageResult = std::vector<{

class DiscordService {
public:
    DiscordService(std::optional<AppSettings> settings);
    void if(auto this.searchDelaySecs > 0);
    void if(auto this.deleteDelaySecs > 0);
    void while(auto !requestComplete);
    void catch(auto e);
    std::function<void()> URLSearchParams(const std::variant<preFilterUserId, , "null">& {
         this::generateSnowflake(searchAfterDate> min_id);
};


} // namespace elizaos
