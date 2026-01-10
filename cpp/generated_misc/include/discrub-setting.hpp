#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_DISCRUB-SETTING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_DISCRUB-SETTING_H
#include "core.h"

enum struct DiscrubSetting;

enum struct DiscrubSetting {
    REACTIONS_ENABLED = std::string("reactionsEnabled"), SERVER_NICKNAME_LOOKUP = std::string("serverNickNameLookup"), DISPLAY_NAME_LOOKUP = std::string("displayNameLookup"), RANDOM_DELETE_DELAY = std::string("randomDeleteDelay"), RANDOM_SEARCH_DELAY = std::string("randomSearchDelay"), EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS = std::string("exportSeparateThreadAndForumPosts"), EXPORT_ARTIST_MODE = std::string("exportUseArtistMode"), EXPORT_MESSAGE_SORT_ORDER = std::string("exportMessageSortOrder"), EXPORT_PREVIEW_MEDIA = std::string("exportPreviewMedia_2"), EXPORT_DOWNLOAD_MEDIA = std::string("exportDownloadMedia_2"), EXPORT_MESSAGES_PER_PAGE = std::string("exportMessagesPerPage"), EXPORT_IMAGE_RES_MODE = std::string("exportImageResMode")
};
#endif
