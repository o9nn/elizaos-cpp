#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "enum/embed-type.hpp"
#include "types/embed-author-object.hpp"
#include "types/embed-field-object.hpp"
#include "types/embed-footer-object.hpp"
#include "types/embed-image-object.hpp"
#include "types/embed-provider-object.hpp"
#include "types/embed-thumbnail-object.hpp"
#include "types/embed-video-object.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/channel#embed-object

class Embed {
public:
    Embed(std::optional<std::any> opts);
};


} // namespace elizaos
