#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/database/tables.h"

array<std::shared_ptr<TableSchema>> TODO_TABLES = array<std::shared_ptr<TableSchema>>{ object{
    object::pair{std::string("name"), std::string("todos")}, 
    object::pair{std::string("pluginName"), std::string("@elizaos/plugin-todo")}, 
    object::pair{std::string("sql"), std::string("CREATE TABLE IF NOT EXISTS "todos" (\
      "id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),\
      "agent_id" UUID NOT NULL,\
      "world_id" UUID NOT NULL,\
      "room_id" UUID NOT NULL,\
      "entity_id" UUID NOT NULL,\
      "name" TEXT NOT NULL,\
      "description" TEXT,\
      "type" TEXT NOT NULL,\
      "priority" INTEGER DEFAULT 4,\
      "is_urgent" BOOLEAN DEFAULT false,\
      "is_completed" BOOLEAN DEFAULT false,\
      "due_date" TIMESTAMP,\
      "completed_at" TIMESTAMP,\
      "created_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      "updated_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      "metadata" JSONB DEFAULT '{}' NOT NULL\
    )")}, 
    object::pair{std::string("fallbackSql"), std::string("CREATE TABLE IF NOT EXISTS todos (\
      id TEXT PRIMARY KEY,\
      agent_id TEXT NOT NULL,\
      world_id TEXT NOT NULL,\
      room_id TEXT NOT NULL,\
      entity_id TEXT NOT NULL,\
      name TEXT NOT NULL,\
      description TEXT,\
      type TEXT NOT NULL,\
      priority INTEGER DEFAULT 4,\
      is_urgent BOOLEAN DEFAULT false,\
      is_completed BOOLEAN DEFAULT false,\
      due_date TIMESTAMP,\
      completed_at TIMESTAMP,\
      created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      metadata TEXT DEFAULT '{}' NOT NULL\
    )")}
}, object{
    object::pair{std::string("name"), std::string("todo_tags")}, 
    object::pair{std::string("pluginName"), std::string("@elizaos/plugin-todo")}, 
    object::pair{std::string("dependencies"), array<string>{ std::string("todos") }}, 
    object::pair{std::string("sql"), std::string("CREATE TABLE IF NOT EXISTS "todo_tags" (\
      "id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),\
      "todo_id" UUID NOT NULL REFERENCES todos(id) ON DELETE CASCADE,\
      "tag" TEXT NOT NULL,\
      "created_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      UNIQUE(todo_id, tag)\
    )")}, 
    object::pair{std::string("fallbackSql"), std::string("CREATE TABLE IF NOT EXISTS todo_tags (\
      id TEXT PRIMARY KEY,\
      todo_id TEXT NOT NULL REFERENCES todos(id) ON DELETE CASCADE,\
      tag TEXT NOT NULL,\
      created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,\
      UNIQUE(todo_id, tag)\
    )")}
} };

void Main(void)
{
}

MAIN
