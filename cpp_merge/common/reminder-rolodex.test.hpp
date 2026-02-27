#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_TESTS_REMINDER_ROLODEX_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_TESTS_REMINDER_ROLODEX_TEST_H
#include "core.hpp"
#include "bun:test.h"
// External dependency removed
// External dependency removed
#include "../services/reminderService.h"
#include "uuid.hpp"
using uuidv4 = v4;

class MockRolodexService;
class LoggerMock;

class MockRolodexService : public object, public std::enable_shared_from_this<MockRolodexService> {
public:
    using std::enable_shared_from_this<MockRolodexService>::shared_from_this;
    ReturnType<mock> sendMessage;
};

class LoggerMock : public object, public std::enable_shared_from_this<LoggerMock> {
public:
    using std::enable_shared_from_this<LoggerMock>::shared_from_this;
    object mock;
};

#endif
