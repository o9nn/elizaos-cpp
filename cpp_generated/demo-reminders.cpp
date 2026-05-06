#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/scripts/demo-reminders.h"

any setupReminderDemo(std::shared_ptr<IAgentRuntime> runtime)
{
    console->log(std::string("🚀 Setting up reminder demo scenarios...\
"));
    auto dataService = createTodoDataService(runtime);
    auto testUserId = as<std::shared_ptr<UUID>>(std::string("test-user-123"));
    auto testRoomId = as<std::shared_ptr<UUID>>(std::string("test-room-456"));
    auto testWorldId = as<std::shared_ptr<UUID>>(std::string("test-world-789"));
    console->log(std::string("📌 Scenario 1: Creating an overdue task..."));
    auto overdueDate = std::make_shared<Date>();
    overdueDate->setHours(overdueDate->getHours() - 2);
    auto overdueTaskId = std::async([=]() { dataService->createTodo(object{
        object::pair{std::string("agentId"), runtime->agentId}, 
        object::pair{std::string("worldId"), testWorldId}, 
        object::pair{std::string("roomId"), testRoomId}, 
        object::pair{std::string("entityId"), testUserId}, 
        object::pair{std::string("name"), std::string("🚨 Submit expense report")}, 
        object::pair{std::string("description"), std::string("Q4 expense report submission - URGENT")}, 
        object::pair{std::string("type"), std::string("one-off")}, 
        object::pair{std::string("priority"), 1}, 
        object::pair{std::string("isUrgent"), true}, 
        object::pair{std::string("dueDate"), overdueDate}, 
        object::pair{std::string("tags"), array<string>{ std::string("urgent"), std::string("finance"), std::string("overdue") }}
    }); });
    console->log(std::string("✅ Created overdue task: ") + overdueTaskId + string_empty);
    console->log(std::string("\
📌 Scenario 2: Creating a task due in 15 minutes..."));
    auto upcomingDate = std::make_shared<Date>();
    upcomingDate->setMinutes(upcomingDate->getMinutes() + 15);
    auto upcomingTaskId = std::async([=]() { dataService->createTodo(object{
        object::pair{std::string("agentId"), runtime->agentId}, 
        object::pair{std::string("worldId"), testWorldId}, 
        object::pair{std::string("roomId"), testRoomId}, 
        object::pair{std::string("entityId"), testUserId}, 
        object::pair{std::string("name"), std::string("📞 Team standup call")}, 
        object::pair{std::string("description"), std::string("Daily team sync on Discord")}, 
        object::pair{std::string("type"), std::string("one-off")}, 
        object::pair{std::string("priority"), 2}, 
        object::pair{std::string("isUrgent"), false}, 
        object::pair{std::string("dueDate"), upcomingDate}, 
        object::pair{std::string("tags"), array<string>{ std::string("meeting"), std::string("team") }}
    }); });
    console->log(std::string("✅ Created upcoming task: ") + upcomingTaskId + string_empty);
    console->log(std::string("\
📌 Scenario 3: Creating daily recurring tasks..."));
    auto dailyTasks = array<object>{ object{
        object::pair{std::string("name"), std::string("💊 Take morning vitamins")}, 
        object::pair{std::string("description"), std::string("Vitamin D, B12, and Omega-3")}, 
        object::pair{std::string("tags"), array<string>{ std::string("health"), std::string("daily"), std::string("morning") }}
    }, object{
        object::pair{std::string("name"), std::string("🏃 30-minute exercise")}, 
        object::pair{std::string("description"), std::string("Cardio or strength training")}, 
        object::pair{std::string("tags"), array<string>{ std::string("health"), std::string("daily"), std::string("fitness") }}
    }, object{
        object::pair{std::string("name"), std::string("📚 Read for 20 minutes")}, 
        object::pair{std::string("description"), std::string("Continue reading current book")}, 
        object::pair{std::string("tags"), array<string>{ std::string("personal"), std::string("daily"), std::string("learning") }}
    } };
    for (auto& task : dailyTasks)
    {
        auto taskId = std::async([=]() { dataService->createTodo(object{
            object::pair{std::string("agentId"), runtime->agentId}, 
            object::pair{std::string("worldId"), testWorldId}, 
            object::pair{std::string("roomId"), testRoomId}, 
            object::pair{std::string("entityId"), testUserId}, 
            object::pair{std::string("name"), task["name"]}, 
            object::pair{std::string("description"), task["description"]}, 
            object::pair{std::string("type"), std::string("daily")}, 
            object::pair{std::string("priority"), 3}, 
            object::pair{std::string("isUrgent"), false}, 
            object::pair{std::string("tags"), task["tags"]}
        }); });
        console->log(std::string("✅ Created daily task: ") + task["name"] + std::string(" (") + taskId + std::string(")"));
    }
    console->log(std::string("\
📌 Scenario 4: Creating future tasks with deadlines..."));
    auto futureTasks = array<object>{ object{
        object::pair{std::string("name"), std::string("📝 Finish project proposal")}, 
        object::pair{std::string("hoursFromNow"), 4}, 
        object::pair{std::string("priority"), 2}, 
        object::pair{std::string("isUrgent"), false}
    }, object{
        object::pair{std::string("name"), std::string("🎂 Buy birthday gift for Sarah")}, 
        object::pair{std::string("hoursFromNow"), 24}, 
        object::pair{std::string("priority"), 3}, 
        object::pair{std::string("isUrgent"), false}
    }, object{
        object::pair{std::string("name"), std::string("✈️ Check in for flight")}, 
        object::pair{std::string("hoursFromNow"), 48}, 
        object::pair{std::string("priority"), 1}, 
        object::pair{std::string("isUrgent"), true}
    } };
    for (auto& task : futureTasks)
    {
        auto dueDate = std::make_shared<Date>();
        dueDate->setHours(dueDate->getHours() + task["hoursFromNow"]);
        auto taskId = std::async([=]() { dataService->createTodo(object{
            object::pair{std::string("agentId"), runtime->agentId}, 
            object::pair{std::string("worldId"), testWorldId}, 
            object::pair{std::string("roomId"), testRoomId}, 
            object::pair{std::string("entityId"), testUserId}, 
            object::pair{std::string("name"), task["name"]}, 
            object::pair{std::string("type"), std::string("one-off")}, 
            object::pair{std::string("priority"), task["priority"]}, 
            object::pair{std::string("isUrgent"), task["isUrgent"]}, 
            object::pair{std::string("dueDate"), std::string("dueDate")}, 
            object::pair{std::string("tags"), array<string>{ std::string("future") }}
        }); });
        console->log(std::string("✅ Created future task: ") + task["name"] + std::string(" (due in ") + task["hoursFromNow"] + std::string("h)"));
    }
    console->log(std::string("\
📌 Scenario 5: Creating aspirational tasks..."));
    auto aspirationalTasks = array<string>{ std::string("🎸 Learn to play guitar"), std::string("🗣️ Become fluent in Spanish"), std::string("🏔️ Climb Mount Kilimanjaro") };
    for (auto& taskName : aspirationalTasks)
    {
        auto taskId = std::async([=]() { dataService->createTodo(object{
            object::pair{std::string("agentId"), runtime->agentId}, 
            object::pair{std::string("worldId"), testWorldId}, 
            object::pair{std::string("roomId"), testRoomId}, 
            object::pair{std::string("entityId"), testUserId}, 
            object::pair{std::string("name"), taskName}, 
            object::pair{std::string("type"), std::string("aspirational")}, 
            object::pair{std::string("priority"), 4}, 
            object::pair{std::string("isUrgent"), false}, 
            object::pair{std::string("tags"), array<string>{ std::string("goals"), std::string("long-term") }}
        }); });
        console->log(std::string("✅ Created aspirational task: ") + taskName + string_empty);
    }
    console->log(std::string("\
📊 Demo Summary:"));
    console->log(std::string("- 1 overdue task (immediate high-priority reminder)"));
    console->log(std::string("- 1 task due in 15 minutes (upcoming reminder)"));
    console->log(std::string("- 3 daily recurring tasks (9 AM & 6 PM reminders)"));
    console->log(std::string("- 3 future tasks with various deadlines"));
    console->log(std::string("- 3 aspirational tasks (no reminders)"));
    console->log(std::string("\
🔔 Triggering immediate reminder check..."));
    auto reminderService = runtime->getService(as<any>(std::string("TODO_REMINDER")));
    auto typedReminderService = as<any>(reminderService);
    if (typedReminderService) {
        std::async([=]() { typedReminderService->checkTasksForReminders(); });
        console->log(std::string("✅ Reminder check complete!"));
    } else {
        console->log(std::string("⚠️ Reminder service not found - make sure it's initialized"));
    }
    console->log(std::string("\
📱 Expected behavior:"));
    console->log(std::string("1. Overdue task should trigger immediately with HIGH priority"));
    console->log(std::string("2. 15-minute task should trigger with MEDIUM priority"));
    console->log(std::string("3. Daily tasks will remind at 9 AM and 6 PM with LOW priority"));
    console->log(std::string("4. Future tasks will remind 30 minutes before due time"));
    console->log(std::string("5. Check your connected platforms (Discord/Slack/etc) for notifications!"));
    return object{
        object::pair{std::string("testUserId"), std::string("testUserId")}, 
        object::pair{std::string("testRoomId"), std::string("testRoomId")}, 
        object::pair{std::string("testWorldId"), std::string("testWorldId")}, 
        object::pair{std::string("overdueTaskId"), std::string("overdueTaskId")}, 
        object::pair{std::string("upcomingTaskId"), std::string("upcomingTaskId")}
    };
};


void monitorReminders(std::shared_ptr<IAgentRuntime> runtime, double duration)
{
    console->log(std::string("\
👀 Monitoring reminders for ") + (duration / 1000) + std::string(" seconds..."));
    shared startTime = Date->now();
    auto checkInterval = 5000;
    shared monitor = setInterval([=]() mutable
    {
        auto elapsed = Date->now() - startTime;
        auto remaining = duration - elapsed;
        if (remaining <= 0) {
            clearInterval(monitor);
            console->log(std::string("\
✅ Monitoring complete!"));
            return;
        }
        console->log(std::string("⏱️  ") + Math->floor(remaining / 1000) + std::string("s remaining..."));
    }
    , checkInterval);
    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
    {
        return setTimeout(resolve, duration);
    }
    ); });
};


void cleanupDemo(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<UUID> testUserId)
{
    console->log(std::string("\
🧹 Cleaning up demo tasks..."));
    auto dataService = createTodoDataService(runtime);
    auto todos = std::async([=]() { dataService->getTodos(object{
        object::pair{std::string("entityId"), testUserId}
    }); });
    for (auto& todo : todos)
    {
        std::async([=]() { dataService->deleteTodo(todo->id); });
    }
    console->log(std::string("✅ Deleted ") + todos->get_length() + std::string(" demo tasks"));
};


