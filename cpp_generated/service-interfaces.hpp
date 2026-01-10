#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_TYPES_SERVICE-INTERFACES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_TYPES_SERVICE-INTERFACES_H
#include "core.h"

class TodoReminderService;
class TodoNotificationService;
class TodoDailyResetService;
class ContentWithText;
class ServiceClass;
class MockFunction;

class TodoReminderService : public object, public std::enable_shared_from_this<TodoReminderService> {
public:
    using std::enable_shared_from_this<TodoReminderService>::shared_from_this;
    virtual std::shared_ptr<Promise<void>> checkTasksForReminders() = 0;
};

class TodoNotificationService : public object, public std::enable_shared_from_this<TodoNotificationService> {
public:
    using std::enable_shared_from_this<TodoNotificationService>::shared_from_this;
    virtual std::shared_ptr<Promise<void>> sendNotification(string message, string roomId) = 0;
};

class TodoDailyResetService : public object, public std::enable_shared_from_this<TodoDailyResetService> {
public:
    using std::enable_shared_from_this<TodoDailyResetService>::shared_from_this;
    virtual std::shared_ptr<Promise<double>> resetDailyTasks() = 0;
};

class ContentWithText : public object, public std::enable_shared_from_this<ContentWithText> {
public:
    using std::enable_shared_from_this<ContentWithText>::shared_from_this;
    string text;
};

class ServiceClass : public object, public std::enable_shared_from_this<ServiceClass> {
public:
    using std::enable_shared_from_this<ServiceClass>::shared_from_this;
    string serviceType;

    string serviceName;
};

class MockFunction : public object, public std::enable_shared_from_this<MockFunction> {
public:
    using std::enable_shared_from_this<MockFunction>::shared_from_this;
    object mock;
};

#endif
