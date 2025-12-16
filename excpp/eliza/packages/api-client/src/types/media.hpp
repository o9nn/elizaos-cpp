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

;

struct MediaUploadParams {
    File | Blob file;
    std::optional<std::string> filename;
    std::optional<std::string> contentType;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


struct MediaUploadResponse {
    std::string url;
    std::string type;
    std::string filename;
    std::string originalName;
    double size;
};


struct ChannelUploadResponse {
    std::string url;
    std::string type;
    std::string filename;
    std::string originalName;
    double size;
};


} // namespace elizaos
