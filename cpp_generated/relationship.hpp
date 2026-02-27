#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC___TESTS___RELATIONSHIP_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC___TESTS___RELATIONSHIP_TEST_H
#include "core.h"
#include "vitest.h"
#include "../relationship.h"
#include "../types.h"

class RelationshipV2;

extern std::function<any(double)> createTestUUID;
class RelationshipV2 : public object, public std::enable_shared_from_this<RelationshipV2> {
public:
    using std::enable_shared_from_this<RelationshipV2>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> sourceEntityId;

    std::shared_ptr<UUID> targetEntityId;

    std::shared_ptr<UUID> agentId;

    array<string> tags;

    object metadata;

    string createdAt;
};

#endif
