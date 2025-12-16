#include ".classes/user.hpp"
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

// https://discord.com/developers/docs/resources/sticker#sticker-object
;

using StickerObject = {
  id: Snowflake;
  pack_id?: Snowflake;
  name: string;
  description: string | Maybe;
  tags: string;
  asset?: string;
  type: number;
  format_type: number;
  available?: boolean;
  guild_id?: Snowflake;
  user?: User;
  sort_value?: number;
};

} // namespace elizaos
