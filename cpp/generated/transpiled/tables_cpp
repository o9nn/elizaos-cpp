#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/database/tables.h"

array<std::shared_ptr<TableSchema>> GOALS_TABLES = array<std::shared_ptr<TableSchema>>{ object{
    object::pair{std::string("name"), std::string("goals")}, 
    object::pair{std::string("pluginName"), std::string("@elizaos/plugin-goals")}, 
    object::pair{std::string("sql"), std::string("CREATE TABLE IF NOT EXISTS "goals" (\
      "id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),\
      "agent_id" UUID NOT NULL,\
      "owner_type" TEXT NOT NULL,\
      "owner_id" UUID NOT NULL,\
      "name" TEXT NOT NULL,\
      "description" TEXT,\
      "is_completed" BOOLEAN DEFAULT false,\
      "completed_at" TIMESTAMP,\
      "created_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      "updated_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      "metadata" JSONB DEFAULT '{}' NOT NULL\
    )")}, 
    object::pair{std::string("fallbackSql"), std::string("CREATE TABLE IF NOT EXISTS goals (\
      id TEXT PRIMARY KEY,\
      agent_id TEXT NOT NULL,\
      owner_type TEXT NOT NULL,\
      owner_id TEXT NOT NULL,\
      name TEXT NOT NULL,\
      description TEXT,\
      is_completed BOOLEAN DEFAULT false,\
      completed_at TIMESTAMP,\
      created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      metadata TEXT DEFAULT '{}' NOT NULL\
    )")}
}, object{
    object::pair{std::string("name"), std::string("goal_tags")}, 
    object::pair{std::string("pluginName"), std::string("@elizaos/plugin-goals")}, 
    object::pair{std::string("dependencies"), array<string>{ std::string("goals") }}, 
    object::pair{std::string("sql"), std::string("CREATE TABLE IF NOT EXISTS "goal_tags" (\
      "id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),\
      "goal_id" UUID NOT NULL REFERENCES goals(id) ON DELETE CASCADE,\
      "tag" TEXT NOT NULL,\
      "created_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      UNIQUE(goal_id, tag)\
    )")}, 
    object::pair{std::string("fallbackSql"), std::string("CREATE TABLE IF NOT EXISTS goal_tags (\
      id TEXT PRIMARY KEY,\
      goal_id TEXT NOT NULL REFERENCES goals(id) ON DELETE CASCADE,\
      tag TEXT NOT NULL,\
      created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      UNIQUE(goal_id, tag)\
    )")}
} };

void Main(void)
{
}

MAIN
