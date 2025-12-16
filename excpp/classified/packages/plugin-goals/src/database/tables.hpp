#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Define our own TableSchema type to avoid import issues during build
struct TableSchema {
    std::string name;
    std::string pluginName;
    std::string sql;
    std::optional<std::string> fallbackSql;
    std::optional<std::vector<std::string>> dependencies;
};


/**
 * Goals plugin table definitions for the unified migration system
 */
const GOALS_TABLES: TableSchema[] = [
  {
    name: 'goals',
    pluginName: '@elizaos/plugin-goals',
    sql: `CREATE TABLE IF NOT EXISTS "goals" (
      "id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
      "agent_id" UUID NOT NULL,
      "owner_type" TEXT NOT NULL,
      "owner_id" UUID NOT NULL,
      "name" TEXT NOT NULL,
      "description" TEXT,
      "is_completed" BOOLEAN DEFAULT false,
      "completed_at" TIMESTAMP,
      "created_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
      "updated_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
      "metadata" JSONB DEFAULT '{}' NOT NULL
    )`,
    fallbackSql: `CREATE TABLE IF NOT EXISTS goals (
      id TEXT PRIMARY KEY,
      agent_id TEXT NOT NULL,
      owner_type TEXT NOT NULL,
      owner_id TEXT NOT NULL,
      name TEXT NOT NULL,
      description TEXT,
      is_completed BOOLEAN DEFAULT false,
      completed_at TIMESTAMP,
      created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
      updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
      metadata TEXT DEFAULT '{}' NOT NULL
    )`,
  },

  {
    name: 'goal_tags',
    pluginName: '@elizaos/plugin-goals',
    dependencies: ['goals'],
    sql: `CREATE TABLE IF NOT EXISTS "goal_tags" (
      "id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
      "goal_id" UUID NOT NULL REFERENCES goals(id) ON DELETE CASCADE,
      "tag" TEXT NOT NULL,
      "created_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
      UNIQUE(goal_id, tag)
    )`,
    fallbackSql: `CREATE TABLE IF NOT EXISTS goal_tags (
      id TEXT PRIMARY KEY,
      goal_id TEXT NOT NULL REFERENCES goals(id) ON DELETE CASCADE,
      tag TEXT NOT NULL,
      created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
      UNIQUE(goal_id, tag)
    )`,
  },
];

struct TaskMetadata {
    std::optional<std::string> dueDate;
    std::optional<double> streak;
    std::optional<bool> completedToday;
    std::optional<std::string> lastReminderSent;
    std::optional<double> pointsAwarded;
    std::optional<std::string> completedAt;
};


} // namespace elizaos
