#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_TESTS_INDEX_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_TESTS_INDEX_TEST_H
#include "core.hpp"
#include "bun:test.h"
#include "../index.h"
// External dependency removed

class ServiceClass;

class ServiceClass : public object, public std::enable_shared_from_this<ServiceClass> {
public:
    using std::enable_shared_from_this<ServiceClass>::shared_from_this;
    any serviceType;

    (Args... args_);
};

#endif
