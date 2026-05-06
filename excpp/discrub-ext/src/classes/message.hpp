#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "attachment.hpp"
#include "channel.hpp"
#include "embed.hpp"
#include "reaction.hpp"
#include "types/application-object.hpp"
#include "types/channel-mention-object.hpp"
#include "types/component-object.hpp"
#include "types/message-activity-object.hpp"
#include "types/message-call-object.hpp"
#include "types/message-interaction-object.hpp"
#include "types/message-reference-object.hpp"
#include "types/resolved-data-object.hpp"
#include "types/role-subscription-data-object.hpp"
#include "types/sticker-item-object.hpp"
#include "types/sticker-object.hpp"
#include "user.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/channel#message-object

class Message {
public:
    Message(std::optional<std::any> opts);

private:
    Snowflake id_;
    Snowflake channel_id_;
    User author_;
    std::string content_;
    std::string timestamp_;
    std::variant<std::string, Maybe> edited_timestamp_;
    bool tts_;
    bool mention_everyone_;
    std::vector<User> mentions_;
    std::vector<Attachment> attachments_;
    std::vector<Embed> embeds_;
    bool pinned_;
    double type_;
};


} // namespace elizaos
