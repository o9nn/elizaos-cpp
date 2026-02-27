#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MediaUploadParams {
    std::variant<File, std::vector<uint8_t>> file;
    std::optional<std:> filename;
    std::optional<std:> contentType;
};

struct MediaUploadResponse {
    std: url;
    std: type;
    std: filename;
    std: originalName;
    double size;
};

struct ChannelUploadResponse {
    std: url;
    std: type;
    std: filename;
    std: originalName;
    double size;
};


} // namespace elizaos
