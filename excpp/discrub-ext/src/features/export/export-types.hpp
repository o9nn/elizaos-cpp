#include "..classes/channel.hpp"
#include "..classes/guild.hpp"
#include "..classes/message.hpp"
#include "..enum/export-type.hpp"
#include "export-utils.hpp"
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

using ExportState = {
  isExporting: boolean;
  name: string;
  isGenerating: boolean;
  currentPage: number;
  totalPages: number;
  exportMaps: ExportMap;
  exportMessages: Message[];
  currentExportEntity: Guild | Channel | Maybe;
};

using ExportMap = {
  /** Cleared at the end of an **/
  userMap: ExportUserMap;
  emojiMap: ExportEmojiMap;
  avatarMap: ExportAvatarMap;
  mediaMap: ExportMediaMap;
  roleMap: ExportRoleMap;
  /*************************************/

  /** Cleared when messages are reset **/
  reactionMap: ExportReactionMap;
  /*************************************/
};

using ExportReaction = {
  id: Snowflake;
  burst: boolean;
};

/**
 * This is a 'Message Id & emoji -> Export Reaction List' map.
 */
using ExportReactionMap = std::vector<{
  [id: Snowflake]: {
    [emoji: string]: ExportReaction>;
  };
};

/**
 * This is a 'User Id -> User Information' map.
 */
using ExportUserMap = std::variant<{
  [id: Snowflake]: {
    userName: string, Maybe>;
    displayName: string | Maybe;
    avatar: string | Maybe;
    guilds: {
      [guildId: Snowflake]: {
        roles: Snowflake[];
        nick: string | Maybe;
        joinedAt: string | Maybe;
      };
    };
  };
};

/**
 * This is an 'Emoji Id -> Local File Path' map.
 */
using ExportEmojiMap = {
  [id: Snowflake]: string;
};

/**
 * This is an 'User Id and Avatar -> Local File Path' map.
 * @example idAndAvatar = "1234567/s3oma03mdsm" where "1234567" is a User Id and "s3oma03mdsm" is an Avatar
 */
using ExportAvatarMap = {
  [idAndAvatar: string]: string;
};

/**
 * This is a 'Remote URL -> Local File Path' map.
 */
using ExportMediaMap = {
  [remoteUrl: string]: string;
};

/**
 * This is a 'Remote URL -> Local File Path' map.
 */
using ExportRoleMap = {
  [remoteUrl: string]: string;
};

using SpecialFormatting = std::vector<{
  userMention: UserMentionRef>;
  channel: ChannelRef[];
  underLine: UnderlineRef[];
  code: CodeRef[];
  italics: ItalicRef[];
  bold: BoldRef[];
  link: LinkRef[];
  quote: QuoteRef[];
  hyperLink: HyperlinkRef[];
  emoji: EmojiRef[];
};

using UserMentionRef = { raw: string; userName: string; id: Snowflake };
using ChannelRef = std::variant<{ channelId: Snowflake, Maybe>; raw: string };
using UnderlineRef = { text: string; raw: string };
using CodeRef = { text: string; raw: string };
using ItalicRef = { text: string; raw: string };
using BoldRef = { text: string; raw: string };
using LinkRef = {
  url: string;
  text: string;
  description: string;
  raw: string;
};
using QuoteRef = { text: string; raw: string };
using HyperlinkRef = { raw: string };
using EmojiRef = { raw: string; name: string; id: Snowflake };

using FilesFromMessagesProps = {
  message: Message;
  exportUtils: ExportUtils;
  paths: { media: string };
  index: number;
};

using AvatarFromMessageProps = {
  message: Message;
  exportUtils: ExportUtils;
};

using GetEmojiProps = {
  emojiRef: EmojiRef;
  isReply: boolean;
  exportView: boolean;
  message: Message;
};

using FormattedInnerHtmlProps = {
  content: string;
  isReply: boolean;
  exportView: boolean;
  message: Message;
};

using EmojisFromMessageProps = {
  message: Message;
  exportUtils: ExportUtils;
};

using ProcessMessagesProps = std::vector<{
  messages: Message>;
  paths: { media: string };
  exportUtils: ExportUtils;
};

using ExportHtmlProps = {
  exportUtils: ExportUtils;
  messages: Message[];
  entityMainDirectory: string;
  entityName: string;
  currentPage: number;
};

using ExportJsonProps = ExportHtmlProps;

using CompressMessagesProps = std::vector<{
  messages: Message>;
  format: ExportType;
  entityName: string;
  entityMainDirectory: string;
  exportUtils: ExportUtils;
  threadData?: { thread: Channel; threadNo: number; threadCount: number };
};

} // namespace elizaos
