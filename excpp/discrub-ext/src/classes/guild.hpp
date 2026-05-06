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
#include "emoji.hpp"
#include "role.hpp"
#include "types/sticker-object.hpp"
#include "types/welcome-screen-object.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/guild#guild-object

class Guild {
public:
    Guild(std::optional<std::any> opts);

private:
    Snowflake id_;
    std::string name_;
    std::variant<std::string, Maybe> icon_;
    std::variant<std::string, Maybe> splash_;
    std::variant<std::string, Maybe> discovery_splash_;
    Snowflake owner_id_;
    std::variant<Snowflake, Maybe> afk_channel_id_;
    double afk_timeout_;
    double verification_level_;
    double default_message_notifications_;
    double explicit_content_filter_;
    std::vector<Role> roles_;
    std::vector<Emoji> emojis_;
    std::vector<std::string> features_;
    double mfa_level_;
    std::variant<Snowflake, Maybe> application_id_;
    std::variant<Snowflake, Maybe> system_channel_id_;
    double system_channel_flags_;
    std::variant<Snowflake, Maybe> rules_channel_id_;
    std::variant<std::string, Maybe> vanity_url_code_;
    std::variant<std::string, Maybe> description_;
    std::variant<std::string, Maybe> banner_;
    double premium_tier_;
    std::string preferred_locale_;
    std::variant<Snowflake, Maybe> public_updates_channel_id_;
    double nsfw_level_;
    bool premium_progress_bar_enabled_;
    std::variant<Snowflake, Maybe> safety_alerts_channel_id_;
};


} // namespace elizaos
