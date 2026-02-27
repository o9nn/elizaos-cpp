#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_CHANNEL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_CHANNEL_H
#include "core.h"
#include "../types/default-reaction-object.h"
#include "../types/forum-tag-object.h"
#include "../types/overwrite-object.h"
#include "../types/thread-member-object.h"
#include "../types/thread-metadata.h"
#include "./user.h"

class Channel;

class Channel : public object, public std::enable_shared_from_this<Channel> {
public:
    using std::enable_shared_from_this<Channel>::shared_from_this;
    std::shared_ptr<Snowflake> id;

    double type;

    std::shared_ptr<Snowflake> guild_id;

    double position;

    array<OverwriteObject> permission_overwrites;

    any name;

    any topic;

    boolean nsfw;

    any last_message_id;

    double bitrate;

    double user_limit;

    double rate_limit_per_user;

    array<std::shared_ptr<User>> recipients;

    any icon;

    std::shared_ptr<Snowflake> owner_id;

    std::shared_ptr<Snowflake> application_id;

    boolean managed;

    any parent_id;

    any last_pin_timestamp;

    any rtc_region;

    double video_quality_mode;

    double message_count;

    double member_count;

    ThreadMetaData thread_metadata;

    ThreadMemberObject member;

    double default_auto_archive_duration;

    string permissions;

    double flags;

    double total_message_sent;

    array<ForumTagObject> available_tags;

    array<std::shared_ptr<Snowflake>> applied_tags;

    any default_reaction_emoji;

    double default_thread_rate_limit_per_user;

    any default_sort_order;

    double default_forum_layout;

    Channel(object opts);
};

#endif
