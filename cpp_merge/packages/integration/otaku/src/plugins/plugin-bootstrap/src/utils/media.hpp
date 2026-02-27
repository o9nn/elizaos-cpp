#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_BOOTSTRAP_SRC_UTILS_MEDIA_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_BOOTSTRAP_SRC_UTILS_MEDIA_H
#include "core.hpp"
// External dependency removed

typedef object MediaData;


std::shared_ptr<Promise<array<MediaData>>> fetchMediaData(array<std::shared_ptr<Media>> attachments);

std::shared_ptr<Promise<array<std::shared_ptr<Media>>>> processAttachments(array<std::shared_ptr<Media>> attachments, std::shared_ptr<IAgentRuntime> runtime);

#endif
