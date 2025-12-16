#include ".types/reaction-count-details-object.hpp"
#include "emoji.hpp"
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

// https://discord.com/developers/docs/resources/channel#reaction-object
;
;

class Reaction {
  count: number;
  count_details: ReactionCountDetailsObject;
  me: boolean;
  me_burst: boolean;
  emoji: Emoji;
  burst_colors: string[];

  constructor(opts: {
    count: number;
    count_details: ReactionCountDetailsObject;
    me: boolean;
    me_burst: boolean;
    emoji: Emoji;
    burst_colors: string[];
  }) {
    this.count = opts.count;
    this.count_details = opts.count_details;
    this.me = opts.me;
    this.me_burst = opts.me_burst;
    this.emoji = opts.emoji;
    this.burst_colors = opts.burst_colors;
  }
}

} // namespace elizaos
