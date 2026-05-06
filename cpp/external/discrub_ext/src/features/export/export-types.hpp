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



using ExportState = {

using ExportMap = {
  /** Cleared at the end of an **/
  /*************************************/

  /** Cleared when messages are reset **/
  /*************************************/

using ExportReaction = {

/**
 * This is a 'Message Id & emoji -> Export Reaction List' map.
 */
using ExportReactionMap = std::vector<{

/**
 * This is a 'User Id -> User Information' map.
 */
using ExportUserMap = std::variant<{

/**
 * This is an 'Emoji Id -> Local File Path' map.
 */
using ExportEmojiMap = {

/**
 * This is an 'User Id and Avatar -> Local File Path' map.
 * @example idAndAvatar = "1234567/s3oma03mdsm" where "1234567" is a User Id and "s3oma03mdsm" is an Avatar
 */
using ExportAvatarMap = {

/**
 * This is a 'Remote URL -> Local File Path' map.
 */
using ExportMediaMap = {

/**
 * This is a 'Remote URL -> Local File Path' map.
 */
using ExportRoleMap = {

using SpecialFormatting = std::vector<{

using UserMentionRef = { raw: std::string; userName: std::string; id: Snowflake };
using ChannelRef = std::variant<{ channelId: Snowflake, Maybe>; raw: std::string };
using UnderlineRef = { text: std::string; raw: std::string };
using CodeRef = { text: std::string; raw: std::string };
using ItalicRef = { text: std::string; raw: std::string };
using BoldRef = { text: std::string; raw: std::string };
using LinkRef = {
using QuoteRef = { text: std::string; raw: std::string };
using HyperlinkRef = { raw: std::string };
using EmojiRef = { raw: std::string; name: std::string; id: Snowflake };

using FilesFromMessagesProps = {

using AvatarFromMessageProps = {

using GetEmojiProps = {

using FormattedInnerHtmlProps = {

using EmojisFromMessageProps = {

using ProcessMessagesProps = std::vector<{

using ExportHtmlProps = {

using ExportJsonProps = ExportHtmlProps;

using CompressMessagesProps = std::vector<{

} // namespace elizaos
