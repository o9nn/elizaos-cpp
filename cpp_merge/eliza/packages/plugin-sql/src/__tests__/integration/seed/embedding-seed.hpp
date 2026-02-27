#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC___TESTS___INTEGRATION_SEED_EMBEDDING_SEED_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC___TESTS___INTEGRATION_SEED_EMBEDDING_SEED_H
#include "core.hpp"
// External dependency removed

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
