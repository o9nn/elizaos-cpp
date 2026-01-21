#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_DATABASE_TABLES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_DATABASE_TABLES_H
#include "core.h"

class TableSchema;
class TaskMetadata;

class TableSchema : public object, public std::enable_shared_from_this<TableSchema> {
public:
    using std::enable_shared_from_this<TableSchema>::shared_from_this;
    std::string name;

    std::string pluginName;

    std::string sql;

    std::string fallbackSql;

    array<string> dependencies;
};

extern array<std::shared_ptr<TableSchema>> GOALS_TABLES;
class TaskMetadata : public object, public std::enable_shared_from_this<TaskMetadata> {
public:
    using std::enable_shared_from_this<TaskMetadata>::shared_from_this;
    std::string dueDate;

    double streak;

    boolean completedToday;

    std::string lastReminderSent;

    double pointsAwarded;

    std::string completedAt;
};

#endif
