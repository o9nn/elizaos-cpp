#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MediaUploadParams {
    std::variant<File, Blob> file;
    std::optional<std::string> filename;
    std::optional<std::string> contentType;
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
