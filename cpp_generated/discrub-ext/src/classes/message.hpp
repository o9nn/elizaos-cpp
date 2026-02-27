#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_MESSAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_MESSAGE_H
#include "core.h"
#include "./attachment.h"
using Attachment = _default;
#include "./embed.h"
using Embed = _default;
#include "../types/message-reference-object.h"
#include "./user.h"
#include "../types/channel-mention-object.h"
#include "./reaction.h"
#include "../types/message-activity-object.h"
#include "../types/application-object.h"
#include "../types/message-interaction-object.h"
#include "./channel.h"
using Channel = _default;
#include "../types/component-object.h"
#include "../types/sticker-item-object.h"
#include "../types/sticker-object.h"
#include "../types/role-subscription-data-object.h"
#include "../types/resolved-data-object.h"
#include "../types/message-call-object.h"

class Message;

class Message : public object, public std::enable_shared_from_this<Message> {
public:
    using std::enable_shared_from_this<Message>::shared_from_this;
    std::shared_ptr<Snowflake> id;

    std::shared_ptr<Snowflake> channel_id;

    MessageCallObject call;

    std::shared_ptr<User> author;

    string content;

    string timestamp;

    any edited_timestamp;

    boolean tts;

    boolean mention_everyone;

    array<std::shared_ptr<User>> mentions;

    array<ChannelMentionObject> mention_channels;

    array<std::shared_ptr<Attachment>> attachments;

    array<std::shared_ptr<Embed>> embeds;

    array<std::shared_ptr<Reaction>> reactions;

    any nonce;

    boolean pinned;

    std::shared_ptr<Snowflake> webhook_id;

    double type;

    MessageActivityObject activity;

    ApplicationObject application;

    std::shared_ptr<Snowflake> application_id;

    MessageReferenceObject message_reference;

    double flags;

    any referenced_message;

    MessageInteractionObject interaction;

    std::shared_ptr<Channel> thread;

    array<std::shared_ptr<ComponentObject>> components;

    array<StickerItemObject> sticker_items;

    array<StickerObject> stickers;

    double position;

    RoleSubscriptionDataObject role_subscription_data;

    std::shared_ptr<ResolvedDataObject> resolved;

    string userName;

    Message(object opts);
};

#endif
