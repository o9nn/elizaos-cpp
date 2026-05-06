#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/classes/guild.h"

Guild::Guild(object opts) {
    this->id = opts["id"];
    this->name = opts["name"];
    this->icon = opts["icon"];
    this->icon_hash = opts["icon_hash"];
    this->splash = opts["splash"];
    this->discovery_splash = opts["discovery_splash"];
    this->owner = opts["owner"];
    this->owner_id = opts["owner_id"];
    this->permissions = opts["permissions"];
    this->region = opts["region"];
    this->afk_channel_id = opts["afk_channel_id"];
    this->afk_timeout = opts["afk_timeout"];
    this->widget_enabled = opts["widget_enabled"];
    this->widget_channel_id = opts["widget_channel_id"];
    this->verification_level = opts["verification_level"];
    this->default_message_notifications = opts["default_message_notifications"];
    this->explicit_content_filter = opts["explicit_content_filter"];
    this->roles = opts["roles"];
    this->emojis = opts["emojis"];
    this->features = opts["features"];
    this->mfa_level = opts["mfa_level"];
    this->application_id = opts["application_id"];
    this->system_channel_id = opts["system_channel_id"];
    this->system_channel_flags = opts["system_channel_flags"];
    this->rules_channel_id = opts["rules_channel_id"];
    this->max_presences = opts["max_presences"];
    this->max_members = opts["max_members"];
    this->vanity_url_code = opts["vanity_url_code"];
    this->description = opts["description"];
    this->banner = opts["banner"];
    this->premium_tier = opts["premium_tier"];
    this->premium_subscription_count = opts["premium_subscription_count"];
    this->preferred_locale = opts["preferred_locale"];
    this->public_updates_channel_id = opts["public_updates_channel_id"];
    this->max_video_channel_users = opts["max_video_channel_users"];
    this->max_stage_video_channel_users = opts["max_stage_video_channel_users"];
    this->approximate_member_count = opts["approximate_member_count"];
    this->approximate_presence_count = opts["approximate_presence_count"];
    this->welcome_screen = opts["welcome_screen"];
    this->nsfw_level = opts["nsfw_level"];
    this->stickers = opts["stickers"];
    this->premium_progress_bar_enabled = opts["premium_progress_bar_enabled"];
    this->safety_alerts_channel_id = opts["safety_alerts_channel_id"];
}

