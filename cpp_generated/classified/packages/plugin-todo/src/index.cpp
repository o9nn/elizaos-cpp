#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/index.h"

std::shared_ptr<Plugin> TodoPlugin = object{
    object::pair{std::string("name"), std::string("todo")}, 
    object::pair{std::string("description"), std::string("Provides task management functionality with daily recurring and one-off tasks.")}, 
    object::pair{std::string("providers"), array<any>{ todosProvider }}, 
    object::pair{std::string("dependencies"), array<string>{ std::string("@elizaos/plugin-sql") }}, 
    object::pair{std::string("testDependencies"), array<string>{ std::string("@elizaos/plugin-sql") }}, 
    object::pair{std::string("actions"), array<any>{ createTodoAction, completeTodoAction, confirmTodoAction, updateTodoAction, cancelTodoAction }}, 
    object::pair{std::string("services"), array<TodoService>{ TodoService, TodoReminderService }}, 
    object::pair{std::string("routes"), std::string("routes")}, 
    object::pair{std::string("schema"), todoSchemaExport}, 
    object::pair{std::string("tests"), e2eTestSuites}, 
    object::pair{std::string("init"), [=](auto _config, auto _runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
