#include "tables.hpp"

array<std::shared_ptr<TableSchema>> GOALS_TABLES = array<std::shared_ptr<TableSchema>>{ object{
    object::pair{std:("name"), std:("goals")}, 
    object::pair{std:("pluginName"), std:("@elizaos/plugin-goals")}, 
    object::pair{std:("sql"), std:("CREATE TABLE IF NOT EXISTS "goals" (\
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
    object::pair{std:("fallbackSql"), std:("CREATE TABLE IF NOT EXISTS goals (\
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
    object::pair{std:("name"), std:("goal_tags")}, 
    object::pair{std:("pluginName"), std:("@elizaos/plugin-goals")}, 
    object::pair{std:("dependencies"), array<string>{ std:("goals") }}, 
    object::pair{std:("sql"), std:("CREATE TABLE IF NOT EXISTS "goal_tags" (\
      "id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),\
      "goal_id" UUID NOT NULL REFERENCES goals(id) ON DELETE CASCADE,\
      "tag" TEXT NOT NULL,\
      "created_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      UNIQUE(goal_id, tag)\
    )")}, 
    object::pair{std:("fallbackSql"), std:("CREATE TABLE IF NOT EXISTS goal_tags (\
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
