#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types/role-tags.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/topics/permissions#role-object

class Role {
public:
    Role(std::optional<std::any> opts);

private:
    Snowflake id_;
    std::string name_;
    double color_;
    bool hoist_;
    double position_;
    std::string permissions_;
    bool managed_;
    bool mentionable_;
    double flags_;
};


} // namespace elizaos
