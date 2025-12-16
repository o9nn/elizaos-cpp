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
;
;

default  = useParams<{ channelId: string }>();
  const [searchParams] = useSearchParams();
  const serverIdFromQuery = searchParams.get('serverId');

  const channelId = validateUuid(channelIdFromPath);
  const serverId = validateUuid(serverIdFromQuery || '');

  if (!channelId || !serverId) {
    return (
      <div className="flex flex-1 justify-center items-center">
        <p>Missing channel or server information.</p>
      </div>
    );
  }

  return (
    <ChatComponent key={channelId} chatType="GROUP" contextId={channelId} serverId={serverId} />
  );
}

} // namespace elizaos
