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
#include "emoji.hpp"
#include "types/reaction-count-details-object.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/channel#reaction-object

class Reaction {
public:
    Reaction(const std::any& opts);

private:
    double count_;
    ReactionCountDetailsObject count_details_;
    bool me_;
    bool me_burst_;
    Emoji emoji_;
    std::vector<std::string> burst_colors_;
};


} // namespace elizaos
