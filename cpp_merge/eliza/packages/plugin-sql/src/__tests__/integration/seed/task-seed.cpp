#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/task-seed.h"

any taskTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any taskTestRoomId = as<std::shared_ptr<UUID>>(uuidv4());
any taskTestWorldId = as<std::shared_ptr<UUID>>(uuidv4());
any taskTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
array<std::shared_ptr<Task>> taskTestTasks = array<std::shared_ptr<Task>>{ object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("name"), std:("Test Task 1")}, 
    object::pair{std:("description"), std:("This is a test task for integration testing #1")}, 
    object::pair{std:("roomId"), taskTestRoomId}, 
    object::pair{std:("worldId"), taskTestWorldId}, 
    object::pair{std:("tags"), array<string>{ std:("test"), std:("integration"), std:("task1") }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("priority"), std:("high")}, 
        object::pair{std:("dueDate"), Date->now() + 86400000}, 
        object::pair{std:("status"), std:("pending")}
    }}
}, object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("name"), std:("Test Task 2")}, 
    object::pair{std:("description"), std:("This is a test task for integration testing #2")}, 
    object::pair{std:("roomId"), taskTestRoomId}, 
    object::pair{std:("worldId"), taskTestWorldId}, 
    object::pair{std:("tags"), array<string>{ std:("test"), std:("integration"), std:("task2") }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("priority"), std:("medium")}, 
        object::pair{std:("dueDate"), Date->now() + 172800000}, 
        object::pair{std:("status"), std:("pending")}
    }}
}, object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("name"), std:("Test Task 3")}, 
    object::pair{std:("description"), std:("This is a test task for integration testing #3")}, 
    object::pair{std:("roomId"), taskTestRoomId}, 
    object::pair{std:("worldId"), taskTestWorldId}, 
    object::pair{std:("tags"), array<string>{ std:("test"), std:("integration"), std:("task3") }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("priority"), std:("low")}, 
        object::pair{std:("dueDate"), Date->now() + 259200000}, 
        object::pair{std:("status"), std:("pending")}
    }}
}, object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("name"), std:("Special Test Task")}, 
    object::pair{std:("description"), std:("This is a special task with unique name for name search testing")}, 
    object::pair{std:("roomId"), taskTestRoomId}, 
    object::pair{std:("worldId"), taskTestWorldId}, 
    object::pair{std:("tags"), array<string>{ std:("test"), std:("integration"), std:("special") }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("priority"), std:("high")}, 
        object::pair{std:("dueDate"), Date->now() + 43200000}, 
        object::pair{std:("status"), std:("pending")}, 
        object::pair{std:("options"), array<object>{ object{
            object::pair{std:("name"), std:("Option 1")}, 
            object::pair{std:("description"), std:("First option")}
        }, object{
            object::pair{std:("name"), std:("Option 2")}, 
            object::pair{std:("description"), std:("Second option")}
        } }}
    }}
} };
std::shared_ptr<Task> taskTestTaskDifferentRoom = object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("name"), std:("Task Different Room")}, 
    object::pair{std:("description"), std:("This task belongs to a different room for filter testing")}, 
    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("worldId"), taskTestWorldId}, 
    object::pair{std:("tags"), array<string>{ std:("test"), std:("different-room") }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("priority"), std:("medium")}, 
        object::pair{std:("status"), std:("pending")}
    }}
};
std::shared_ptr<Task> taskTestTaskWithSpecificTags = object{
    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std:("name"), std:("Task With Specific Tags")}, 
    object::pair{std:("description"), std:("This task has specific tags for filter testing")}, 
    object::pair{std:("roomId"), taskTestRoomId}, 
    object::pair{std:("worldId"), taskTestWorldId}, 
    object::pair{std:("tags"), array<string>{ std:("test"), std:("filter-test"), std:("specific-tag") }}, 
    object::pair{std:("metadata"), object{
        object::pair{std:("priority"), std:("high")}, 
        object::pair{std:("status"), std:("pending")}
    }}
};

void Main(void)
{
}

MAIN
