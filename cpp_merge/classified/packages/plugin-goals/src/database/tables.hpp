#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_GOALS_SRC_DATABASE_TABLES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_GOALS_SRC_DATABASE_TABLES_H
#include "core.hpp"

class TableSchema;
class TaskMetadata;

class TableSchema : public object, public std::enable_shared_from_this<TableSchema> {
public:
    using std::enable_shared_from_this<TableSchema>::shared_from_this;
    string name;

    string pluginName;

    string sql;

    string fallbackSql;

    array<string> dependencies;
};

extern array<std::shared_ptr<TableSchema>> GOALS_TABLES;
class TaskMetadata : public object, public std::enable_shared_from_this<TaskMetadata> {
public:
    using std::enable_shared_from_this<TaskMetadata>::shared_from_this;
    string dueDate;

    double streak;

    boolean completedToday;

    string lastReminderSent;

    double pointsAwarded;

    string completedAt;
};

#endif
