#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC___TESTS___INTEGRATION_SEED_RELATIONSHIP_SEED_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC___TESTS___INTEGRATION_SEED_RELATIONSHIP_SEED_H
#include "core.hpp"
#include "uuid.hpp"
using uuidv4 = v4;
// External dependency removed

extern any relationshipTestAgentId;
extern any relationshipTestSourceEntityId;
extern any relationshipTestTargetEntityId;
extern std::shared_ptr<Agent> relationshipTestAgent;
extern std::shared_ptr<Entity> relationshipTestSourceEntity;
extern std::shared_ptr<Entity> relationshipTestTargetEntity;
extern array<std::shared_ptr<Relationship>> relationshipTestRelationships;
extern std::function<any(any, any, array<string>, std::shared_ptr<Record<string, any>>)> createTestRelationship;
#endif
