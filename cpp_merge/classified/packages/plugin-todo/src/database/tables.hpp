#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_DATABASE_TABLES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_DATABASE_TABLES_H
#include "core.hpp"

class TableSchema;

class TableSchema : public object, public std::enable_shared_from_this<TableSchema> {
public:
    using std::enable_shared_from_this<TableSchema>::shared_from_this;
    string name;

    string pluginName;

    string sql;

    array<string> dependencies;

    string fallbackSql;
};

extern array<std::shared_ptr<TableSchema>> TODO_TABLES;
#endif
