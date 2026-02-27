#include "completeTodo.hpp"

std::shared_ptr<Promise<std::shared_ptr<TaskCompletion>>> extractTaskCompletion(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<TodoData>> availableTasks, std::shared_ptr<State> state)
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
            object::pair{std:("template"), extractCompletionTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std:("prompt"), std:("prompt")}, 
            object::pair{std:("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        if (OR((!parsedResult), (type_of(parsedResult->isFound) == std:("undefined")))) {
            logger->error(std:("Failed to parse valid task completion information from XML"));
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
        logger->error(std:("Error extracting task completion information:"), error);
        return object{
            object::pair{std:("taskId"), string_empty}, 
            object::pair{std:("taskName"), string_empty}, 
            object::pair{std:("isFound"), false}
        };
    }
};


string extractCompletionTemplate = std:("\
# Task: Extract Task Completion Information\
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
Parse the user's message to identify which task they're marking as completed.\
Match against the list of available tasks by name or description.\
If multiple tasks have similar names, choose the closest match.\
\
Return an XML object with:\
<response>\
  <taskId>ID of the task being completed, or 'null' if not found</taskId>\
  <taskName>Name of the task being completed, or 'null' if not found</taskName>\
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
std::shared_ptr<Action> completeTodoAction = object{
    object::pair{std:("name"), std:("COMPLETE_TODO")}, 
    object::pair{std:("similes"), array<string>{ std:("FINISH_TODO"), std:("DONE_TODO"), std:("MARK_TODO_DONE"), std:("TODO_COMPLETE") }}, 
    object::pair{std:("description"), std:("Mark a todo item as completed, including tasks done in the real world. Works with task name or ID. Can be chained with LIST_TODOS to show remaining tasks.")}, 
    object::pair{std:("validate"), [=](auto _runtime, auto _message) mutable
    {
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto options, auto callback) mutable
    {
        logger->info(std:("[completeTodo] Handler invoked"), object{
            object::pair{std:("messageContent"), message->content->text}
        });
        auto dataService = as<std::shared_ptr<TodoService>>(runtime->getService(std:("todo")));
        if (!dataService) {
            logger->error(std:("[completeTodo] Todo service not available"));
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
            auto availableTasks = std::async([=]() { dataService->getTodos(object{
                object::pair{std:("entityId"), message->entityId}, 
                object::pair{std:("roomId"), message->roomId}, 
                object::pair{std:("isCompleted"), false}
            }); });
            if (availableTasks->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("You don't have any incomplete tasks to mark as done. Would you like to create a new task?")}, 
                        object::pair{std:("actions"), array<string>{ std:("COMPLETE_TODO_NO_TASKS") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("COMPLETE_TODO")}, 
                        object::pair{std:("error"), std:("No incomplete tasks found")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("hasActiveTasks"), false}
                    }}
                };
            }
            shared taskCompletion = (options["taskId"]) ? any(object{
                object::pair{std:("taskId"), options["taskId"]}, 
                object::pair{std:("taskName"), options["taskName"]}, 
                object::pair{std:("isFound"), true}
            }) (std::async([=]() { extractTaskCompletion(runtime, message, availableTasks, state); }));
            if (!taskCompletion["isFound"]) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I couldn't determine which task you're marking as completed. Could you be more specific? Here are your current tasks:\
\
") + availableTasks->map([=](auto task) mutable
                        {
                            return std:("- ") + task->name + string_empty;
                        }
                        )->join(std:("\
")) + string_empty}, 
                        object::pair{std:("actions"), array<string>{ std:("COMPLETE_TODO_NOT_FOUND") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("COMPLETE_TODO")}, 
                        object::pair{std:("error"), std:("Could not identify which task to complete")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("availableTaskCount"), availableTasks->get_length()}
                    }}
                };
            }
            auto task = availableTasks->find([=](auto t) mutable
            {
                return t->id == taskCompletion["taskId"];
            }
            );
            if (!task) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I couldn't find a task matching "") + taskCompletion["taskName"] + std:("". Please try again with the exact task name.")}, 
                        object::pair{std:("actions"), array<string>{ std:("COMPLETE_TODO_NOT_FOUND") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("COMPLETE_TODO")}, 
                        object::pair{std:("error"), std:("Task not found: ") + taskCompletion["taskName"] + string_empty}, 
                        object::pair{std:("searchedTaskId"), taskCompletion["taskId"]}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}
                    }}
                };
            }
            std::async([=]() { dataService->updateTodo(task->id, object{
                object::pair{std:("isCompleted"), true}, 
                object::pair{std:("completedAt"), std::make_shared<Date>()}, 
                object::pair{std:("metadata"), utils::assign(object{
                    , 
                    object::pair{std:("completedAt"), ((std::make_shared<Date>()))->toISOString()}
                }, task->metadata)}
            }); });
            auto responseText = string_empty;
            if (task->type == std:("daily")) {
                responseText = std:("✅ Daily task completed: "") + task->name + std:(""");
            } else if (task->type == std:("one-off")) {
                auto completedOnTime = (task->dueDate) ? any(std::make_shared<Date>() <= task->dueDate) (true);
                auto timeStatus = (completedOnTime) ? std:("on time") : std:("late");
                auto priority = OR((task->priority), (4));
                responseText = std:("✅ Task completed: "") + task->name + std:("" (Priority ") + priority + std:(", ") + timeStatus + std:(")");
            } else if (task->type == std:("aspirational")) {
                responseText = std:("🌟 Congratulations on achieving your aspirational goal: "") + task->name + std:(""!\
\
This is a significant accomplishment.");
            } else {
                responseText = std:("✅ Marked "") + task->name + std:("" as completed.");
            }
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), responseText}, 
                    object::pair{std:("actions"), array<string>{ std:("COMPLETE_TODO") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("COMPLETE_TODO")}, 
                    object::pair{std:("taskId"), task->id}, 
                    object::pair{std:("taskName"), task->name}, 
                    object::pair{std:("taskType"), task->type}, 
                    object::pair{std:("completedAt"), ((std::make_shared<Date>()))->toISOString()}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("taskId"), task->id}, 
                    object::pair{std:("taskName"), task->name}, 
                    object::pair{std:("taskType"), task->type}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in completeTodo handler:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I encountered an error while completing your task. Please try again.")}, 
                    object::pair{std:("actions"), array<string>{ std:("COMPLETE_TODO_ERROR") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("COMPLETE_TODO")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}
                }}
            };
        }
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I completed my taxes")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Task completed: "Finish taxes" (Priority 2, on time)")}, 
            object::pair{std:("actions"), array<string>{ std:("COMPLETE_TODO") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I did my 50 pushups today")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Daily task completed: "Do 50 pushups"")}, 
            object::pair{std:("actions"), array<string>{ std:("COMPLETE_TODO") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I read three books this month")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("🌟 Congratulations on achieving your aspirational goal: "Read more books"!\
\
This is a significant accomplishment.")}, 
            object::pair{std:("actions"), array<string>{ std:("COMPLETE_TODO") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
