#include "group.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void GroupRoute() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { channelId: channelIdFromPath } = useParams<{ channelId: string }>();
    const auto [searchParams] = useSearchParams();
    const auto serverIdFromQuery = searchParams.get('serverId');

    const auto channelId = validateUuid(channelIdFromPath);
    const auto serverId = validateUuid(serverIdFromQuery || '');

    if (!channelId || !serverId) {
        return (;
        <div className="flex flex-1 justify-center items-center">;
        <p>Missing channel or server information.</p>;
        </div>;
        );
    }

    return (;
    <ChatComponent key={channelId} chatType="GROUP" contextId={channelId} serverId={serverId} />;
    );

}

} // namespace elizaos
