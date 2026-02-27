#include "demo-reminders.h"

any setupReminderDemo(std::shared_ptr<IAgentRuntime> runtime)
{
    console->log(std:("🚀 Setting up reminder demo scenarios...\
"));
    auto dataService = createTodoDataService(runtime);
    auto testUserId = as<std::shared_ptr<UUID>>(std:("test-user-123"));
    auto testRoomId = as<std::shared_ptr<UUID>>(std:("test-room-456"));
    auto testWorldId = as<std::shared_ptr<UUID>>(std:("test-world-789"));
    console->log(std:("📌 Scenario 1: Creating an overdue task..."));
    auto overdueDate = std::make_shared<Date>();
    overdueDate->setHours(overdueDate->getHours() - 2);
    auto overdueTaskId = std::async([=]() { dataService->createTodo(object{
        object::pair{std:("agentId"), runtime->agentId}, 
        object::pair{std:("worldId"), testWorldId}, 
        object::pair{std:("roomId"), testRoomId}, 
        object::pair{std:("entityId"), testUserId}, 
        object::pair{std:("name"), std:("🚨 Submit expense report")}, 
        object::pair{std:("description"), std:("Q4 expense report submission - URGENT")}, 
        object::pair{std:("type"), std:("one-off")}, 
        object::pair{std:("priority"), 1}, 
        object::pair{std:("isUrgent"), true}, 
        object::pair{std:("dueDate"), overdueDate}, 
        object::pair{std:("tags"), array<string>{ std:("urgent"), std:("finance"), std:("overdue") }}
    }); });
    console->log(std:("✅ Created overdue task: ") + overdueTaskId + string_empty);
    console->log(std:("\
📌 Scenario 2: Creating a task due in 15 minutes..."));
    auto upcomingDate = std::make_shared<Date>();
    upcomingDate->setMinutes(upcomingDate->getMinutes() + 15);
    auto upcomingTaskId = std::async([=]() { dataService->createTodo(object{
        object::pair{std:("agentId"), runtime->agentId}, 
        object::pair{std:("worldId"), testWorldId}, 
        object::pair{std:("roomId"), testRoomId}, 
        object::pair{std:("entityId"), testUserId}, 
        object::pair{std:("name"), std:("📞 Team standup call")}, 
        object::pair{std:("description"), std:("Daily team sync on Discord")}, 
        object::pair{std:("type"), std:("one-off")}, 
        object::pair{std:("priority"), 2}, 
        object::pair{std:("isUrgent"), false}, 
        object::pair{std:("dueDate"), upcomingDate}, 
        object::pair{std:("tags"), array<string>{ std:("meeting"), std:("team") }}
    }); });
    console->log(std:("✅ Created upcoming task: ") + upcomingTaskId + string_empty);
    console->log(std:("\
📌 Scenario 3: Creating daily recurring tasks..."));
    auto dailyTasks = array<object>{ object{
        object::pair{std:("name"), std:("💊 Take morning vitamins")}, 
        object::pair{std:("description"), std:("Vitamin D, B12, and Omega-3")}, 
        object::pair{std:("tags"), array<string>{ std:("health"), std:("daily"), std:("morning") }}
    }, object{
        object::pair{std:("name"), std:("🏃 30-minute exercise")}, 
        object::pair{std:("description"), std:("Cardio or strength training")}, 
        object::pair{std:("tags"), array<string>{ std:("health"), std:("daily"), std:("fitness") }}
    }, object{
        object::pair{std:("name"), std:("📚 Read for 20 minutes")}, 
        object::pair{std:("description"), std:("Continue reading current book")}, 
        object::pair{std:("tags"), array<string>{ std:("personal"), std:("daily"), std:("learning") }}
    } };
    for (auto& task : dailyTasks)
    {
        auto taskId = std::async([=]() { dataService->createTodo(object{
            object::pair{std:("agentId"), runtime->agentId}, 
            object::pair{std:("worldId"), testWorldId}, 
            object::pair{std:("roomId"), testRoomId}, 
            object::pair{std:("entityId"), testUserId}, 
            object::pair{std:("name"), task["name"]}, 
            object::pair{std:("description"), task["description"]}, 
            object::pair{std:("type"), std:("daily")}, 
            object::pair{std:("priority"), 3}, 
            object::pair{std:("isUrgent"), false}, 
            object::pair{std:("tags"), task["tags"]}
        }); });
        console->log(std:("✅ Created daily task: ") + task["name"] + std:(" (") + taskId + std:(")"));
    }
    console->log(std:("\
📌 Scenario 4: Creating future tasks with deadlines..."));
    auto futureTasks = array<object>{ object{
        object::pair{std:("name"), std:("📝 Finish project proposal")}, 
        object::pair{std:("hoursFromNow"), 4}, 
        object::pair{std:("priority"), 2}, 
        object::pair{std:("isUrgent"), false}
    }, object{
        object::pair{std:("name"), std:("🎂 Buy birthday gift for Sarah")}, 
        object::pair{std:("hoursFromNow"), 24}, 
        object::pair{std:("priority"), 3}, 
        object::pair{std:("isUrgent"), false}
    }, object{
        object::pair{std:("name"), std:("✈️ Check in for flight")}, 
        object::pair{std:("hoursFromNow"), 48}, 
        object::pair{std:("priority"), 1}, 
        object::pair{std:("isUrgent"), true}
    } };
    for (auto& task : futureTasks)
    {
        auto dueDate = std::make_shared<Date>();
        dueDate->setHours(dueDate->getHours() + task["hoursFromNow"]);
        auto taskId = std::async([=]() { dataService->createTodo(object{
            object::pair{std:("agentId"), runtime->agentId}, 
            object::pair{std:("worldId"), testWorldId}, 
            object::pair{std:("roomId"), testRoomId}, 
            object::pair{std:("entityId"), testUserId}, 
            object::pair{std:("name"), task["name"]}, 
            object::pair{std:("type"), std:("one-off")}, 
            object::pair{std:("priority"), task["priority"]}, 
            object::pair{std:("isUrgent"), task["isUrgent"]}, 
            object::pair{std:("dueDate"), std:("dueDate")}, 
            object::pair{std:("tags"), array<string>{ std:("future") }}
        }); });
        console->log(std:("✅ Created future task: ") + task["name"] + std:(" (due in ") + task["hoursFromNow"] + std:("h)"));
    }
    console->log(std:("\
📌 Scenario 5: Creating aspirational tasks..."));
    auto aspirationalTasks = array<string>{ std:("🎸 Learn to play guitar"), std:("🗣️ Become fluent in Spanish"), std:("🏔️ Climb Mount Kilimanjaro") };
    for (auto& taskName : aspirationalTasks)
    {
        auto taskId = std::async([=]() { dataService->createTodo(object{
            object::pair{std:("agentId"), runtime->agentId}, 
            object::pair{std:("worldId"), testWorldId}, 
            object::pair{std:("roomId"), testRoomId}, 
            object::pair{std:("entityId"), testUserId}, 
            object::pair{std:("name"), taskName}, 
            object::pair{std:("type"), std:("aspirational")}, 
            object::pair{std:("priority"), 4}, 
            object::pair{std:("isUrgent"), false}, 
            object::pair{std:("tags"), array<string>{ std:("goals"), std:("long-term") }}
        }); });
        console->log(std:("✅ Created aspirational task: ") + taskName + string_empty);
    }
    console->log(std:("\
📊 Demo Summary:"));
    console->log(std:("- 1 overdue task (immediate high-priority reminder)"));
    console->log(std:("- 1 task due in 15 minutes (upcoming reminder)"));
    console->log(std:("- 3 daily recurring tasks (9 AM & 6 PM reminders)"));
    console->log(std:("- 3 future tasks with various deadlines"));
    console->log(std:("- 3 aspirational tasks (no reminders)"));
    console->log(std:("\
🔔 Triggering immediate reminder check..."));
    auto reminderService = runtime->getService(as<any>(std:("TODO_REMINDER")));
    auto typedReminderService = as<any>(reminderService);
    if (typedReminderService) {
        std::async([=]() { typedReminderService->checkTasksForReminders(); });
        console->log(std:("✅ Reminder check complete!"));
    } else {
        console->log(std:("⚠️ Reminder service not found - make sure it's initialized"));
    }
    console->log(std:("\
📱 Expected behavior:"));
    console->log(std:("1. Overdue task should trigger immediately with HIGH priority"));
    console->log(std:("2. 15-minute task should trigger with MEDIUM priority"));
    console->log(std:("3. Daily tasks will remind at 9 AM and 6 PM with LOW priority"));
    console->log(std:("4. Future tasks will remind 30 minutes before due time"));
    console->log(std:("5. Check your connected platforms (Discord/Slack/etc) for notifications!"));
    return object{
        object::pair{std:("testUserId"), std:("testUserId")}, 
        object::pair{std:("testRoomId"), std:("testRoomId")}, 
        object::pair{std:("testWorldId"), std:("testWorldId")}, 
        object::pair{std:("overdueTaskId"), std:("overdueTaskId")}, 
        object::pair{std:("upcomingTaskId"), std:("upcomingTaskId")}
    };
};


void monitorReminders(std::shared_ptr<IAgentRuntime> runtime, double duration)
{
    console->log(std:("\
👀 Monitoring reminders for ") + (duration / 1000) + std:(" seconds..."));
    shared startTime = Date->now();
    auto checkInterval = 5000;
    shared monitor = setInterval([=]() mutable
    {
        auto elapsed = Date->now() - startTime;
        auto remaining = duration - elapsed;
        if (remaining <= 0) {
            clearInterval(monitor);
            console->log(std:("\
✅ Monitoring complete!"));
            return;
        }
        console->log(std:("⏱️  ") + Math->floor(remaining / 1000) + std:("s remaining..."));
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
    console->log(std:("\
🧹 Cleaning up demo tasks..."));
    auto dataService = createTodoDataService(runtime);
    auto todos = std::async([=]() { dataService->getTodos(object{
        object::pair{std:("entityId"), testUserId}
    }); });
    for (auto& todo : todos)
    {
        std::async([=]() { dataService->deleteTodo(todo->id); });
    }
    console->log(std:("✅ Deleted ") + todos->get_length() + std:(" demo tasks"));
};


