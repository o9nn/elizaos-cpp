#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_SERVICES_DBCOMPATIBILITY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_SERVICES_DBCOMPATIBILITY_H
#include "core.h"
#include "@elizaos/core.h"

class DatabaseCompatibilityService;

class DatabaseCompatibilityService : public object, public std::enable_shared_from_this<DatabaseCompatibilityService> {
public:
    using std::enable_shared_from_this<DatabaseCompatibilityService>::shared_from_this;
    any databaseType = std::string("unknown");

    DatabaseCompatibilityService();
    virtual void detectDatabaseType();
    template <typename RET>
    RET formatUuid(string uuid);
    template <typename RET>
    RET formatBoolean(boolean value);
    virtual boolean parseBoolean(any value);
    template <typename P0>
    string formatDate(P0 date);
    virtual any parseDate(any value);
    template <typename RET>
    RET formatJson(any obj);
    virtual any parseJson(any value);
    template <typename RET>
    RET formatArray(array<string> arr);
    virtual array<string> parseArray(any value);
    virtual string buildCaseInsensitiveSearch(string column, string value);
    template <typename RET>
    RET getDatabaseType();
};

extern std::shared_ptr<DatabaseCompatibilityService> dbCompat;
template <typename RET>
RET DatabaseCompatibilityService::formatUuid(string uuid)
{
    if (this->databaseType == std::string("postgres")) {
        return uuid;
    }
    return uuid->toLowerCase();
}

template <typename RET>
RET DatabaseCompatibilityService::formatBoolean(boolean value)
{
    if (this->databaseType == std::string("sqlite")) {
        return (value) ? 1 : 0;
    }
    return value;
}

template <typename P0>
string DatabaseCompatibilityService::formatDate(P0 date)
{
    if (type_of(date) == std::string("string")) {
        date = std::make_shared<Date>(date);
    }
    if (this->databaseType == std::string("sqlite")) {
        return date->toISOString();
    }
    return date->toISOString();
}

template <typename RET>
RET DatabaseCompatibilityService::formatJson(any obj)
{
    if (this->databaseType == std::string("sqlite")) {
        return JSON->stringify(obj);
    }
    return obj;
}

template <typename RET>
RET DatabaseCompatibilityService::formatArray(array<string> arr)
{
    if (this->databaseType == std::string("sqlite")) {
        return JSON->stringify(arr);
    }
    return arr;
}

template <typename RET>
RET DatabaseCompatibilityService::getDatabaseType()
{
    return this->databaseType;
}

#endif
