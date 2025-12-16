#include "elizaos/core.hpp"
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

async 
    const isDM = roomDetails.type === 'dm'
    if (isDM) {
      discordService.sendDM(message.metadata.authorId, reply)
      responses.length = 0
    } else {
      responses.length = 0
      const entityId = createUniqueUuid(runtime, message.metadata.authorId);
      responses.push({
        entityId,
        agentId: runtime.agentId,
        roomId: message.roomId,
        content: {
          text: reply,
          attachments: [],
          inReplyTo: createUniqueUuid(runtime, message.id)
        },
        // embedding
        // metadata: entityName, type, authorId
      })
    }
    return true
  }
  logger.warn('unknown platform', message.content.source)
  return false
}


    discordService.sendDM(message.metadata.authorId, reply)
    return true
  }
  logger.warn('unknown platform', message.content.source)
  return false
}

} // namespace elizaos
