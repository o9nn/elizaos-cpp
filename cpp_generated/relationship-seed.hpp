#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_RELATIONSHIP-SEED_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_RELATIONSHIP-SEED_H
#include "core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "@elizaos/core.h"

extern any relationshipTestAgentId;
extern any relationshipTestSourceEntityId;
extern any relationshipTestTargetEntityId;
extern std::shared_ptr<Agent> relationshipTestAgent;
extern std::shared_ptr<Entity> relationshipTestSourceEntity;
extern std::shared_ptr<Entity> relationshipTestTargetEntity;
extern array<std::shared_ptr<Relationship>> relationshipTestRelationships;
extern std::function<any(any, any, array<string>, std::shared_ptr<Record<string, any>>)> createTestRelationship;
#endif
