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
#include <variant>
#include <vector>
#include "user.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/emoji#emoji-object

class Emoji {
public:
    Emoji(std::optional<std::any> opts);

private:
    std::variant<Snowflake, Maybe> id_;
    std::variant<std::string, Maybe> name_;
};


} // namespace elizaos
