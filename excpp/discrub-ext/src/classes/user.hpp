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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/user#user-object
class User {
public:
    User(std::optional<std::any> opts);

private:
    Snowflake id_;
    std::string username_;
    std::string discriminator_;
    std::variant<std::string, Maybe> global_name_;
    std::variant<std::string, Maybe> avatar_;
};


} // namespace elizaos
