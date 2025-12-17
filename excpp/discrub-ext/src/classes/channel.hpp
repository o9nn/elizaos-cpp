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
#include "types/default-reaction-object.hpp"
#include "types/forum-tag-object.hpp"
#include "types/overwrite-object.hpp"
#include "types/thread-member-object.hpp"
#include "types/thread-metadata.hpp"
#include "user.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/channel#channel-object

class Channel {
public:
    Channel(std::optional<std::any> opts);

private:
    Snowflake id_;
    double type_;
};


} // namespace elizaos
