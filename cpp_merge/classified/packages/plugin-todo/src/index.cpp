#include "index.hpp"

std::shared_ptr<Plugin> TodoPlugin = object{
    object::pair{std:("name"), std:("todo")}, 
    object::pair{std:("description"), std:("Provides task management functionality with daily recurring and one-off tasks.")}, 
    object::pair{std:("providers"), array<any>{ todosProvider }}, 
    object::pair{std:("dependencies"), array<string>{ std:("@elizaos/plugin-sql") }}, 
    object::pair{std:("testDependencies"), array<string>{ std:("@elizaos/plugin-sql") }}, 
    object::pair{std:("actions"), array<any>{ createTodoAction, completeTodoAction, confirmTodoAction, updateTodoAction, cancelTodoAction }}, 
    object::pair{std:("services"), array<TodoService>{ TodoService, TodoReminderService }}, 
    object::pair{std:("routes"), std:("routes")}, 
    object::pair{std:("schema"), todoSchemaExport}, 
    object::pair{std:("tests"), e2eTestSuites}, 
    object::pair{std:("init"), [=](auto _config, auto _runtime) mutable
    {
    }
    }
};

void Main(void)
{
}

MAIN
