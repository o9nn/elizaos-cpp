#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "lib/base-client.hpp"
#include "types/media.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class MediaService {
public:
    std::future<MediaUploadResponse> uploadAgentMedia(UUID agentId, MediaUploadParams params);
    std::future<ChannelUploadResponse> uploadChannelMedia(UUID channelId, File file);
};


} // namespace elizaos
