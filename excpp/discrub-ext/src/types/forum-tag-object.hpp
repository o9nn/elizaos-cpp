#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/channel#forum-tag-object
using ForumTagObject = {
  id: Snowflake;
  name: string;
  moderated: boolean;
  emoji_id: string | Maybe;
  emoji_name: string | Maybe;
};

} // namespace elizaos
