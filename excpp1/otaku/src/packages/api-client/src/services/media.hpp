#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".lib/base-client.hpp"
#include ".types/media.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class MediaService {
public:
    std::future<MediaUploadResponse> uploadAgentMedia(UUID agentId, MediaUploadParams params);
    std::future<ChannelUploadResponse> uploadChannelMedia(UUID channelId, File file);
};


} // namespace elizaos
