#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_TESTS_INDEX_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_TESTS_INDEX_TEST_H
#include "core.h"
#include "bun:test.h"
#include "../index.h"
#include "@elizaos/core.h"

class ServiceClass;

class ServiceClass : public object, public std::enable_shared_from_this<ServiceClass> {
public:
    using std::enable_shared_from_this<ServiceClass>::shared_from_this;
    any serviceType;

    (Args... args_);
};

#endif
