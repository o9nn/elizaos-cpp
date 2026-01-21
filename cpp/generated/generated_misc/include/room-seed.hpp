#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_ROOM-SEED_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_ROOM-SEED_H
#include "core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "@elizaos/core.h"

extern std::any roomTestAgentId;
extern std::any roomTestEntityId;
extern std::any roomTestWorldId;
extern std::any roomTestRoomId;
extern std::any roomTestRoom2Id;
extern std::shared_ptr<Agent> roomTestAgent;
extern std::shared_ptr<Entity> roomTestEntity;
extern std::shared_ptr<World> roomTestWorld;
extern array<std::shared_ptr<Room>> roomTestRooms;
extern std::function<std::any(std::any)> createModifiedRoom;
#endif
