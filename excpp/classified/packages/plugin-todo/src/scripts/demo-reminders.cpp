#include "demo-reminders.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> setupReminderDemo(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << '🚀 Setting up reminder demo scenarios...\n' << std::endl;

    const auto dataService = createTodoDataService(runtime);

    // Test user details
    const auto testUserId = 'test-user-123';
    const auto testRoomId = 'test-room-456';
    const auto testWorldId = 'test-world-789';

    // Scenario 1: Overdue task (should trigger immediately)
    std::cout << '📌 Scenario 1: Creating an overdue task...' << std::endl;
    const auto overdueDate = new Date();
    overdueDate.setHours(overdueDate.getHours() - 2); // 2 hours overdue;

    const auto overdueTaskId = dataService.createTodo({;
        agentId: runtime.agentId,
        worldId: testWorldId,
        roomId: testRoomId,
        entityId: testUserId,
        name: '🚨 Submit expense report',
        description: 'Q4 expense report submission - URGENT',
        type: 'one-off',
        priority: 1,
        isUrgent: true,
        dueDate: overdueDate,
        tags: ['urgent', 'finance', 'overdue'],
        });
        std::cout << "✅ Created overdue task: " + std::to_string(overdueTaskId) << std::endl;

        // Scenario 2: Task due in 15 minutes (should trigger upcoming reminder)
        std::cout << '\n📌 Scenario 2: Creating a task due in 15 minutes...' << std::endl;
        const auto upcomingDate = new Date();
        upcomingDate.setMinutes(upcomingDate.getMinutes() + 15);

        const auto upcomingTaskId = dataService.createTodo({;
            agentId: runtime.agentId,
            worldId: testWorldId,
            roomId: testRoomId,
            entityId: testUserId,
            name: '📞 Team standup call',
            description: 'Daily team sync on Discord',
            type: 'one-off',
            priority: 2,
            isUrgent: false,
            dueDate: upcomingDate,
            tags: ['meeting', 'team'],
            });
            std::cout << "✅ Created upcoming task: " + std::to_string(upcomingTaskId) << std::endl;

            // Scenario 3: Daily recurring task (will remind at 9 AM and 6 PM)
            std::cout << '\n📌 Scenario 3: Creating daily recurring tasks...' << std::endl;

            const auto dailyTasks = [;
            {
                name: '💊 Take morning vitamins',
                description: 'Vitamin D, B12, and Omega-3',
                tags: ['health', 'daily', 'morning'],
                },
                {
                    name: '🏃 30-minute exercise',
                    description: 'Cardio or strength training',
                    tags: ['health', 'daily', 'fitness'],
                    },
                    {
                        name: '📚 Read for 20 minutes',
                        description: 'Continue reading current book',
                        tags: ['personal', 'daily', 'learning'],
                        },
                        ];

                        for (const auto& task : dailyTasks)
                            const auto taskId = dataService.createTodo({;
                                agentId: runtime.agentId,
                                worldId: testWorldId,
                                roomId: testRoomId,
                                entityId: testUserId,
                                name: task.name,
                                description: task.description,
                                type: 'daily',
                                priority: 3,
                                isUrgent: false,
                                tags: task.tags,
                                });
                                std::cout << "✅ Created daily task: " + std::to_string(task.name) + " (" + std::to_string(taskId) + ")" << std::endl;
                            }

                            // Scenario 4: Future tasks with various deadlines
                            std::cout << '\n📌 Scenario 4: Creating future tasks with deadlines...' << std::endl;

                            const auto futureTasks = [;
                            {
                                name: '📝 Finish project proposal',
                                hoursFromNow: 4,
                                priority: 2,
                                isUrgent: false,
                                },
                                {
                                    name: '🎂 Buy birthday gift for Sarah',
                                    hoursFromNow: 24,
                                    priority: 3,
                                    isUrgent: false,
                                    },
                                    {
                                        name: '✈️ Check in for flight',
                                        hoursFromNow: 48,
                                        priority: 1,
                                        isUrgent: true,
                                        },
                                        ];

                                        for (const auto& task : futureTasks)
                                            const auto dueDate = new Date();
                                            dueDate.setHours(dueDate.getHours() + task.hoursFromNow);

                                            const auto taskId = dataService.createTodo({;
                                                agentId: runtime.agentId,
                                                worldId: testWorldId,
                                                roomId: testRoomId,
                                                entityId: testUserId,
                                                name: task.name,
                                                type: 'one-off',
                                                priority: task.priority,
                                                isUrgent: task.isUrgent,
                                                dueDate,
                                                tags: ['future'],
                                                });
                                                std::cout << "✅ Created future task: " + std::to_string(task.name) + " (due in " + std::to_string(task.hoursFromNow) + "h)" << std::endl;
                                            }

                                            // Scenario 5: Aspirational tasks (no due date)
                                            std::cout << '\n📌 Scenario 5: Creating aspirational tasks...' << std::endl;

                                            const auto aspirationalTasks = [;
                                            '🎸 Learn to play guitar',
                                            '🗣️ Become fluent in Spanish',
                                            '🏔️ Climb Mount Kilimanjaro',
                                            ];

                                            for (const auto& taskName : aspirationalTasks)
                                                const auto taskId = dataService.createTodo({;
                                                    agentId: runtime.agentId,
                                                    worldId: testWorldId,
                                                    roomId: testRoomId,
                                                    entityId: testUserId,
                                                    name: taskName,
                                                    type: 'aspirational',
                                                    priority: 4,
                                                    isUrgent: false,
                                                    tags: ['goals', 'long-term'],
                                                    });
                                                    std::cout << "✅ Created aspirational task: " + std::to_string(taskName) << std::endl;
                                                }

                                                std::cout << '\n📊 Demo Summary:' << std::endl;
                                                std::cout << '- 1 overdue task (immediate high-priority reminder)' << std::endl;
                                                std::cout << '- 1 task due in 15 minutes (upcoming reminder)' << std::endl;
                                                std::cout << '- 3 daily recurring tasks (9 AM & 6 PM reminders)' << std::endl;
                                                std::cout << '- 3 future tasks with various deadlines' << std::endl;
                                                std::cout << '- 3 aspirational tasks (no reminders)' << std::endl;

                                                // Force an immediate reminder check
                                                std::cout << '\n🔔 Triggering immediate reminder check...' << std::endl;
                                                const auto reminderService = runtime.getService('TODO_REMINDER');
                                                const auto typedReminderService = reminderService | nullptr;
                                                if (typedReminderService) {
                                                    typedReminderService.checkTasksForReminders();
                                                    std::cout << '✅ Reminder check complete!' << std::endl;
                                                    } else {
                                                        std::cout << "⚠️ Reminder service not found - make sure it's initialized" << std::endl;
                                                    }

                                                    std::cout << '\n📱 Expected behavior:' << std::endl;
                                                    std::cout << '1. Overdue task should trigger immediately with HIGH priority' << std::endl;
                                                    std::cout << '2. 15-minute task should trigger with MEDIUM priority' << std::endl;
                                                    std::cout << '3. Daily tasks will remind at 9 AM and 6 PM with LOW priority' << std::endl;
                                                    std::cout << '4. Future tasks will remind 30 minutes before due time' << std::endl;
                                                    std::cout << '5. Check your connected platforms (Discord/Slack/etc) for notifications!' << std::endl;

                                                    return {
                                                        testUserId,
                                                        testRoomId,
                                                        testWorldId,
                                                        overdueTaskId,
                                                        upcomingTaskId,
                                                        };

}

std::future<void> monitorReminders(IAgentRuntime runtime, double duration = 60000) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n👀 Monitoring reminders for " + std::to_string(duration / 1000) + " seconds..." << std::endl;

    const auto startTime = Date.now();
    const auto checkInterval = 5000; // Check every 5 seconds;

    const auto monitor = setInterval(() => {;
        const auto elapsed = Date.now() - startTime;
        const auto remaining = duration - elapsed;

        if (remaining <= 0) {
            clearInterval(monitor);
            std::cout << '\n✅ Monitoring complete!' << std::endl;
            return;
        }

        std::cout << "⏱️  " + std::to_string(Math.floor(remaining / 1000)) + "s remaining..." << std::endl;
        }, checkInterval);

        // Wait for monitoring to complete
        new Promise((resolve) => setTimeout(resolve, duration));

}

std::future<void> cleanupDemo(IAgentRuntime runtime, UUID testUserId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << '\n🧹 Cleaning up demo tasks...' << std::endl;

    const auto dataService = createTodoDataService(runtime);
    const auto todos = dataService.getTodos({ entityId: testUserId });

    for (const auto& todo : todos)
        dataService.deleteTodo(todo.id);
    }

    std::cout << "✅ Deleted " + std::to_string(todos.length) + " demo tasks" << std::endl;

}

} // namespace elizaos
