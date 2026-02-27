#include "cancelTodo.hpp"
#include <string>

std::shared_ptr<Promise<std::shared_ptr<TaskCancellation>>> extractTaskCancellation(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<TodoData>> availableTasks, std::shared_ptr<State> state)
{
    try
    {
        auto tasksText = availableTasks->map([=](auto task) mutable
        {
            return std::string("ID: ") + task->id + std::string("\
Name: ") + task->name + std::string("\
Description: ") + (OR((task->description), (task->name))) + std::string("\
Tags: ") + (OR((task->tags->join(std::string(", "))), (std::string("none")))) + std::string("\
");
        }
        )->join(std::string("\
---\
"));
        auto messageHistory = formatMessages(object{
            object::pair{std::string("messages"), OR((state->data->messages), (array<any>()))}, 
            object::pair{std::string("entities"), OR((state->data->entities), (array<any>()))}
        });
        auto prompt = composePrompt(object{
            object::pair{std::string("state"), object{
                object::pair{std::string("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std::string("availableTasks"), tasksText}, 
                object::pair{std::string("messageHistory"), std::string("messageHistory")}
            }}, 
            object::pair{std::string("template"), extractCancellationTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        logger->debug(std::string("Parsed XML Result"), parsedResult);
        if (OR((!parsedResult), (type_of(parsedResult->isFound) == std::string("std::nullopt")))) {
            logger->error(std::string("Failed to parse valid task cancellation information from XML"));
            return object{
                object::pair{std::string("taskId"), string_empty}, 
                object::pair{std::string("taskName"), string_empty}, 
                object::pair{std::string("isFound"), false}
            };
        }
        auto finalResult = object{
            object::pair{std::string("taskId"), (parsedResult->taskId == std::string("null")) ? any(string_empty) (String(OR((parsedResult->taskId), (string_empty))))}, 
            object::pair{std::string("taskName"), (parsedResult->taskName == std::string("null")) ? any(string_empty) (String(OR((parsedResult->taskName), (string_empty))))}, 
            object::pair{std::string("isFound"), String(parsedResult->isFound) == std::string("true")}
        };
        return finalResult;
    }
    catch (const any& error)
    {
        logger->error(std::string("Error extracting task cancellation information:"), error);
        return object{
            object::pair{std::string("taskId"), string_empty}, 
            object::pair{std::string("taskName"), string_empty}, 
            object::pair{std::string("isFound"), false}
        };
    }
};


string extractCancellationTemplate = std::string("\
# Task: Extract Task Cancellation Information\
\
## User Message\
{{text}}\
\
## Message History\
{{messageHistory}}\
\
## Available Tasks\
{{availableTasks}}\
\
## Instructions\
Parse the user's message to identify which task they want to cancel or delete.\
Match against the list of available tasks by name or description.\
If multiple tasks have similar names, choose the closest match.\
\
Return an XML object with:\
<response>\
  <taskId>ID of the task being cancelled, or 'null' if not found</taskId>\
  <taskName>Name of the task being cancelled, or 'null' if not found</taskName>\
  <isFound>'true' or 'false' indicating if a matching task was found</isFound>\
</response>\
\
## Example Output Format\
<response>\
  <taskId>123e4567-e89b-12d3-a456-426614174000</taskId>\
  <taskName>Finish report</taskName>\
  <isFound>true</isFound>\
</response>\
\
If no matching task was found:\
<response>\
  <taskId>null</taskId>\
  <taskName>null</taskName>\
  <isFound>false</isFound>\
</response>\
");
std::shared_ptr<Action> cancelTodoAction = object{
    object::pair{std::string("name"), std::string("CANCEL_TODO")}, 
    object::pair{std::string("similes"), array<string>{ std::string("DELETE_TODO"), std::string("REMOVE_TASK"), std::string("DELETE_TASK"), std::string("REMOVE_TODO") }}, 
    object::pair{std::string("description"), std::string("Cancels and deletes a todo item from the user's task list immediately.")}, 
    object::pair{std::string("validate"), [=](auto _runtime, auto _message) mutable
    {
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback) mutable
    {
        logger->info(std::string("[cancelTodo] Handler invoked"), object{
            object::pair{std::string("messageContent"), message->content->text}
        });
        auto dataService = as<std::shared_ptr<TodoService>>(runtime->getService(std::string("todo")));
        if (!dataService) {
            logger->error(std::string("[cancelTodo] Todo service not available"));
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("Sorry, the todo service is not available right now. Please try again later.")}, 
                    object::pair{std::string("error"), true}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("error"), std::string("Todo service not available")}
                }}
            };
        }
        try
        {
            auto allTodos = std::async([=]() { dataService->getTodos(object{
                object::pair{std::string("entityId"), message->entityId}, 
                object::pair{std::string("roomId"), message->roomId}
            }); });
            if (allTodos->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("You don't have any active tasks to cancel. Would you like to create a new task?")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CANCEL_TODO_NO_TASKS") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<object>>();
            }
            shared taskCancellation = std::async([=]() { extractTaskCancellation(runtime, message, allTodos, state); });
            if (!taskCancellation->isFound) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I couldn't determine which task you want to cancel. Could you be more specific? Here are your current tasks:\
\
") + allTodos->map([=](auto task) mutable
                        {
                            return std::string("- ") + task->name + string_empty;
                        }
                        )->join(std::string("\
")) + string_empty}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CANCEL_TODO_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<object>>();
            }
            auto task = allTodos->find([=](auto t) mutable
            {
                return t->id == taskCancellation->taskId;
            }
            );
            if (!task) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I couldn't find a task matching "") + taskCancellation->taskName + std::string("". Please try again with the exact task name.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CANCEL_TODO_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<object>>();
            }
            std::async([=]() { dataService->deleteTodo(as<std::shared_ptr<UUID>>(task->id)); });
            auto taskName = OR((task->name), (std::string("task")));
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("✓ Task cancelled: "") + taskName + std::string("" has been removed from your todo list.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("CANCEL_TODO_SUCCESS") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in cancelTodo handler:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I encountered an error while trying to cancel your task. Please try again.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("CANCEL_TODO_ERROR") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Cancel my task to finish taxes")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Are you sure you want to cancel this one-off task: "Finish taxes" (Priority 2, due 4/15/2023)? Once cancelled, it will be permanently removed.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_TODO_CONFIRM") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Yes, please cancel it")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✓ Task cancelled: "Finish taxes" has been removed from your todo list.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_TODO") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I don't want to do 50 pushups anymore, please delete that task")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Are you sure you want to cancel this daily task: "Do 50 pushups" (current streak: 3 days)? Once cancelled, it will be permanently removed.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_TODO_CONFIRM") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("No, I changed my mind, I'll keep it")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've kept your daily task "Do 50 pushups" active. Keep up the good work with your streak!")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_TODO_REJECTED") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
