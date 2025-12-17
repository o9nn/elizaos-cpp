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

enum DiscrubSetting {
  REACTIONS_ENABLED = "reactionsEnabled",
  SERVER_NICKNAME_LOOKUP = "serverNickNameLookup",
  DISPLAY_NAME_LOOKUP = "displayNameLookup",
  RANDOM_DELETE_DELAY = "randomDeleteDelay",
  RANDOM_SEARCH_DELAY = "randomSearchDelay",

  EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS = "exportSeparateThreadAndForumPosts",
  EXPORT_ARTIST_MODE = "exportUseArtistMode",
  EXPORT_MESSAGE_SORT_ORDER = "exportMessageSortOrder",
  EXPORT_PREVIEW_MEDIA = "exportPreviewMedia_2",
  EXPORT_DOWNLOAD_MEDIA = "exportDownloadMedia_2",
  EXPORT_MESSAGES_PER_PAGE = "exportMessagesPerPage",
  EXPORT_IMAGE_RES_MODE = "exportImageResMode",
}

} // namespace elizaos
