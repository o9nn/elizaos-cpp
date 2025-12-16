#include ".lib/base-client.hpp"
#include ".types/media.hpp"
#include "elizaos/core.hpp"
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



class MediaService extends BaseApiClient {
  /**
   * Upload media for an agent
   */
  async uploadAgentMedia(agentId: UUID, params: MediaUploadParams): Promise<MediaUploadResponse> {
    const formData = new FormData();

    formData.append('file', params.file, params.filename);

    if (params.contentType) formData.append('contentType', params.contentType);
    if (params.metadata) formData.append('metadata', JSON.stringify(params.metadata));

    return this.request<MediaUploadResponse>('POST', `/api/media/agents/${agentId}/upload-media`, {
      body: formData,
    });
  }

  /**
   * Upload file to a channel
   */


} // namespace elizaos
