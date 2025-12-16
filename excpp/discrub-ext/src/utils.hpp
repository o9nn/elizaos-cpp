#include "app/guards.hpp"
#include "classes/attachment.hpp"
#include "classes/channel.hpp"
#include "classes/embed.hpp"
#include "classes/emoji.hpp"
#include "classes/guild.hpp"
#include "classes/message.hpp"
#include "classes/role.hpp"
#include "components/reaction-list-item-button.hpp"
#include "enum/channel-type.hpp"
#include "enum/embed-type.hpp"
#include "enum/message-regex.hpp"
#include "enum/message-type.hpp"
#include "features/export/export-types.hpp"
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

/**
 *
 * @param a Compare from
 * @param b Compare to
 * @param property Object property
 * @param direction Possible directions: 'desc' or 'asc'. (Default = 'asc')
 */
const sortByProperty = <T>(
  a: T,
  b: T,
  property: string,
  direction = "asc"
) => {
  const aVal = a[property as keyof T];
  const bVal = b[property as keyof T];
  return aVal < bVal
    ? direction === "asc"
      ? -1
      : 1
    : aVal > bVal
    ? direction === "asc"
      ? 1
      : -1
    : 0;
};

/**
 *
 * @param seconds Number of seconds to wait for
 * @param callback Optional ) => {
  await new Promise((resolve) => setTimeout(resolve, seconds * 1000));
  return callback();
};

/**
 *
 * @param date Date object to retrieve timezone from
 * @returns Timezone as String
 */
const getTimeZone = (date = new Date()) => {
  return date
    .toLocaleTimeString(undefined, { timeZoneName: "short" })
    .split(" ")[2];
};

/**
 *
 * @param index The index to check the percentage of
 * @param total The total number that to check the percentage from
 * @returns Percent value from `index` of `total`
 */
const getPercent = (index: number, total: number): string => {
  if (index === 0 && total === 0) return "0";
  return ((index / total) * 100).toString().split(".")[0];
};

/**
 *
 * @param arr
 * @returns The joined strings properly punctuated
 */
const punctuateStringArr = (arr: String[]) => {
  let str = "";
  arr.forEach((s) => {
    str = `${str}${
      str.length ? `${arr[arr.length - 1] === s ? " and " : ", "}` : ""
    }${s}`;
  });
  return str;
};

/**
 *
 * @param color Integer representation of hexadecimal color code
 * @returns Hexadecimal color code
 */
const colorToHex = (color: number | Maybe): string => {
  if (!color) {
    return "#FFF";
  }

  return `#${color.toString(16)}`;
};

/**
 *
 * @param name The value to strip unsafe characters from
 * @returns OS safe version of `name`
 */
const getSafeExportName = (name: string) => {
  const matchedWindowsCharacters =
    name.match(MessageRegex.WINDOWS_INVALID_CHARACTERS) || [];
  let retStr = name;
  matchedWindowsCharacters.forEach((char) => {
    retStr = retStr.replaceAll(char, "");
  });
  return retStr;
};

struct FormatUserData {
    std::optional<string | Maybe> userId;
    std::optional<string | Maybe> userName;
    std::optional<string | Maybe> displayName;
    std::optional<string | Maybe> guildNickname;
    std::optional<string | Maybe> joinedAt;
    std::optional<std::vector<std::string>> roleNames;
};


/**
 *
 * @param userId
 * @param userName
 * @param displayName
 * @param guildNickName
 * @param joinedAt,
 * @param roleNames Array of Role names as Strings
 * @returns String formatted User data, to be used as HTML element title prop value
 */
const formatUserData = ({
  userId,
  userName,
  displayName,
  guildNickname,
  joinedAt,
  roleNames = [],
}: FormatUserData) => {
  return `${userName ? `Username: ${userName}\n` : ""}${
    displayName ? `Display Name: ${displayName}\n` : ""
  }${guildNickname ? `Server Nickname: ${guildNickname}\n` : ""}${
    userId ? `User ID: ${userId}` : ""
  }${joinedAt ? `\nJoined Server: ${joinedAt}` : ""}${
    roleNames.length ? `\n\nRoles: ${roleNames.join(", ")}` : ""
  }`;
};

const getRichEmbeds = (message: Message): Embed[] => {
  return message.embeds.filter((embed) => embed.type === EmbedType.RICH);
};

const getExportFileName = (
  entity: Role | Attachment | Embed,
  type: string
) => {
  if (isRole(entity)) {
    return `${getSafeExportName(entity.name)}_${entity.id}.${type}`;
  } else if (isAttachment(entity)) {
    return `${getSafeExportName(entity.filename)}.${uuidv4()}.${type}`;
  } else {
    const name = entity.title ? `${entity.title}_` : "";
    return `${getSafeExportName(name)}.${uuidv4()}.${type}`;
  }
};

const getColor = (value: number): string => {
  return colorToHex(value);
};

const getIconUrl = (entity: Channel | Guild) => {
  if (isGuild(entity)) {
    if (!entity.icon) {
      return "resources/media/default_group_chat_icon.png";
    }
    return `https://cdn.discordapp.com/icons/${entity.id}/${entity.icon}`;
  } else {
    if (entity.type === ChannelType.GROUP_DM) {
      return "resources/media/default_group_chat_icon.png";
    }

    if (entity.type === ChannelType.DM && entity.recipients?.[0]?.avatar) {
      return resolveAvatarUrl(
        entity.recipients[0].id,
        entity.recipients[0].avatar
      ).remote;
    }

    return "resources/media/default_dm_icon.png";
  }
};

const entityIsImage = (entity: Attachment | Embed) => {
  if (isAttachment(entity)) {
    return Boolean(
      entity.content_type?.includes("image") ||
        ["png", "jpg", "jpeg", "gif"].some((sit) =>
          entity.filename.includes(sit)
        )
    );
  } else {
    return [EmbedType.IMAGE, EmbedType.RICH].some(
      (type) => type === entity.type
    );
  }
};

const entityIsVideo = (entity: Attachment | Embed) => {
  if (isAttachment(entity)) {
    return Boolean(entity.content_type?.includes("video"));
  } else {
    // TODO: Look into supporting embedded video.
    // GIFV may actually need to be moved to entityIsImage...
    return [EmbedType.GIFV].some((type) => type === entity.type);
  }
};

const entityIsAudio = (entity: Attachment | Embed) => {
  if (isAttachment(entity)) {
    return Boolean(
      entity.content_type?.includes("audio") ||
        ["ogg"].some((sit) => entity.filename.includes(sit))
    );
  }
  // TODO: Look into supporting embedded audio.
  return false;
};

const entityContainsMedia = (entity: Attachment | Embed) => {
  return (
    entityIsImage(entity) || entityIsVideo(entity) || entityIsAudio(entity)
  );
};

const getMediaUrls = (entity: Attachment | Embed) => {
  if (isAttachment(entity)) {
    return [entity.proxy_url].filter(Boolean);
  } else {
    if (entity.type === EmbedType.GIFV) {
      const url = entity.video?.proxy_url;
      if (url) return [url];
    }
    if (entity.type === EmbedType.IMAGE) {
      const url = entity.thumbnail?.proxy_url;
      if (url) return [url];
    }
    if (entity.type === EmbedType.RICH) {
      const retArr: string[] = [];
      [
        entity.author?.proxy_icon_url,
        entity.image?.proxy_url,
        entity.thumbnail?.proxy_url,
        entity.footer?.proxy_icon_url,
      ].forEach((url) => {
        if (url) retArr.push(url);
      });
      return retArr;
    }
    return [];
  }
};

const isDm = (channel: Channel) => {
  return [ChannelType.DM, ChannelType.GROUP_DM].some(
    (type) => type === channel.type
  );
};

using HighestRole = std::variant<{
  colorRole: Role, Maybe>;
  iconRole: Role | Maybe;
};

/**
 *
 * @param {Array} roleIds Array of String roleIds
 * @returns An object containing Role entities for the highest position color and icon
 */
const getHighestRoles = (
  roleIds: string[] = [],
  guild: Guild
): HighestRole | Maybe => {
  if (!guild.roles || !roleIds) {
    return null;
  }

  const applicableRoles = _getApplicableRoles(roleIds, guild);

  const colorRole =
    _orderRoles(applicableRoles.filter((role) => Boolean(role.color)))?.[0] ||
    null;

  const iconRole =
    _orderRoles(
      applicableRoles.filter(
        (role) => !!resolveRoleUrl(role.id, role.icon).remote
      )
    )?.[0] || null;

  return { colorRole, iconRole };
};

const getRoleNames = (
  roleIds: string[] = [],
  guild: Guild
): string[] => {
  const applicableRoles = _getApplicableRoles(roleIds, guild);

  return _orderRoles(applicableRoles).map((role) => role.name);
};

/**
 *
 * @param {Array} roles Array of Roles to be ordered
 * @returns An ordered array of Roles, descending by position
 */
const _orderRoles = (roles: Role[] = []): Role[] => {
  return roles
    .map((role) => new Role({ ...role }))
    .sort((a, b) => sortByProperty(a, b, "position", "desc"));
};

const _getApplicableRoles = (roleIds: string[] = [], guild: Guild): Role[] => {
  return (
    guild.roles?.filter(
      (role) => roleIds.some((id) => id === role.id) && Boolean(role.position)
    ) || []
  );
};

const getEncodedEmoji = (emoji: Emoji): string | null => {
  const { name, id } = emoji;
  const emojiString = id ? `${name}:${id}` : name;
  return emojiString || null;
};

const isGuildForum = (channel: Channel | Maybe) => {
  return !!(
    channel &&
    [ChannelType.GUILD_FORUM, ChannelType.GUILD_MEDIA].some(
      (type) => type === channel.type
    )
  );
};

using ResolvedFilePathObject = std::variant<{
  local: string, std::nullopt>;
  remote: string | undefined;
};

const resolveRoleUrl = (
  roleId: Snowflake,
  roleIcon: string | Maybe,
  roleMap?: ExportRoleMap | Maybe
): ResolvedFilePathObject => {
  const remoteFilePath =
    roleId && roleIcon
      ? `https://cdn.discordapp.com/role-icons/${roleId}/${roleIcon}`
      : undefined;

  let localFilePath = remoteFilePath
    ? roleMap?.[remoteFilePath] || undefined
    : undefined;

  if (localFilePath) {
    localFilePath = `../${localFilePath}`;
  }

  return {
    remote: remoteFilePath,
    local: localFilePath,
  };
};

const resolveEmojiUrl = (
  emojiId: Snowflake | Maybe,
  emojiMap?: ExportEmojiMap | Maybe
): ResolvedFilePathObject => {
  let localFilePath = emojiId ? emojiMap?.[emojiId] || undefined : undefined;
  if (localFilePath) {
    localFilePath = `../${localFilePath}`;
  }

  return {
    remote: `https://cdn.discordapp.com/emojis/${emojiId}`,
    local: localFilePath,
  };
};

const resolveAvatarUrl = (
  userId: Snowflake,
  avatar: string | Maybe,
  avatarMap?: ExportAvatarMap
): ResolvedFilePathObject => {
  const idAndAvatar = `${userId}/${avatar}`;
  let localFilePath = avatarMap?.[idAndAvatar] || undefined;
  if (localFilePath) {
    localFilePath = `../${localFilePath}`;
  }

  return {
    remote: avatar
      ? `https://cdn.discordapp.com/avatars/${idAndAvatar}`
      : "resources/media/default_avatar.png",
    local: localFilePath,
  };
};

const stringToBool = (str: string): boolean =>
  str.toLowerCase() === "true";

const boolToString = (b: boolean): string =>
  b === true ? "true" : "false";

const stringToTypedArray = <T>(str: string): T[] => {
  return str ? str.split(",").map((s) => s as T) : [];
};

const getReactingUsers = (
  exportReactions: ExportReaction[],
  userMap: ExportUserMap,
  selectedGuild: Guild | Maybe
): ReactingUser[] => {
  return exportReactions
    .filter(({ id: userId }) => userMap[userId])
    .map(({ id: userId, burst }) => {
      const mapping = userMap[userId];
      const guildNickName = selectedGuild
        ? mapping?.guilds?.[selectedGuild.id]?.nick
        : null;

      return {
        displayName: guildNickName || mapping.displayName,
        userName: mapping.userName,
        id: userId,
        avatar: mapping.avatar,
        burst,
      };
    });
};

const isThreadMessage = (message?: Message, threads: Channel[] = []) => {
  return !!message?.thread || threads.some((t) => t.id === message?.channel_id);
};

const isNonStandardMessage = (message: Message) => {
  const nonStandardTypes = [
    MessageType.CALL,
    MessageType.CHANNEL_PINNED_MESSAGE,
  ];
  return nonStandardTypes.some((v) => messageTypeEquals(message.type, v));
};

const messageTypeEquals = (type: number, compareType: MessageType) => {
  return `${type}` === compareType;
};

const isRemovableMessage = (message: Message): boolean => {
  return ![
    MessageType.RECIPIENT_ADD,
    MessageType.RECIPIENT_REMOVE,
    MessageType.CALL,
    MessageType.CHANNEL_NAME_CHANGE,
    MessageType.CHANNEL_ICON_CHANGE,
    MessageType.THREAD_STARTER_MESSAGE,
  ].some((t) => messageTypeEquals(message.type, t));
};

} // namespace elizaos
