#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/channel#attachment-object
class Attachment {
public:
    Attachment(std::optional<std::any> opts);

private:
    Snowflake id_;
    std::string filename_;
    double size_;
    std::string url_;
    std::string proxy_url_;
};


} // namespace elizaos
