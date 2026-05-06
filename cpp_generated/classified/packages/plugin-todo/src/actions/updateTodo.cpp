#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/src/actions/updateTodo.h"

std::shared_ptr<Promise<std::shared_ptr<TaskSelection>>> extractTaskSelection(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<TodoData>> availableTasks)
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
        auto prompt = composePrompt(object{
            object::pair{std::string("state"), object{
                object::pair{std::string("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std::string("availableTasks"), tasksText}
            }}, 
            object::pair{std::string("template"), extractTaskTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        if (OR((!parsedResult), (type_of(parsedResult->isFound) == std::string("undefined")))) {
            logger->error(std::string("Failed to parse valid task selection information from XML"));
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
        logger->error(std::string("Error extracting task selection information:"), error);
        return object{
            object::pair{std::string("taskId"), string_empty}, 
            object::pair{std::string("taskName"), string_empty}, 
            object::pair{std::string("isFound"), false}
        };
    }
};


std::shared_ptr<Promise<any>> extractTaskUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<TodoData> task)
{
    try
    {
        auto taskDetails = std::string("Name: ") + task->name + std::string("\
");
        if (task->description) {
            taskDetails += std::string("Description: ") + task->description + std::string("\
");
        }
        taskDetails += std::string("Type: ") + task->type + std::string("\
");
        if (task->type == std::string("daily")) {
            auto recurringTag = task->tags->find([=](auto tag) mutable
            {
                return tag->startsWith(std::string("recurring-"));
            }
            );
            if (recurringTag) {
                auto recurring = const_(recurringTag->split(std::string("-")))[1];
                taskDetails += std::string("Recurring: ") + recurring + std::string("\
");
            }
            auto streak = OR((task->metadata->streak), (0));
            taskDetails += std::string("Current streak: ") + streak + std::string("\
");
        } else if (task->type == std::string("one-off")) {
            taskDetails += std::string("Priority: ") + (OR((task->priority), (4))) + std::string("\
");
            taskDetails += std::string("Urgent: ") + (task->isUrgent) ? std::string("Yes") : std::string("No") + std::string("\
");
            if (task->dueDate) {
                taskDetails += std::string("Due date: ") + const_(task->dueDate->toISOString()->split(std::string("T")))[0] + std::string("\
");
            }
        }
        auto prompt = composePrompt(object{
            object::pair{std::string("state"), object{
                object::pair{std::string("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std::string("taskDetails"), std::string("taskDetails")}
            }}, 
            object::pair{std::string("template"), extractUpdateTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("stopSequences"), array<any>()}
        }); });
        auto parsedUpdate = as<any>(parseKeyValueXml(result));
        if (OR((!parsedUpdate), (Object->keys(parsedUpdate)->get_length() == 0))) {
            logger->error(std::string("Failed to extract valid task update information from XML"));
            return nullptr;
        }
        auto finalUpdate = utils::assign(object{
        }, parsedUpdate);
        if (finalUpdate->priority) {
            auto priorityVal = parseInt(String(finalUpdate->priority), 10);
            if (AND((AND((!isNaN(priorityVal)), (priorityVal >= 1))), (priorityVal <= 4))) {
                finalUpdate->priority = as<any>(priorityVal);
            } else {
                finalUpdate.Delete("priority");
            }
        }
        if (finalUpdate->urgent != undefined) {
            finalUpdate->urgent = String(finalUpdate->urgent) == std::string("true");
        }
        if (finalUpdate->dueDate == std::string("null")) {
            finalUpdate->dueDate = nullptr;
        } else if (finalUpdate->dueDate == undefined) {
            finalUpdate.Delete("dueDate");
        } else {
            finalUpdate->dueDate = String(finalUpdate->dueDate);
        }
        if (finalUpdate->recurring) {
            auto recurringVal = String(finalUpdate->recurring);
            if ((array<string>{ std::string("daily"), std::string("weekly"), std::string("monthly") })->includes(recurringVal)) {
                finalUpdate->recurring = as<any>(recurringVal);
            } else {
                finalUpdate.Delete("recurring");
            }
        }
        if (Object->keys(finalUpdate)->get_length() == 0) {
            logger->warn(std::string("No valid update fields found after parsing XML."));
            return nullptr;
        }
        return finalUpdate;
    }
    catch (const any& error)
    {
        logger->error(std::string("Error extracting task update information:"), error);
        return nullptr;
    }
};


std::shared_ptr<Promise<std::shared_ptr<TodoData>>> applyTaskUpdate(std::shared_ptr<TodoService> dataService, std::shared_ptr<TodoData> task, std::shared_ptr<TaskUpdate> update)
{
    auto updatedTags = array<string>{ (OR((task->tags), (array<any>()))) };
    if (AND((update->recurring), (task->type == std::string("daily")))) {
        auto recurringIndex = updatedTags->findIndex([=](auto tag) mutable
        {
            return tag->startsWith(std::string("recurring-"));
        }
        );
        if (recurringIndex != -1) {
            updatedTags->splice(recurringIndex, 1);
        }
        updatedTags->push(std::string("recurring-") + update->recurring + string_empty);
    }
    auto updateData = utils::assign(object{
        , 
        object::pair{std::string("metadata"), utils::assign(object{
        }, task->metadata, ((update->recurring) ? object{
            object::pair{std::string("recurring"), update->recurring}
        } : object{}))}
    }, ((update->name) ? object{
        object::pair{std::string("name"), update->name}
    } : object{}), ((update->description != undefined) ? object{
        object::pair{std::string("description"), update->description}
    } : object{}), ((AND((update->priority != undefined), (task->type == std::string("one-off")))) ? object{
        object::pair{std::string("priority"), update->priority}
    } : object{}), ((AND((update->urgent != undefined), (task->type == std::string("one-off")))) ? object{
        object::pair{std::string("isUrgent"), update->urgent}
    } : object{}), ((update->dueDate != undefined) ? object{
        object::pair{std::string("dueDate"), (update->dueDate) ? any(std::make_shared<Date>(update->dueDate)) : any(undefined)}
    } : object{}));
    std::async([=]() { dataService->updateTodo(task->id, updateData); });
    auto updatedTask = std::async([=]() { dataService->getTodo(task->id); });
    return OR((updatedTask), (task));
};


string extractTaskTemplate = std::string("\
# Task: Extract Task Selection Information\
\
## User Message\
{{text}}\
\
## Available Tasks\
{{availableTasks}}\
\
## Instructions\
Parse the user's message to identify which task they want to update or modify.\
Match against the list of available tasks by name or description.\
If multiple tasks have similar names, choose the closest match.\
\
Return an XML object with:\
<response>\
  <taskId>ID of the task being updated, or 'null' if not found</taskId>\
  <taskName>Name of the task being updated, or 'null' if not found</taskName>\
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
string extractUpdateTemplate = std::string("\
# Task: Extract Task Update Information\
\
## User Message\
{{text}}\
\
## Current Task Details\
{{taskDetails}}\
\
## Instructions\
Parse the user's message to determine what changes they want to make to the task.\
Only include fields that the user explicitly wants to update.\
\
Return an XML object with these potential fields (only include fields that should be changed):\
<response>\
  <name>New name for the task</name>\
  <description>New description for the task</description>\
  <priority>New priority (1-4, where 1 is highest)</priority>\
  <urgent>'true' or 'false' for whether the task is urgent</urgent>\
  <dueDate>New due date in ISO format (YYYY-MM-DD), or 'null' to remove the due date</dueDate>\
  <recurring>New recurrence pattern ('daily', 'weekly', 'monthly')</recurring>\
</response>\
\
## Example Output Format\
<response>\
  <description>Updated task description</description>\
  <priority>2</priority>\
  <dueDate>2023-04-30</dueDate>\
</response>\
");
std::shared_ptr<Action> updateTodoAction = object{
    object::pair{std::string("name"), std::string("UPDATE_TODO")}, 
    object::pair{std::string("similes"), array<string>{ std::string("EDIT_TODO"), std::string("MODIFY_TASK"), std::string("CHANGE_TASK"), std::string("MODIFY_TODO"), std::string("EDIT_TASK") }}, 
    object::pair{std::string("description"), std::string("Updates an existing todo item immediately based on user description.")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        try
        {
            if (!message->roomId) {
                return false;
            }
            auto dataService = as<std::shared_ptr<TodoService>>(runtime->getService(std::string("todo")));
            auto todos = std::async([=]() { dataService->getTodos(object{
                object::pair{std::string("roomId"), message->roomId}, 
                object::pair{std::string("isCompleted"), false}
            }); });
            return todos->get_length() > 0;
        }
        catch (const any& error)
        {
            logger->error(std::string("Error validating UPDATE_TODO action:"), error);
            return false;
        }
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            if (!state) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Unable to process request without state context.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            if (!message->roomId) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I cannot update a todo without a room context.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            auto dataService = as<std::shared_ptr<TodoService>>(runtime->getService(std::string("todo")));
            auto availableTasks = std::async([=]() { dataService->getTodos(object{
                object::pair{std::string("roomId"), message->roomId}, 
                object::pair{std::string("isCompleted"), false}
            }); });
            if (availableTasks->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("You don't have any active tasks to update. Would you like to create a new task?")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_NO_TASKS") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            shared taskSelection = std::async([=]() { extractTaskSelection(runtime, message, availableTasks); });
            if (!taskSelection->isFound) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I couldn't determine which task you want to update. Could you be more specific? Here are your current tasks:\
\
") + availableTasks->map([=](auto task) mutable
                        {
                            return std::string("- ") + task->name + string_empty;
                        }
                        )->join(std::string("\
")) + string_empty}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            auto task = availableTasks->find([=](auto t) mutable
            {
                return t->id == taskSelection->taskId;
            }
            );
            if (!task) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I couldn't find a task matching "") + taskSelection->taskName + std::string("". Please try again with the exact task name.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            auto update = std::async([=]() { extractTaskUpdate(runtime, message, task); });
            if (!update) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I couldn't determine what changes you want to make to "") + task->name + std::string("". Could you please specify what you want to update, such as the name, description, priority, or due date?")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_INVALID_UPDATE") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            auto updatedTask = std::async([=]() { applyTaskUpdate(dataService, task, update); });
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("✓ Task updated: "") + updatedTask->name + std::string("" has been updated.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_SUCCESS") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in updateTodo handler:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I encountered an error while trying to update your task. Please try again.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_ERROR") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Update my taxes task to be due on April 18 instead")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✓ Task updated: "Finish taxes" has been updated.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_SUCCESS") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Change the priority of my report task to high priority and make it urgent")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✓ Task updated: "Write report" has been updated.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATE_TODO_SUCCESS") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
