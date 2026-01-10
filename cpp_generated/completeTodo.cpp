#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/actions/completeTodo.h"

std::shared_ptr<Promise<std::shared_ptr<TaskCompletion>>> extractTaskCompletion(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<TodoData>> availableTasks, std::shared_ptr<State> state)
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
            object::pair{std::string("template"), extractCompletionTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        if (OR((!parsedResult), (type_of(parsedResult->isFound) == std::string("undefined")))) {
            logger->error(std::string("Failed to parse valid task completion information from XML"));
            return object{
                object::pair{std::string("taskId"), string_empty}, 
                object::pair{std::string("taskName"), string_empty}, 
                object::pair{std::string("isFound"), false}
            };
        }
        auto finalResult = object{
            object::pair{std::string("taskId"), (parsedResult->taskId == std::string("null")) ? any(string_empty) : any(String(OR((parsedResult->taskId), (string_empty))))}, 
            object::pair{std::string("taskName"), (parsedResult->taskName == std::string("null")) ? any(string_empty) : any(String(OR((parsedResult->taskName), (string_empty))))}, 
            object::pair{std::string("isFound"), String(parsedResult->isFound) == std::string("true")}
        };
        return finalResult;
    }
    catch (const any& error)
    {
        logger->error(std::string("Error extracting task completion information:"), error);
        return object{
            object::pair{std::string("taskId"), string_empty}, 
            object::pair{std::string("taskName"), string_empty}, 
            object::pair{std::string("isFound"), false}
        };
    }
};


string extractCompletionTemplate = std::string("\
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
    object::pair{std::string("name"), std::string("COMPLETE_TODO")}, 
    object::pair{std::string("similes"), array<string>{ std::string("FINISH_TODO"), std::string("DONE_TODO"), std::string("MARK_TODO_DONE"), std::string("TODO_COMPLETE") }}, 
    object::pair{std::string("description"), std::string("Mark a todo item as completed, including tasks done in the real world. Works with task name or ID. Can be chained with LIST_TODOS to show remaining tasks.")}, 
    object::pair{std::string("validate"), [=](auto _runtime, auto _message) mutable
    {
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto options, auto callback) mutable
    {
        logger->info(std::string("[completeTodo] Handler invoked"), object{
            object::pair{std::string("messageContent"), message->content->text}
        });
        auto dataService = as<std::shared_ptr<TodoService>>(runtime->getService(std::string("todo")));
        if (!dataService) {
            logger->error(std::string("[completeTodo] Todo service not available"));
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
            auto availableTasks = std::async([=]() { dataService->getTodos(object{
                object::pair{std::string("entityId"), message->entityId}, 
                object::pair{std::string("roomId"), message->roomId}, 
                object::pair{std::string("isCompleted"), false}
            }); });
            if (availableTasks->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("You don't have any incomplete tasks to mark as done. Would you like to create a new task?")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("COMPLETE_TODO_NO_TASKS") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("COMPLETE_TODO")}, 
                        object::pair{std::string("error"), std::string("No incomplete tasks found")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("hasActiveTasks"), false}
                    }}
                };
            }
            shared taskCompletion = (options["taskId"]) ? any(object{
                object::pair{std::string("taskId"), options["taskId"]}, 
                object::pair{std::string("taskName"), options["taskName"]}, 
                object::pair{std::string("isFound"), true}
            }) : any(std::async([=]() { extractTaskCompletion(runtime, message, availableTasks, state); }));
            if (!taskCompletion["isFound"]) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I couldn't determine which task you're marking as completed. Could you be more specific? Here are your current tasks:\
\
") + availableTasks->map([=](auto task) mutable
                        {
                            return std::string("- ") + task->name + string_empty;
                        }
                        )->join(std::string("\
")) + string_empty}, 
                        object::pair{std::string("actions"), array<string>{ std::string("COMPLETE_TODO_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("COMPLETE_TODO")}, 
                        object::pair{std::string("error"), std::string("Could not identify which task to complete")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("availableTaskCount"), availableTasks->get_length()}
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
                        object::pair{std::string("text"), std::string("I couldn't find a task matching "") + taskCompletion["taskName"] + std::string("". Please try again with the exact task name.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("COMPLETE_TODO_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("COMPLETE_TODO")}, 
                        object::pair{std::string("error"), std::string("Task not found: ") + taskCompletion["taskName"] + string_empty}, 
                        object::pair{std::string("searchedTaskId"), taskCompletion["taskId"]}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}
                    }}
                };
            }
            std::async([=]() { dataService->updateTodo(task->id, object{
                object::pair{std::string("isCompleted"), true}, 
                object::pair{std::string("completedAt"), std::make_shared<Date>()}, 
                object::pair{std::string("metadata"), utils::assign(object{
                    , 
                    object::pair{std::string("completedAt"), ((std::make_shared<Date>()))->toISOString()}
                }, task->metadata)}
            }); });
            auto responseText = string_empty;
            if (task->type == std::string("daily")) {
                responseText = std::string("✅ Daily task completed: "") + task->name + std::string(""");
            } else if (task->type == std::string("one-off")) {
                auto completedOnTime = (task->dueDate) ? any(std::make_shared<Date>() <= task->dueDate) : any(true);
                auto timeStatus = (completedOnTime) ? std::string("on time") : std::string("late");
                auto priority = OR((task->priority), (4));
                responseText = std::string("✅ Task completed: "") + task->name + std::string("" (Priority ") + priority + std::string(", ") + timeStatus + std::string(")");
            } else if (task->type == std::string("aspirational")) {
                responseText = std::string("🌟 Congratulations on achieving your aspirational goal: "") + task->name + std::string(""!\
\
This is a significant accomplishment.");
            } else {
                responseText = std::string("✅ Marked "") + task->name + std::string("" as completed.");
            }
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), responseText}, 
                    object::pair{std::string("actions"), array<string>{ std::string("COMPLETE_TODO") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("COMPLETE_TODO")}, 
                    object::pair{std::string("taskId"), task->id}, 
                    object::pair{std::string("taskName"), task->name}, 
                    object::pair{std::string("taskType"), task->type}, 
                    object::pair{std::string("completedAt"), ((std::make_shared<Date>()))->toISOString()}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("taskId"), task->id}, 
                    object::pair{std::string("taskName"), task->name}, 
                    object::pair{std::string("taskType"), task->type}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in completeTodo handler:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I encountered an error while completing your task. Please try again.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("COMPLETE_TODO_ERROR") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("COMPLETE_TODO")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}
                }}
            };
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I completed my taxes")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Task completed: "Finish taxes" (Priority 2, on time)")}, 
            object::pair{std::string("actions"), array<string>{ std::string("COMPLETE_TODO") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I did my 50 pushups today")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Daily task completed: "Do 50 pushups"")}, 
            object::pair{std::string("actions"), array<string>{ std::string("COMPLETE_TODO") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I read three books this month")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("🌟 Congratulations on achieving your aspirational goal: "Read more books"!\
\
This is a significant accomplishment.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("COMPLETE_TODO") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
