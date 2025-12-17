#include "devfix.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> updateSpartanData(auto agentEntityId, auto spartanData) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (spartanDataNew) {
        runtime.createComponent({
            id: uuidv4(),
            agentId: runtime.agentId,
            worldId: roomDetails.worldId,
            roomId: message.roomId,
            sourceEntityId: entityId,
            entityId: agentEntityId,
            type: SPARTAN_SERVICE_TYPE,
            data: spartanData,
            });
            } else {
                runtime.updateComponent({
                    id: spartanData.id,
                    // do we need all these fields?
                    //agentId: runtime.agentId,
                    //worldId: roomDetails.worldId,
                    //roomId: message.roomId,
                    //sourceEntityId: entityId,
                    //entityId: entityId,
                    //type: SPARTAN_SERVICE_TYPE,
                    data: agentEntity.components,
                    });
                }

}

} // namespace elizaos
