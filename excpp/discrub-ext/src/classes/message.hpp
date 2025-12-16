#include ".types/application-object.hpp"
#include ".types/channel-mention-object.hpp"
#include ".types/component-object.hpp"
#include ".types/message-activity-object.hpp"
#include ".types/message-call-object.hpp"
#include ".types/message-interaction-object.hpp"
#include ".types/message-reference-object.hpp"
#include ".types/resolved-data-object.hpp"
#include ".types/role-subscription-data-object.hpp"
#include ".types/sticker-item-object.hpp"
#include ".types/sticker-object.hpp"
#include "attachment.hpp"
#include "channel.hpp"
#include "embed.hpp"
#include "reaction.hpp"
#include "user.hpp"
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

// https://discord.com/developers/docs/resources/channel#message-object
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
;
;
;
;
;
;

class Message {
  id: Snowflake;
  channel_id: Snowflake;
  call?: MessageCallObject;
  author: User;
  content: string;
  timestamp: string;
  edited_timestamp: string | Maybe;
  tts: boolean;
  mention_everyone: boolean;
  mentions: User[];
  mention_channels?: ChannelMentionObject[];
  attachments: Attachment[];
  embeds: Embed[];
  reactions?: Reaction[];
  nonce?: string | number;
  pinned: boolean;
  webhook_id?: Snowflake;
  type: number;
  activity?: MessageActivityObject;
  application?: ApplicationObject;
  application_id?: Snowflake;
  message_reference?: MessageReferenceObject;
  flags?: number;
  referenced_message?: Message | Maybe;
  interaction?: MessageInteractionObject;
  thread?: Channel;
  components?: ComponentObject[];
  sticker_items?: StickerItemObject[];
  stickers?: StickerObject[];
  position?: number;
  role_subscription_data?: RoleSubscriptionDataObject;
  resolved?: ResolvedDataObject;
  userName?: string; // Used for quick filtering purposes

  constructor(opts: {
    id: Snowflake;
    call?: MessageCallObject;
    channel_id: Snowflake;
    author: User;
    content: string;
    timestamp: string;
    edited_timestamp: string | Maybe;
    tts: boolean;
    mention_everyone: boolean;
    mentions: User[];
    mention_channels?: ChannelMentionObject[];
    attachments: Attachment[];
    embeds: Embed[];
    reactions?: Reaction[];
    nonce?: string | number;
    pinned: boolean;
    webhook_id?: Snowflake;
    type: number;
    activity?: MessageActivityObject;
    application?: ApplicationObject;
    application_id?: Snowflake;
    message_reference?: MessageReferenceObject;
    flags?: number;
    referenced_message?: Message | Maybe;
    interaction?: MessageInteractionObject;
    thread?: Channel;
    components?: ComponentObject[];
    sticker_items?: StickerItemObject[];
    stickers?: StickerObject[];
    position?: number;
    role_subscription_data?: RoleSubscriptionDataObject;
    resolved?: ResolvedDataObject;
  }) {
    this.id = opts.id;
    this.call = opts.call;
    this.channel_id = opts.channel_id;
    this.author = opts.author;
    this.content = opts.content;
    this.timestamp = opts.timestamp;
    this.edited_timestamp = opts.edited_timestamp;
    this.tts = opts.tts;
    this.mention_everyone = opts.mention_everyone;
    this.mentions = opts.mentions;
    this.mention_channels = opts.mention_channels;
    this.attachments = opts.attachments;
    this.embeds = opts.embeds;
    this.reactions = opts.reactions;
    this.nonce = opts.nonce;
    this.pinned = opts.pinned;
    this.webhook_id = opts.webhook_id;
    this.type = opts.type;
    this.activity = opts.activity;
    this.application = opts.application;
    this.application_id = opts.application_id;
    this.message_reference = opts.message_reference;
    this.flags = opts.flags;
    this.referenced_message = opts.referenced_message;
    this.interaction = opts.interaction;
    this.thread = opts.thread;
    this.components = opts.components;
    this.sticker_items = opts.sticker_items;
    this.stickers = opts.stickers;
    this.position = opts.position;
    this.role_subscription_data = opts.role_subscription_data;
    this.resolved = opts.resolved;
    this.userName = opts.author.username;
  }
}
default Message;

} // namespace elizaos
