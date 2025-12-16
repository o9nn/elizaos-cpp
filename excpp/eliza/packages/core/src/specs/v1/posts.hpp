#include ".v2.hpp"
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

;
import type { Actor, Memory } from './types';

const formatPosts = ({
  messages,
  actors,
  conversationHeader = true,
}: {
  messages: Memory[];
  actors: Actor[];
  conversationHeader?: boolean;
}) => {
  const entities = actors.map((actor) => ({
    id: actor.id,
    names: [actor.name, actor.username].filter(Boolean) as string[],
    metadata: actor.details,
    agentId: undefined as any, // agentId is not available on v1 Actor and not used by coreFormatPosts
  }));
  return coreFormatPosts({
    messages: messages as any,
    entities: entities as any,
    conversationHeader,
  });
};

} // namespace elizaos
