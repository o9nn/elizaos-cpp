#include "choice.hpp"

string optionExtractionTemplate = std:("# Task: Extract selected task and option from user message\
\
# Recent Messages:\
{{recentMessages}}\
\
# Available Tasks:\
{{#each tasks}}\
Task ID: {{taskId}} - {{name}}\
Available options:\
{{#each options}}\
- {{name}}: {{description}}\
{{/each}}\
- ABORT: Cancel this task\
\
{{/each}}\
# Instructions:\
1. Review the user's message and identify which task and option they are selecting.\
2. Match against the available tasks and their options, including ABORT.\
3. Return the task ID (shortened UUID) and selected option name exactly as listed above.\
4. If no clear selection is made, return null or empty strings for both fields.\
\
Return an XML response like this:\
<response>\
  <taskId>string | null</taskId>\
  <selectedOption>OPTION_NAME | null</selectedOption>\
</response>\
\
Your response MUST ONLY include the <response> XML block.\
");
std::shared_ptr<Action> choiceAction = object{
    object::pair{std:("name"), std:("CHOOSE_OPTION")}, 
    object::pair{std:("similes"), array<string>{ std:("SELECT_OPTION"), std:("SELECT"), std:("PICK"), std:("CHOOSE") }}, 
    object::pair{std:("description"), std:("Selects an option for a pending task that has multiple options")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state) mutable
    {
        auto pendingTasks = std::async([=]() { runtime->getTasks(object{
            object::pair{std:("roomId"), message->roomId}, 
            object::pair{std:("tags"), array<string>{ std:("AWAITING_CHOICE") }}
        }); });
        auto tasksWithOptions = OR((pendingTasks->filter([=](auto task) mutable
        {
            return task["metadata"]->options;
        }
        )), (array<any>()));
        if (tasksWithOptions["length"] == 0) {
            return false;
        }
        auto isTodoConfirmationPending = tasksWithOptions["some"]([=](auto task) mutable
        {
            return OR((OR((task["tags"]->includes(std:("todo_confirmation"))), (task["tags"]->includes(std:("todo_update"))))), (task["tags"]->includes(std:("todo_cancellation"))));
        }
        );
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback, auto responses) mutable
    {
        logger->info(std:("[choiceAction] Handler started for message: "") + message->content->text + std:("""));
        try
        {
            auto pendingTasks = std::async([=]() { runtime->getTasks(object{
                object::pair{std:("roomId"), message->roomId}, 
                object::pair{std:("tags"), array<string>{ std:("AWAITING_CHOICE") }}
            }); });
            if (!pendingTasks->length) {
                logger->warn(std:("[choiceAction] No pending tasks found in handler, though validation passed."));
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I don't see any pending choices right now.")}, 
                    object::pair{std:("actions"), array<string>{ std:("NONE") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
                return std::shared_ptr<Promise<void>>();
            }
            auto tasksWithOptions = pendingTasks->filter([=](auto task) mutable
            {
                return task["metadata"]->options;
            }
            );
            if (!tasksWithOptions->length) {
                logger->warn(std:("[choiceAction] Pending tasks found, but none have options metadata."));
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I found pending tasks, but none seem to have options to choose from.")}, 
                    object::pair{std:("actions"), array<string>{ std:("NONE") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
                return std::shared_ptr<Promise<void>>();
            }
            logger->debug(std:("[choiceAction] Found ") + tasksWithOptions->length + std:(" tasks with options."));
            shared taskId = nullptr;
            auto selectedOption = nullptr;
            shared targetTaskFullId = nullptr;
            if (tasksWithOptions->length == 1) {
                auto singleTask = const_(tasksWithOptions)[0];
                targetTaskFullId = singleTask->id;
                auto availableOptions = (OR(((as<array<any>>(singleTask->metadata->options))), (array<any>())))->map([=](auto opt) mutable
                {
                    return (type_of(opt) == std:("string")) ? object{
                        object::pair{std:("name"), opt}, 
                        object::pair{std:("description"), opt}
                    } : object{
                        object::pair{std:("name"), opt["name"]}, 
                        object::pair{std:("description"), OR((opt["description"]), (opt["name"]))}
                    };
                }
                );
                availableOptions->push(object{
                    object::pair{std:("name"), std:("ABORT")}, 
                    object::pair{std:("description"), std:("Cancel this task")}
                });
                auto simplePrompt = std:("# Task: Extract selected option for task "") + singleTask->name + std:(""\
\
## User Message:\
") + message->content->text + std:("\
\
## Available Options for this task:\
") + availableOptions->map([=](auto opt) mutable
                {
                    return std:("- ") + opt["name"] + std:(": ") + opt["description"] + string_empty;
                }
                )->join(std:("\
")) + std:("\
\
# Instructions:\
1. Review the user's message.\
2. Determine which of the Available Options the user selected (match name exactly).\
3. If no clear option is selected, return null or an empty string.\
\
Return an XML response like this:\
<response>\
  <selectedOption>OPTION_NAME | null</selectedOption>\
</response>\
\
Your response MUST ONLY include the <response> XML block.");
                logger->debug(std:("[choiceAction] Using simple prompt for single task:"), simplePrompt);
                auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
                    object::pair{std:("prompt"), simplePrompt}, 
                    object::pair{std:("stopSequences"), array<any>()}
                }); });
                auto parsed = parseKeyValueXml(result);
                logger->debug(std:("[choiceAction] Simple XML Parsing Result:"), parsed);
                if (AND((parsed), (parsed->selectedOption))) {
                    selectedOption = parsed->selectedOption;
                } else {
                    logger->warn(std:("[choiceAction] Failed to extract option even for single task."));
                }
            } else {
                auto formattedTasks = tasksWithOptions->map([=](auto task) mutable
                {
                    try
                    {
                        auto shortId = (as<string>(task["id"]))->substring(0, 8);
                        auto optionsMeta = OR(((as<array<any>>(task["metadata"]["options"]))), (array<any>()));
                        return object{
                            object::pair{std:("taskId"), shortId}, 
                            object::pair{std:("fullId"), task["id"]}, 
                            object::pair{std:("name"), task["name"]}, 
                            object::pair{std:("options"), optionsMeta->map([=](auto opt) mutable
                            {
                                return (object{
                                    object::pair{std:("name"), (type_of(opt) == std:("string")) ? any(opt) (opt["name"])}, 
                                    object::pair{std:("description"), (type_of(opt) == std:("string")) ? any(opt) (OR((opt["description"]), (opt["name"])))}
                                });
                            }
                            )}
                        };
                    }
                    catch (const any& mapError)
                    {
                        logger->error(std:("[choiceAction] Error formatting task ") + task["id"] + std:(":"), mapError);
                        return nullptr;
                    }
                }
                )->filter(Boolean);
                logger->debug(std:("[choiceAction] Formatted tasks object:"), formattedTasks);
                auto prompt = composePromptFromState(object{
                    object::pair{std:("state"), object{
                        object::pair{std:("values"), object{
                            object::pair{std:("tasks"), formattedTasks}, 
                            object::pair{std:("recentMessages"), message->content->text}
                        }}, 
                        object::pair{std:("data"), object{}}, 
                        object::pair{std:("text"), message->content->text}
                    }}, 
                    object::pair{std:("template"), optionExtractionTemplate}
                });
                auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
                    object::pair{std:("prompt"), std:("prompt")}, 
                    object::pair{std:("stopSequences"), array<any>()}
                }); });
                auto parsed = parseKeyValueXml(result);
                logger->debug(std:("[choiceAction] Multi-task XML Parsing Result:"), parsed);
                if (AND((AND((parsed), (parsed->taskId))), (parsed->selectedOption))) {
                    auto taskMap = std::make_shared<Map>(formattedTasks->map([=](auto task) mutable
                    {
                        return array<any>{ task["taskId"], task };
                    }
                    ));
                    auto taskInfo = taskMap->get(parsed->taskId);
                    if (taskInfo) {
                        taskId = parsed->taskId;
                        selectedOption = parsed->selectedOption;
                        targetTaskFullId = taskInfo->fullId;
                    } else {
                        logger->warn(std:("[choiceAction] Could not find task matching parsed shortId: ") + parsed->taskId + string_empty);
                    }
                } else {
                    logger->warn(std:("[choiceAction] Failed to parse valid taskId and selectedOption from multi-task XML response."));
                }
            }
            if (AND((targetTaskFullId), (selectedOption))) {
                logger->debug(std:("[choiceAction] Proceeding - TaskId: ") + targetTaskFullId + std:(", Option: ") + selectedOption + string_empty);
                auto selectedTask = tasksWithOptions->find([=](auto task) mutable
                {
                    return task["id"] == targetTaskFullId;
                }
                );
                if (!selectedTask) {
                    logger->error(std:("[choiceAction] Internal error: Could not re-find task with full ID ") + targetTaskFullId + string_empty);
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("An internal error occurred. Please try again.")}, 
                        object::pair{std:("actions"), array<string>{ std:("SELECT_OPTION_ERROR") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                    return std::shared_ptr<Promise<void>>();
                }
                if (selectedOption == std:("ABORT")) {
                    std::async([=]() { runtime->deleteTask(selectedTask->id); });
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("Task "") + selectedTask->name + std:("" has been cancelled.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CHOOSE_OPTION_CANCELLED") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                    return std::shared_ptr<Promise<void>>();
                }
                auto originalAction = as<string>(selectedTask->metadata->originalAction);
                auto originalMetadata = utils::assign(object{
                }, selectedTask->metadata);
                logger->debug(std:("[choiceAction] Original Action: ") + originalAction + string_empty, originalMetadata);
                if (!originalAction) {
                    logger->error(std:("Task ") + selectedTask->id + std:(" is awaiting choice but missing originalAction metadata."));
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("Error: Could not determine the original action for this choice.")}, 
                        object::pair{std:("actions"), array<string>{ std:("SELECT_OPTION_ERROR") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                    return std::shared_ptr<Promise<void>>();
                }
                auto handlerWorkerName = string_empty;
                static switch_type __switch12240_13500 = {
                    { any(std:("CREATE_TODO")), 1 },
                    { any(std:("UPDATE_TODO")), 2 },
                    { any(std:("CANCEL_TODO")), 3 },
                    { any(std:("TWITTER_POST")), 4 }
                };
                switch (__switch12240_13500[originalAction])
                {
                case 1:
                    handlerWorkerName = std:("HANDLE_TODO_CREATION_CONFIRM");
                    break;
                case 2:
                    handlerWorkerName = std:("HANDLE_TODO_UPDATE_CONFIRM");
                    break;
                case 3:
                    handlerWorkerName = std:("HANDLE_TODO_CANCELLATION_CONFIRM");
                    break;
                case 4:
                    handlerWorkerName = std:("HANDLE_TWITTER_POST_CONFIRM");
                    break;
                default:
                    logger->error(std:("No handler worker defined for originalAction: ") + originalAction + string_empty);
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("Error: Cannot process choice for action type ") + originalAction + std:(".")}, 
                        object::pair{std:("actions"), array<string>{ std:("SELECT_OPTION_ERROR") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                    return std::shared_ptr<Promise<void>>();
                }
                logger->debug(std:("[choiceAction] Determined handler worker name: ") + handlerWorkerName + string_empty);
                auto updatedTags = (OR((selectedTask->tags), (array<any>())))["filter"]([=](auto tag) mutable
                {
                    return tag != std:("AWAITING_CHOICE");
                }
                );
                updatedTags["push"](std:("CHOICE_MADE"));
                auto updatedMetadata = utils::assign(object{
                    , 
                    object::pair{std:("selectedOption"), selectedOption}
                }, originalMetadata);
                try
                {
                    std::async([=]() { runtime->updateTask(selectedTask->id, object{
                        object::pair{std:("name"), handlerWorkerName}, 
                        object::pair{std:("tags"), updatedTags}, 
                        object::pair{std:("metadata"), updatedMetadata}
                    }); });
                    logger->info(std:("[choiceAction] Successfully updated task ") + selectedTask->id + std:(" to trigger worker ") + handlerWorkerName + std:(" with option ") + selectedOption + string_empty);
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("Got it. Processing your choice: ") + selectedOption + string_empty}, 
                        object::pair{std:("actions"), array<string>{ std:("CHOOSE_OPTION_PROCESSING") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                catch (const any& updateError)
                {
                    logger->error(std:("[choiceAction] Failed to update task ") + selectedTask->id + std:(" to trigger handler:"), updateError);
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("Sorry, there was an error recording your choice. Please try again.")}, 
                        object::pair{std:("actions"), array<string>{ std:("SELECT_OPTION_ERROR") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
            } else {
                logger->warn(std:("[choiceAction] No valid selection determined. Listing options."));
                shared optionsText = std:("Please select a valid option from one of these tasks:\
\
");
                tasksWithOptions->forEach([=](auto task) mutable
                {
                    auto shortId = task["id"]["substring"](0, 8);
                    optionsText += std:("**") + task["name"] + std:("** (ID: ") + shortId + std:("):\
");
                    auto options = (OR(((as<array<any>>(task["metadata"]["options"]))), (array<any>())))->map([=](auto opt) mutable
                    {
                        return ((type_of(opt) == std:("string")) ? any(opt) (opt["name"]));
                    }
                    );
                    options->push(std:("ABORT"));
                    optionsText += options->map([=](auto opt) mutable
                    {
                        return std:("- ") + opt + string_empty;
                    }
                    )->join(std:("\
"));
                    optionsText += std:("\
\
");
                }
                );
                std::async([=]() { callback(object{
                    object::pair{std:("text"), optionsText}, 
                    object::pair{std:("actions"), array<string>{ std:("SELECT_OPTION_INVALID") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Error in select option handler:"), error);
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("There was an error processing the option selection.")}, 
                object::pair{std:("actions"), array<string>{ std:("SELECT_OPTION_ERROR") }}, 
                object::pair{std:("source"), message->content->source}
            }); });
        }
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("post")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Selected option: post for task: Confirm Twitter Post")}, 
            object::pair{std:("actions"), array<string>{ std:("CHOOSE_OPTION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I choose cancel")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Selected option: cancel for task: Confirm Twitter Post")}, 
            object::pair{std:("actions"), array<string>{ std:("CHOOSE_OPTION") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
