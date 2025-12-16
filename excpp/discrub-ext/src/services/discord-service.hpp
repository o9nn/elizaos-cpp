#include ".classes/attachment.hpp"
#include ".classes/channel.hpp"
#include ".classes/embed.hpp"
#include ".classes/guild.hpp"
#include ".classes/message.hpp"
#include ".classes/role.hpp"
#include ".classes/user.hpp"
#include ".enum/query-string-param.hpp"
#include ".enum/reaction-type.hpp"
#include ".features/app/app-types.hpp"
#include ".features/message/message-types.hpp"
#include ".types/allowed-mention-object.hpp"
#include ".types/component-object.hpp"
#include ".types/default-reaction-object.hpp"
#include ".types/forum-tag-object.hpp"
#include ".types/guild-member-object.hpp"
#include ".types/overwrite-object.hpp"
#include ".utils.hpp"
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
;
;
;

using GuildChannelModify = {
  name?: string;
  type?: number;
  position?: number | Maybe;
  topic?: string | Maybe;
  nsfw?: boolean | Maybe;
  rate_limit_peruser?: number | Maybe;
  bitrate?: number | Maybe;
  user_limit?: number | Maybe;
  permission_overwrites?: OverwriteObject[] | Maybe;
  parent_id?: string | Maybe;
  rtc_region?: string | Maybe;
  video_quality_mode?: number | Maybe;
  default_auto_archive_duration?: number | Maybe;
  flags?: number;
  available_tags?: ForumTagObject[];
  default_reaction_emoji?: DefaultReactionObject | Maybe;
  default_thread_rate_limit_per_user?: number;
  default_sort_order?: number | Maybe;
  default_forum_layout?: number;
};

using ThreadModify = {
  name?: string;
  archived?: boolean;
  auto_archive_duration?: number;
  locked?: boolean;
  invitable?: boolean;
  rate_limit_per_user?: number | Maybe;
  flags?: number;
  applied_tags?: string[];
};

using MessageModify = {
  content?: string;
  embeds?: Embed[];
  flags?: number;
  allowed_mentions?: AllowedMentionObject;
  components?: ComponentObject;
  payload_json?: string;
  attachments?: Attachment[];
};

using SearchMessageResult = std::vector<{
  messages: Message>;
  threads: Channel[];
  total_results: number;
};

class DiscordService {
  searchDelaySecs = 0;
  deleteDelaySecs = 0;
  DISCORD_API_URL = "https://discord.com/api/v10";
  DISCORD_USERS_ENDPOINT = `${this.DISCORD_API_URL}/users`;
  DISCORD_GUILDS_ENDPOINT = `${this.DISCORD_API_URL}/guilds`;
  DISCORD_CHANNELS_ENDPOINT = `${this.DISCORD_API_URL}/channels`;
  userAgent =
    "Mozilla/5.0 (X11; Linux x86_64; rv:17.0) Gecko/20121202 Firefox/17.0 Iceweasel/17.0.1";

  constructor(settings?: AppSettings) {
    if (settings) {
      this.searchDelaySecs = Number(settings.randomSearchDelay);
      this.deleteDelaySecs = Number(settings.randomDeleteDelay);
    }
  }

  generateSnowflake = (date: Date = new Date()): string =>
    ((BigInt(date.valueOf()) - BigInt(1420070400000)) << BigInt(22)).toString();

  calculateRandomNumber = (max: number, min: number = 0) =>
    Math.random() * (max - min) + min;

  withSearchDelay = async <T = void>(
    func: () => Promise<DiscordApiResponse<T>>
  ) => {
    if (this.searchDelaySecs > 0) {
      const delay = this.calculateRandomNumber(this.searchDelaySecs);
      console.warn(`Applying Search Delay: ${delay} seconds`);
      await wait(delay);
    }

    return func();
  };

  withDeleteDelay = async <T = void>(
    func: () => Promise<DiscordApiResponse<T>>
  ) => {
    if (this.deleteDelaySecs > 0) {
      const delay = this.calculateRandomNumber(this.deleteDelaySecs);
      console.warn(`Applying Delete/Edit Delay: ${delay} seconds`);
      await wait(delay);
    }

    return func();
  };

  withRetry = async <T = void>(
    promise: () => Promise<Response>,
    isBlob: boolean = false
  ): Promise<DiscordApiResponse<T>> => {
    let apiResponse: DiscordApiResponse<T> = { success: false };
    try {
      let requestComplete = false;
      while (!requestComplete) {
        const response = await promise();
        const { status, ok } = response;
        if (ok) {
          // Request was successful
          requestComplete = true;
          if (status === 200) {
            // Successful request has data
            const data: T = isBlob
              ? await response.blob()
              : await response.json();
            apiResponse = { success: true, data: data };
          } else {
            // Successful request does not have data
            apiResponse = { success: true };
          }
        } else if (status === 429) {
          // Request must be re-attempted after x seconds
          const json = await response.json();
          await wait(json.retry_after);
        } else {
          // Request failed for unknown reason
          requestComplete = true;
          console.error("Request could not be completed", response);
        }
      }
      return apiResponse;
    } catch (e) {
      console.error("Request threw an exception", e);
      return apiResponse;
    }
  };

  getUser = (authorization: string, userId: string) =>
    this.withSearchDelay(() =>
      this.withRetry<User>(() =>
        fetch(`${this.DISCORD_USERS_ENDPOINT}/${userId}`, {
          method: "GET",
          headers: {
            "Content-Type": "application/json",
            authorization: authorization,
            "user-agent": this.userAgent,
          },
        })
      )
    );

  fetchUserData = (authorization: string) =>
    this.withRetry<User>(() =>
      fetch(`${this.DISCORD_USERS_ENDPOINT}/@me`, {
        method: "GET",
        headers: {
          "Content-Type": "application/json",
          authorization: authorization,
          "user-agent": this.userAgent,
        },
      })
    );

  fetchGuildUser = (guildId: string, userId: string, authorization: string) =>
    this.withSearchDelay(() =>
      this.withRetry<GuildMemberObject>(() =>
        fetch(`${this.DISCORD_GUILDS_ENDPOINT}/${guildId}/members/${userId}`, {
          method: "GET",
          headers: {
            "Content-Type": "application/json",
            authorization: authorization,
            "user-agent": this.userAgent,
          },
        })
      )
    );

  fetchDirectMessages = (authorization: string) =>
    this.withRetry<Channel[]>(() =>
      fetch(`${this.DISCORD_USERS_ENDPOINT}/@me/channels`, {
        method: "GET",
        headers: {
          "Content-Type": "application/json",
          authorization: authorization,
          "user-agent": this.userAgent,
        },
      })
    );

  fetchGuilds = (authorization: string) =>
    this.withRetry<Guild[]>(() =>
      fetch(`${this.DISCORD_USERS_ENDPOINT}/@me/guilds`, {
        method: "GET",
        headers: {
          "Content-Type": "application/json",
          authorization: authorization,
          "user-agent": this.userAgent,
        },
      })
    );

  fetchRoles = (guildId: string, authorization: string) =>
    this.withRetry<Role[]>(() =>
      fetch(`${this.DISCORD_GUILDS_ENDPOINT}/${guildId}/roles`, {
        method: "GET",
        headers: {
          "Content-Type": "application/json",
          authorization: authorization,
          "user-agent": this.userAgent,
        },
      })
    );

  fetchChannels = (authorization: string, guildId: string) =>
    this.withRetry<Channel[]>(() =>
      fetch(`${this.DISCORD_GUILDS_ENDPOINT}/${guildId}/channels`, {
        method: "GET",
        headers: {
          "Content-Type": "application/json",
          authorization: authorization,
          "user-agent": this.userAgent,
        },
      })
    );

  fetchChannel = (authorization: string, channelId: string) =>
    this.withSearchDelay(() =>
      this.withRetry<Channel>(() =>
        fetch(`${this.DISCORD_CHANNELS_ENDPOINT}/${channelId}`, {
          method: "GET",
          headers: {
            "Content-Type": "application/json",
            authorization: authorization,
            "user-agent": this.userAgent,
          },
        })
      )
    );

  editChannel = (
    authorization: string,
    channelId: string,
    updateObj: ThreadModify | GuildChannelModify
  ) =>
    this.withDeleteDelay(() =>
      this.withRetry<Channel>(() =>
        fetch(`${this.DISCORD_CHANNELS_ENDPOINT}/${channelId}`, {
          method: "PATCH",
          headers: {
            "Content-Type": "application/json",
            authorization: authorization,
            "user-agent": this.userAgent,
          },
          body: JSON.stringify({ ...updateObj }),
        })
      )
    );

  editMessage = (
    authorization: string,
    messageId: string,
    updateProps: MessageModify,
    channelId: string
  ) =>
    this.withDeleteDelay(() =>
      this.withRetry<Message>(() =>
        fetch(
          `${this.DISCORD_CHANNELS_ENDPOINT}/${channelId}/messages/${messageId}`,
          {
            method: "PATCH",
            headers: {
              "Content-Type": "application/json",
              authorization: authorization,
              "user-agent": this.userAgent,
            },
            body: JSON.stringify({ ...updateProps }),
          }
        )
      )
    );

  deleteMessage = (
    authorization: string,
    messageId: string,
    channelId: string
  ) =>
    this.withDeleteDelay(() =>
      this.withRetry(() =>
        fetch(
          `${this.DISCORD_CHANNELS_ENDPOINT}/${channelId}/messages/${messageId}`,
          {
            method: "DELETE",
            headers: {
              "Content-Type": "application/json",
              authorization: authorization,
              "user-agent": this.userAgent,
            },
          }
        )
      )
    );

  fetchMessageData = (
    authorization: string,
    lastId: string,
    channelId: string,
    queryParam: QueryStringParam = QueryStringParam.BEFORE
  ) =>
    this.withSearchDelay(() =>
      this.withRetry<Message[]>(() =>
        fetch(
          `${this.DISCORD_CHANNELS_ENDPOINT}/${channelId}/messages?limit=${
            queryParam === QueryStringParam.AROUND ? "50" : "100"
          }${lastId.length > 0 ? `&${queryParam}=${lastId}` : ""}`,
          {
            method: "GET",
            headers: {
              "Content-Type": "application/json",
              authorization: authorization,
              "user-agent": this.userAgent,
            },
          }
        )
      )
    );

  fetchSearchMessageData = (
    authorization: string,
    offset: number,
    channelId: string | Maybe,
    guildId: string | Maybe,
    searchCriteria: Partial<SearchMessageProps>
  ) => {
    const {
      preFilterUserId,
      searchAfterDate,
      searchBeforeDate,
      searchMessageContent,
      selectedHasTypes,
    } = searchCriteria;
    const urlSearchParams = new URLSearchParams({
      author_id: preFilterUserId || "null",
      min_id: searchAfterDate
        ? this.generateSnowflake(searchAfterDate)
        : "null",
      max_id: searchBeforeDate
        ? this.generateSnowflake(searchBeforeDate)
        : "null",
      content: searchMessageContent || "null",
      channel_id: guildId && channelId ? channelId : "null",
      include_nsfw: "true",
    });
    selectedHasTypes?.forEach((type: string) => {
      urlSearchParams.append("has", type);
    });
    const nullKeys: string[] = [];
    urlSearchParams.forEach((value, key) => {
      if (value === "null") {
        nullKeys.push(key);
      }
    });
    nullKeys.forEach((key) => {
      urlSearchParams.delete(key);
    });

    return this.withSearchDelay(() =>
      this.withRetry<SearchMessageResult>(() =>
        fetch(
          `${
            guildId
              ? this.DISCORD_GUILDS_ENDPOINT
              : this.DISCORD_CHANNELS_ENDPOINT
          }/${
            guildId || channelId
          }/messages/search?${urlSearchParams.toString()}${
            offset > 0 ? `&offset=${offset}` : ""
          }`,
          {
            method: "GET",
            headers: {
              "Content-Type": "application/json",
              authorization: authorization,
              "user-agent": this.userAgent,
            },
          }
        )
      )
    );
  };

  fetchPrivateThreads = (authorization: string, channelId: string) =>
    this.withSearchDelay(() =>
      this.withRetry<Channel[]>(() =>
        fetch(
          `${this.DISCORD_CHANNELS_ENDPOINT}/${channelId}/threads/archived/private`,
          {
            method: "GET",
            headers: {
              "Content-Type": "application/json",
              authorization: authorization,
              "user-agent": this.userAgent,
            },
          }
        )
      )
    );

  fetchPublicThreads = (authorization: string, channelId: string) =>
    this.withSearchDelay(() =>
      this.withRetry<Channel[]>(() =>
        fetch(
          `${this.DISCORD_CHANNELS_ENDPOINT}/${channelId}/threads/archived/public`,
          {
            method: "GET",
            headers: {
              "Content-Type": "application/json",
              authorization: authorization,
              "user-agent": this.userAgent,
            },
          }
        )
      )
    );

  createDm = (authorization: string, recipient_id: string) =>
    this.withRetry<unknown>(() =>
      fetch(`${this.DISCORD_USERS_ENDPOINT}/@me/channels`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          authorization: authorization,
          "user-agent": this.userAgent,
        },
        body: JSON.stringify({ recipient_id }),
      })
    );

  sendFriendRequest = (
    authorization: string,
    props: { username: string; discriminator: string }
  ) =>
    this.withRetry<unknown>(() =>
      fetch(`${this.DISCORD_USERS_ENDPOINT}/@me/relationships`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          authorization: authorization,
          "user-agent": this.userAgent,
        },
        body: JSON.stringify(props),
      })
    );

  deleteFriendRequest = (authorization: string, userId: string) =>
    this.withRetry<unknown>(() =>
      fetch(`${this.DISCORD_USERS_ENDPOINT}/@me/relationships/${userId}`, {
        method: "DELETE",
        headers: {
          "Content-Type": "application/json",
          authorization: authorization,
          "user-agent": this.userAgent,
        },
      })
    );

  getRelationships = (authorization: string) =>
    this.withRetry<unknown[]>(() =>
      fetch(`${this.DISCORD_USERS_ENDPOINT}/@me/relationships`, {
        method: "GET",
        headers: {
          "Content-Type": "application/json",
          authorization: authorization,
          "user-agent": this.userAgent,
        },
      })
    );

  downloadFile = (downloadUrl: string) =>
    this.withSearchDelay(() =>
      this.withRetry<Blob>(() => fetch(downloadUrl), true)
    );

  getReactions = (
    authorization: string,
    channelId: Snowflake,
    messageId: Snowflake,
    emoji: string,
    type: ReactionType,
    lastId?: Snowflake | null
  ) =>
    this.withSearchDelay(() =>
      this.withRetry<User[]>(() =>
        fetch(
          `${
            this.DISCORD_CHANNELS_ENDPOINT
          }/${channelId}/messages/${messageId}/reactions/${emoji}?limit=100&type=${type}${
            lastId ? `&after=${lastId}` : ""
          }`,
          {
            method: "GET",
            headers: {
              "Content-Type": "application/json",
              authorization: authorization,
              "user-agent": this.userAgent,
            },
          }
        )
      )
    );

  deleteReaction = (
    authorization: string,
    channelId: Snowflake,
    messageId: Snowflake,
    emoji: string
  ) =>
    this.withDeleteDelay(() =>
      this.withRetry(() =>
        fetch(
          `${this.DISCORD_CHANNELS_ENDPOINT}/${channelId}/messages/${messageId}/reactions/${emoji}/@me`,
          {
            method: "DELETE",
            headers: {
              "Content-Type": "application/json",
              authorization: authorization,
              "user-agent": this.userAgent,
            },
          }
        )
      )
    );
}

default DiscordService;

} // namespace elizaos
