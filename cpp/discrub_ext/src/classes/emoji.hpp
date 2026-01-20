#include "user.hpp"
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

// https://discord.com/developers/docs/resources/emoji#emoji-object

class Emoji {
  id: Snowflake | Maybe;
  name: string | Maybe;
  roles?: Snowflake[];
  user?: User;
  require_colons?: boolean;
  managed?: boolean;
  animated?: boolean;
  available?: boolean;

  constructor(opts: {
    id: Snowflake | Maybe;
    name: string | Maybe;
    roles?: Snowflake[];
    user?: User;
    require_colons?: boolean;
    managed?: boolean;
    animated?: boolean;
    available?: boolean;
  }) {
    this.id = opts.id;
    this.name = opts.name;
    this.roles = opts.roles;
    this.user = opts.user;
    this.require_colons = opts.require_colons;
    this.managed = opts.managed;
    this.animated = opts.animated;
    this.available = opts.available;
  }

} // namespace elizaos
