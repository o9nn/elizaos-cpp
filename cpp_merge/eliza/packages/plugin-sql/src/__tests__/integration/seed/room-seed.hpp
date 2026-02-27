#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC___TESTS___INTEGRATION_SEED_ROOM_SEED_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC___TESTS___INTEGRATION_SEED_ROOM_SEED_H
#include "core.hpp"
#include "uuid.hpp"
using uuidv4 = v4;
// External dependency removed

extern any roomTestAgentId;
extern any roomTestEntityId;
extern any roomTestWorldId;
extern any roomTestRoomId;
extern any roomTestRoom2Id;
extern std::shared_ptr<Agent> roomTestAgent;
extern std::shared_ptr<Entity> roomTestEntity;
extern std::shared_ptr<World> roomTestWorld;
extern array<std::shared_ptr<Room>> roomTestRooms;
extern std::function<any(any)> createModifiedRoom;
#endif
