#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Interface representing an attachment.
 *
 * @interface
 * @property {string} url - The URL of the attachment.
 * @property {string} contentType - The content type of the attachment.
 * @property {string} title - The title of the attachment.
 */
struct IAttachment {
    std::string url;
    std::string contentType;
    std::string title;
};


} // namespace elizaos
