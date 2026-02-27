#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-SQL_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-SQL_SRC_TYPES_H
#include "core.h"
#include "drizzle-orm/node-postgres.h"
#include "drizzle-orm/pglite.h"

typedef any DrizzleDatabase;

template <typename T>
class IDatabaseClientManager;

template <typename T>
class IDatabaseClientManager : public object, public std::enable_shared_from_this<IDatabaseClientManager<T>> {
public:
    using std::enable_shared_from_this<IDatabaseClientManager<T>>::shared_from_this;
    virtual std::shared_ptr<Promise<void>> initialize() = 0;
    virtual T getConnection() = 0;
    virtual std::shared_ptr<Promise<void>> close() = 0;
};

#endif
