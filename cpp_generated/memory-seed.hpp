#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_MEMORY-SEED_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_MEMORY-SEED_H
#include "core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "@elizaos/core.h"

extern any memoryTestAgentId;
extern any memoryTestEntityId;
extern any memoryTestRoomId;
extern any memoryTestWorldId;
extern std::shared_ptr<Agent> memoryTestAgent;
extern std::shared_ptr<Entity> memoryTestEntity;
extern std::shared_ptr<World> memoryTestWorld;
extern std::shared_ptr<Room> memoryTestRoom;
extern std::function<array<double>(double)> generateEmbedding;
extern array<std::shared_ptr<Memory>> memoryTestMemories;
extern array<std::shared_ptr<Memory>> memoryTestMemoriesWithEmbedding;
extern any documentMemoryId;
extern std::shared_ptr<Memory> memoryTestDocument;
extern array<std::shared_ptr<Memory>> memoryTestFragments;
extern std::function<any(any, double)> createSimilarMemoryVector;
#endif
