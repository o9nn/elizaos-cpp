#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_RELATIONSHIP-SEED_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___INTEGRATION_SEED_RELATIONSHIP-SEED_H
#include "core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "@elizaos/core.h"

extern std::any relationshipTestAgentId;
extern std::any relationshipTestSourceEntityId;
extern std::any relationshipTestTargetEntityId;
extern std::shared_ptr<Agent> relationshipTestAgent;
extern std::shared_ptr<Entity> relationshipTestSourceEntity;
extern std::shared_ptr<Entity> relationshipTestTargetEntity;
extern array<std::shared_ptr<Relationship>> relationshipTestRelationships;
extern std::function<std::any(std::any, std::any, array<string>, std::shared_ptr<Record<std::string, any>>)> createTestRelationship;
#endif
