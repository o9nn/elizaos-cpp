#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/task-seed.h"

any taskTestAgentId = as<std::shared_ptr<UUID>>(uuidv4());
any taskTestRoomId = as<std::shared_ptr<UUID>>(uuidv4());
any taskTestWorldId = as<std::shared_ptr<UUID>>(uuidv4());
any taskTestEntityId = as<std::shared_ptr<UUID>>(uuidv4());
array<std::shared_ptr<Task>> taskTestTasks = array<std::shared_ptr<Task>>{ object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("name"), std::string("Test Task 1")}, 
    object::pair{std::string("description"), std::string("This is a test task for integration testing #1")}, 
    object::pair{std::string("roomId"), taskTestRoomId}, 
    object::pair{std::string("worldId"), taskTestWorldId}, 
    object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("integration"), std::string("task1") }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("priority"), std::string("high")}, 
        object::pair{std::string("dueDate"), Date->now() + 86400000}, 
        object::pair{std::string("status"), std::string("pending")}
    }}
}, object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("name"), std::string("Test Task 2")}, 
    object::pair{std::string("description"), std::string("This is a test task for integration testing #2")}, 
    object::pair{std::string("roomId"), taskTestRoomId}, 
    object::pair{std::string("worldId"), taskTestWorldId}, 
    object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("integration"), std::string("task2") }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("priority"), std::string("medium")}, 
        object::pair{std::string("dueDate"), Date->now() + 172800000}, 
        object::pair{std::string("status"), std::string("pending")}
    }}
}, object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("name"), std::string("Test Task 3")}, 
    object::pair{std::string("description"), std::string("This is a test task for integration testing #3")}, 
    object::pair{std::string("roomId"), taskTestRoomId}, 
    object::pair{std::string("worldId"), taskTestWorldId}, 
    object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("integration"), std::string("task3") }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("priority"), std::string("low")}, 
        object::pair{std::string("dueDate"), Date->now() + 259200000}, 
        object::pair{std::string("status"), std::string("pending")}
    }}
}, object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("name"), std::string("Special Test Task")}, 
    object::pair{std::string("description"), std::string("This is a special task with unique name for name search testing")}, 
    object::pair{std::string("roomId"), taskTestRoomId}, 
    object::pair{std::string("worldId"), taskTestWorldId}, 
    object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("integration"), std::string("special") }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("priority"), std::string("high")}, 
        object::pair{std::string("dueDate"), Date->now() + 43200000}, 
        object::pair{std::string("status"), std::string("pending")}, 
        object::pair{std::string("options"), array<object>{ object{
            object::pair{std::string("name"), std::string("Option 1")}, 
            object::pair{std::string("description"), std::string("First option")}
        }, object{
            object::pair{std::string("name"), std::string("Option 2")}, 
            object::pair{std::string("description"), std::string("Second option")}
        } }}
    }}
} };
std::shared_ptr<Task> taskTestTaskDifferentRoom = object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("name"), std::string("Task Different Room")}, 
    object::pair{std::string("description"), std::string("This task belongs to a different room for filter testing")}, 
    object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("worldId"), taskTestWorldId}, 
    object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("different-room") }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("priority"), std::string("medium")}, 
        object::pair{std::string("status"), std::string("pending")}
    }}
};
std::shared_ptr<Task> taskTestTaskWithSpecificTags = object{
    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
    object::pair{std::string("name"), std::string("Task With Specific Tags")}, 
    object::pair{std::string("description"), std::string("This task has specific tags for filter testing")}, 
    object::pair{std::string("roomId"), taskTestRoomId}, 
    object::pair{std::string("worldId"), taskTestWorldId}, 
    object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("filter-test"), std::string("specific-tag") }}, 
    object::pair{std::string("metadata"), object{
        object::pair{std::string("priority"), std::string("high")}, 
        object::pair{std::string("status"), std::string("pending")}
    }}
};

void Main(void)
{
}

MAIN
