#include "updateTodo.hpp"

std::shared_ptr<Promise<std::shared_ptr<TaskSelection>>> extractTaskSelection(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<Task>> availableTasks)
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
        auto prompt = composePrompt(object{
            object::pair{std:("state"), object{
                object::pair{std:("text"), message->content->text}, 
                object::pair{std:("availableTasks"), tasksText}
            }}, 
            object::pair{std:("template"), extractTaskTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std:("prompt"), std:("prompt")}, 
            object::pair{std:("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        if (OR((!parsedResult), (type_of(parsedResult->isFound) == std:("undefined")))) {
            logger->error(std:("Failed to parse valid task selection information from XML"));
            return object{
                object::pair{std:("taskId"), string_empty}, 
                object::pair{std:("taskName"), string_empty}, 
                object::pair{std:("isFound"), false}
            };
        }
        auto finalResult = object{
            object::pair{std:("taskId"), (parsedResult->taskId == std:("null")) ? any(string_empty) (OR((parsedResult->taskId), (string_empty)))}, 
            object::pair{std:("taskName"), (parsedResult->taskName == std:("null")) ? any(string_empty) (OR((parsedResult->taskName), (string_empty)))}, 
            object::pair{std:("isFound"), String(parsedResult->isFound)->toLowerCase() == std:("true")}
        };
        return finalResult;
    }
    catch (const any& error)
    {
        logger->error(std:("Error extracting task selection information:"), error);
        return object{
            object::pair{std:("taskId"), string_empty}, 
            object::pair{std:("taskName"), string_empty}, 
            object::pair{std:("isFound"), false}
        };
    }
};


std::shared_ptr<Promise<any>> extractTaskUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<Task> task)
{
    try
    {
        auto taskDetails = std:("Name: ") + task->name + std:("\
");
        if (task->description) taskDetails += std:("Description: ") + task->description + std:("\
");
        if (task->tags->includes(std:("daily"))) {
            taskDetails += std:("Type: daily\
");
            auto recurringTag = task->tags->find([=](auto tag) mutable
            {
                return tag["startsWith"](std:("recurring-"));
            }
            );
            if (recurringTag) {
                auto recurring = const_(recurringTag->split(std:("-")))[1];
                taskDetails += std:("Recurring: ") + recurring + std:("\
");
            }
            auto streak = OR((task->metadata->streak), (0));
            taskDetails += std:("Current streak: ") + streak + std:("\
");
        } else if (task->tags->includes(std:("one-off"))) {
            taskDetails += std:("Type: one-off\
");
            auto priorityTag = task->tags->find([=](auto tag) mutable
            {
                return tag["startsWith"](std:("priority-"));
            }
            );
            if (priorityTag) {
                auto priority = const_(priorityTag->split(std:("-")))[1];
                taskDetails += std:("Priority: ") + priority + std:("\
");
            }
            taskDetails += std:("Urgent: ") + (task->tags->includes(std:("urgent"))) ? std:("Yes") : std:("No") + std:("\
");
            if (task->metadata->dueDate) {
                auto dueDate = std::make_shared<Date>(String(task->metadata->dueDate));
                taskDetails += std:("Due date: ") + const_(dueDate->toISOString()->split(std:("T")))[0] + std:("\
");
            }
        } else if (task->tags->includes(std:("aspirational"))) {
            taskDetails += std:("Type: aspirational goal\
");
        }
        auto prompt = composePrompt(object{
            object::pair{std:("state"), object{
                object::pair{std:("text"), message->content->text}, 
                object::pair{std:("taskDetails"), std:("taskDetails")}
            }}, 
            object::pair{std:("template"), extractUpdateTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std:("prompt"), std:("prompt")}, 
            object::pair{std:("stopSequences"), array<any>()}
        }); });
        auto parsedUpdate = as<any>(parseKeyValueXml(result));
        if (OR((!parsedUpdate), (Object->keys(parsedUpdate)->get_length() == 0))) {
            logger->error(std:("Failed to extract valid task update information from XML"));
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
        if (finalUpdate->urgent != undefined) finalUpdate->urgent = String(finalUpdate->urgent)->toLowerCase() == std:("true");
        if (finalUpdate->dueDate == std:("null")) finalUpdate->dueDate = nullptr; else if (finalUpdate->dueDate == undefined) finalUpdate.Delete("dueDate"); else finalUpdate->dueDate = String(finalUpdate->dueDate);
        if (finalUpdate->recurring) {
            auto recurringVal = String(finalUpdate->recurring)->toLowerCase();
            if ((array<string>{ std:("daily"), std:("weekly"), std:("monthly") })->includes(recurringVal)) {
                finalUpdate->recurring = as<any>(recurringVal);
            } else {
                finalUpdate.Delete("recurring");
            }
        }
        if (Object->keys(finalUpdate)->get_length() == 0) {
            logger->warn(std:("No valid update fields found after parsing XML."));
            return nullptr;
        }
        return finalUpdate;
    }
    catch (const any& error)
    {
        logger->error(std:("Error extracting task update information:"), error);
        return nullptr;
    }
};


std::shared_ptr<Promise<std::shared_ptr<Task>>> applyTaskUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Task> task, std::shared_ptr<TaskUpdate> update)
{
    auto updatedTags = array<any>{ (OR((task->tags), (array<any>()))) };
    auto updatedMetadata = utils::assign(object{
    }, (OR((task->metadata), (object{}))));
    auto updatedTask = utils::assign(object{
    }, ((update->name) ? object{
        object::pair{std:("name"), update->name}
    } : object{}), ((update->description) ? object{
        object::pair{std:("description"), update->description}
    } : object{}));
    if (AND((update->priority), (task->tags->includes(std:("one-off"))))) {
        auto priorityIndex = updatedTags->findIndex([=](auto tag) mutable
        {
            return tag["startsWith"](std:("priority-"));
        }
        );
        if (priorityIndex != -1) {
            updatedTags->splice(priorityIndex, 1);
        }
        updatedTags->push(std:("priority-") + update->priority + string_empty);
    }
    if (AND((update->urgent != undefined), (task->tags->includes(std:("one-off"))))) {
        auto urgentIndex = updatedTags->indexOf(std:("urgent"));
        if (urgentIndex != -1) {
            updatedTags->splice(urgentIndex, 1);
        }
        if (update->urgent) {
            updatedTags->push(std:("urgent"));
        }
    }
    if (AND((update->recurring), (task->tags->includes(std:("daily"))))) {
        auto recurringIndex = updatedTags->findIndex([=](auto tag) mutable
        {
            return tag["startsWith"](std:("recurring-"));
        }
        );
        if (recurringIndex != -1) {
            updatedTags->splice(recurringIndex, 1);
        }
        updatedTags->push(std:("recurring-") + update->recurring + string_empty);
    }
    if (update->dueDate != undefined) {
        if (update->dueDate == nullptr) {
            updatedMetadata.Delete("dueDate");
        } else {
            updatedMetadata["dueDate"] = update->dueDate;
        }
    }
    std::async([=]() { runtime->updateTask(task->id, utils::assign(object{
        , 
        object::pair{std:("tags"), updatedTags}, 
        object::pair{std:("metadata"), updatedMetadata}
    }, updatedTask)); });
    auto updatedTaskResult = std::async([=]() { runtime->getTask(task->id); });
    return updatedTaskResult;
};


string extractTaskTemplate = std:("\
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
string extractUpdateTemplate = std:("\
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
    object::pair{std:("name"), std:("UPDATE_TODO")}, 
    object::pair{std:("similes"), array<string>{ std:("EDIT_TODO"), std:("MODIFY_TASK"), std:("CHANGE_TASK"), std:("MODIFY_TODO"), std:("EDIT_TASK") }}, 
    object::pair{std:("description"), std:("Updates an existing todo item immediately based on user description.")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        try
        {
            auto tasks = std::async([=]() { runtime->getTasks(object{
                object::pair{std:("roomId"), message->roomId}, 
                object::pair{std:("tags"), array<string>{ std:("TODO") }}
            }); });
            auto activeTasks = tasks->filter([=](auto task) mutable
            {
                return !task["tags"]->includes(std:("completed"));
            }
            );
            return activeTasks->length > 0;
        }
        catch (const any& error)
        {
            logger->error(std:("Error validating UPDATE_TODO action:"), error);
            return false;
        }
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto options, auto callback) mutable
    {
        try
        {
            auto tasks = std::async([=]() { runtime->getTasks(object{
                object::pair{std:("roomId"), message->roomId}, 
                object::pair{std:("tags"), array<string>{ std:("TODO") }}
            }); });
            auto availableTasks = tasks->filter([=](auto task) mutable
            {
                return !task["tags"]->includes(std:("completed"));
            }
            );
            if (availableTasks->length == 0) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("You don't have any active tasks to update. Would you like to create a new task?")}, 
                    object::pair{std:("actions"), array<string>{ std:("UPDATE_TODO_NO_TASKS") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
                return std::shared_ptr<Promise<void>>();
            }
            shared taskSelection = std::async([=]() { extractTaskSelection(runtime, message, availableTasks); });
            if (!taskSelection->isFound) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I couldn't determine which task you want to update. Could you be more specific? Here are your current tasks:\
\
") + availableTasks->map([=](auto task) mutable
                    {
                        return std:("- ") + task["name"] + string_empty;
                    }
                    )->join(std:("\
"))}, 
                    object::pair{std:("actions"), array<string>{ std:("UPDATE_TODO_NOT_FOUND") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
                return std::shared_ptr<Promise<void>>();
            }
            auto task = availableTasks->find([=](auto t) mutable
            {
                return t["id"] == taskSelection->taskId;
            }
            );
            if (!task) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I couldn't find a task matching "") + taskSelection->taskName + std:("". Please try again with the exact task name.")}, 
                    object::pair{std:("actions"), array<string>{ std:("UPDATE_TODO_NOT_FOUND") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
                return std::shared_ptr<Promise<void>>();
            }
            auto update = std::async([=]() { extractTaskUpdate(runtime, message, task); });
            if (!update) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I couldn't determine what changes you want to make to "") + task->name + std:("". Could you please specify what you want to update, such as the name, description, priority, or due date?")}, 
                    object::pair{std:("actions"), array<string>{ std:("UPDATE_TODO_INVALID_UPDATE") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
                return std::shared_ptr<Promise<void>>();
            }
            auto updatedTask = std::async([=]() { applyTaskUpdate(runtime, task, update); });
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("✓ Task updated: "") + updatedTask->name + std:("" has been updated.")}, 
                object::pair{std:("actions"), array<string>{ std:("UPDATE_TODO_SUCCESS") }}, 
                object::pair{std:("source"), message->content->source}
            }); });
        }
        catch (const any& error)
        {
            logger->error(std:("Error in updateTodo handler:"), error);
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("I encountered an error while trying to update your task. Please try again.")}, 
                object::pair{std:("actions"), array<string>{ std:("UPDATE_TODO_ERROR") }}, 
                object::pair{std:("source"), message->content->source}
            }); });
        }
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Update my taxes task to be due on April 18 instead")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✓ Task updated: "Finish taxes" has been updated.")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATE_TODO_SUCCESS") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Change the priority of my report task to high priority and make it urgent")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✓ Task updated: "Write report" has been updated.")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATE_TODO_SUCCESS") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
