#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_EMBEDDING-SEED_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_EMBEDDING-SEED_H
#include "core.h"
#include "@elizaos/core.h"

class TestMemory;

extern std::function<any(double)> fixedUuid;
extern any embeddingTestAgentId;
extern any embeddingTestRoomId;
extern any embeddingTestEntityId;
extern any embeddingTestWorldId;
extern std::function<array<double>(double)> generateRandomVector;
extern any embeddingTestAgent;
extern std::shared_ptr<Entity> embeddingTestEntity;
extern std::shared_ptr<Room> embeddingTestRoom;
class TestMemory : public Memory, public std::enable_shared_from_this<TestMemory> {
public:
    using std::enable_shared_from_this<TestMemory>::shared_from_this;
    string type;
};

extern array<std::shared_ptr<TestMemory>> embeddingTestMemories;
extern array<any> embeddingTestData;
extern array<any> embeddingTestMemoriesWithEmbedding;
#endif
