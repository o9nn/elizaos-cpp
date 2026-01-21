#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_GUILD_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_GUILD_H
#include "core.h"
#include "../types/sticker-object.h"
#include "../types/welcome-screen-object.h"
#include "./emoji.h"
#include "./role.h"
using Role = _default;

class Guild;

class Guild : public object, public std::enable_shared_from_this<Guild> {
public:
    using std::enable_shared_from_this<Guild>::shared_from_this;
    std::shared_ptr<Snowflake> id;

    string name;

    any icon;

    any icon_hash;

    any splash;

    any discovery_splash;

    boolean owner;

    std::shared_ptr<Snowflake> owner_id;

    string permissions;

    any region;

    any afk_channel_id;

    double afk_timeout;

    boolean widget_enabled;

    any widget_channel_id;

    double verification_level;

    double default_message_notifications;

    double explicit_content_filter;

    array<std::shared_ptr<Role>> roles;

    array<std::shared_ptr<Emoji>> emojis;

    array<string> features;

    double mfa_level;

    any application_id;

    any system_channel_id;

    double system_channel_flags;

    any rules_channel_id;

    any max_presences;

    double max_members;

    any vanity_url_code;

    any description;

    any banner;

    double premium_tier;

    double premium_subscription_count;

    string preferred_locale;

    any public_updates_channel_id;

    double max_video_channel_users;

    double max_stage_video_channel_users;

    double approximate_member_count;

    double approximate_presence_count;

    WelcomeScreenObject welcome_screen;

    double nsfw_level;

    array<StickerObject> stickers;

    boolean premium_progress_bar_enabled;

    any safety_alerts_channel_id;

    Guild(object opts);
};

#endif
