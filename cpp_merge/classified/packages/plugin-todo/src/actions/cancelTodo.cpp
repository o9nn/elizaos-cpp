#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/actions/cancelTodo.h"

std::shared_ptr<Promise<std::shared_ptr<TaskCancellation>>> extractTaskCancellation(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<TodoData>> availableTasks, std::shared_ptr<State> state)
{
    try
    {
        auto tasksText = availableTasks->map([=](auto task) mutable
        {
            return std:("ID: ") + task->id + std:("\
Name: ") + task->name + std:("\
Description: ") + (OR((task->description), (task->name))) + std:("\
Tags: ") + (OR((task->tags->join(std:(", "))), (std:("none")))) + std:("\
");
        }
        )->join(std:("\
---\
"));
        auto messageHistory = formatMessages(object{
            object::pair{std:("messages"), OR((state->data->messages), (array<any>()))}, 
            object::pair{std:("entities"), OR((state->data->entities), (array<any>()))}
        });
        auto prompt = composePrompt(object{
            object::pair{std:("state"), object{
                object::pair{std:("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std:("availableTasks"), tasksText}, 
                object::pair{std:("messageHistory"), std:("messageHistory")}
            }}, 
            object::pair{std:("template"), extractCancellationTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std:("prompt"), std:("prompt")}, 
            object::pair{std:("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        logger->debug(std:("Parsed XML Result"), parsedResult);
        if (OR((!parsedResult), (type_of(parsedResult->isFound) == std:("undefined")))) {
            logger->error(std:("Failed to parse valid task cancellation information from XML"));
            return object{
                object::pair{std:("taskId"), string_empty}, 
                object::pair{std:("taskName"), string_empty}, 
                object::pair{std:("isFound"), false}
            };
        }
        auto finalResult = object{
            object::pair{std:("taskId"), (parsedResult->taskId == std:("null")) ? any(string_empty) (String(OR((parsedResult->taskId), (string_empty))))}, 
            object::pair{std:("taskName"), (parsedResult->taskName == std:("null")) ? any(string_empty) (String(OR((parsedResult->taskName), (string_empty))))}, 
            object::pair{std:("isFound"), String(parsedResult->isFound) == std:("true")}
        };
        return finalResult;
    }
    catch (const any& error)
    {
        logger->error(std:("Error extracting task cancellation information:"), error);
        return object{
            object::pair{std:("taskId"), string_empty}, 
            object::pair{std:("taskName"), string_empty}, 
            object::pair{std:("isFound"), false}
        };
    }
};


string extractCancellationTemplate = std:("\
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
    object::pair{std:("name"), std:("CANCEL_TODO")}, 
    object::pair{std:("similes"), array<string>{ std:("DELETE_TODO"), std:("REMOVE_TASK"), std:("DELETE_TASK"), std:("REMOVE_TODO") }}, 
    object::pair{std:("description"), std:("Cancels and deletes a todo item from the user's task list immediately.")}, 
    object::pair{std:("validate"), [=](auto _runtime, auto _message) mutable
    {
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback) mutable
    {
        logger->info(std:("[cancelTodo] Handler invoked"), object{
            object::pair{std:("messageContent"), message->content->text}
        });
        auto dataService = as<std::shared_ptr<TodoService>>(runtime->getService(std:("todo")));
        if (!dataService) {
            logger->error(std:("[cancelTodo] Todo service not available"));
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("Sorry, the todo service is not available right now. Please try again later.")}, 
                    object::pair{std:("error"), true}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("values"), object{
                    object::pair{std:("error"), std:("Todo service not available")}
                }}
            };
        }
        try
        {
            auto allTodos = std::async([=]() { dataService->getTodos(object{
                object::pair{std:("entityId"), message->entityId}, 
                object::pair{std:("roomId"), message->roomId}
            }); });
            if (allTodos->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("You don't have any active tasks to cancel. Would you like to create a new task?")}, 
                        object::pair{std:("actions"), array<string>{ std:("CANCEL_TODO_NO_TASKS") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<object>>();
            }
            shared taskCancellation = std::async([=]() { extractTaskCancellation(runtime, message, allTodos, state); });
            if (!taskCancellation->isFound) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I couldn't determine which task you want to cancel. Could you be more specific? Here are your current tasks:\
\
") + allTodos->map([=](auto task) mutable
                        {
                            return std:("- ") + task->name + string_empty;
                        }
                        )->join(std:("\
")) + string_empty}, 
                        object::pair{std:("actions"), array<string>{ std:("CANCEL_TODO_NOT_FOUND") }}, 
                        object::pair{std:("source"), message->content->source}
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
                        object::pair{std:("text"), std:("I couldn't find a task matching "") + taskCancellation->taskName + std:("". Please try again with the exact task name.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CANCEL_TODO_NOT_FOUND") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<object>>();
            }
            std::async([=]() { dataService->deleteTodo(as<std::shared_ptr<UUID>>(task->id)); });
            auto taskName = OR((task->name), (std:("task")));
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("✓ Task cancelled: "") + taskName + std:("" has been removed from your todo list.")}, 
                    object::pair{std:("actions"), array<string>{ std:("CANCEL_TODO_SUCCESS") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Error in cancelTodo handler:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I encountered an error while trying to cancel your task. Please try again.")}, 
                    object::pair{std:("actions"), array<string>{ std:("CANCEL_TODO_ERROR") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
        }
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Cancel my task to finish taxes")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Are you sure you want to cancel this one-off task: "Finish taxes" (Priority 2, due 4/15/2023)? Once cancelled, it will be permanently removed.")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_TODO_CONFIRM") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Yes, please cancel it")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✓ Task cancelled: "Finish taxes" has been removed from your todo list.")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_TODO") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I don't want to do 50 pushups anymore, please delete that task")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Are you sure you want to cancel this daily task: "Do 50 pushups" (current streak: 3 days)? Once cancelled, it will be permanently removed.")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_TODO_CONFIRM") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("No, I changed my mind, I'll keep it")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've kept your daily task "Do 50 pushups" active. Keep up the good work with your streak!")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_TODO_REJECTED") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
