#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_TESTS_TODODATASERVICE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_TESTS_TODODATASERVICE_TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "bun:test.h"
#include "./test-utils.h"
#include "../schema.ts.h"
#include "../services/todoService.ts.h"

template <typename T>
using ResolveFn = std::function<void(T)>;

typedef std::function<void(std::shared_ptr<Error>)> RejectFn;

class MockThenable;
class MockDb;

class MockThenable : public object, public std::enable_shared_from_this<MockThenable> {
public:
    using std::enable_shared_from_this<MockThenable>::shared_from_this;
    ReturnType<mock> from;

    ReturnType<mock> where;

    ReturnType<mock> orderBy;

    ReturnType<mock> limit;

    ReturnType<mock> returning;

    ReturnType<mock> values;

    ReturnType<mock> set;

    ReturnType<mock> then;

    ReturnType<mock> execute;

    ReturnType<mock> findFirst;

    ReturnType<mock> all;

    ReturnType<mock> $dynamic;
};

class MockDb : public object, public std::enable_shared_from_this<MockDb> {
public:
    using std::enable_shared_from_this<MockDb>::shared_from_this;
    ReturnType<mock> insert;

    ReturnType<mock> select;

    ReturnType<mock> update;

    ReturnType<mock> delete;

    ReturnType<mock> execute;
};

#endif
