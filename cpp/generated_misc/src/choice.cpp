#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-bootstrap/actions/choice.h"

string optionExtractionTemplate = std::string("# Task: Extract selected task and option from user message\
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
    object::pair{std::string("name"), std::string("CHOOSE_OPTION")}, 
    object::pair{std::string("similes"), array<string>{ std::string("SELECT_OPTION"), std::string("SELECT"), std::string("PICK"), std::string("CHOOSE") }}, 
    object::pair{std::string("description"), std::string("Selects an option for a pending task that has multiple options")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state) mutable
    {
        auto pendingTasks = std::async([=]() { runtime->getTasks(object{
            object::pair{std::string("roomId"), message->roomId}, 
            object::pair{std::string("tags"), array<string>{ std::string("AWAITING_CHOICE") }}
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
            return OR((OR((task["tags"]->includes(std::string("todo_confirmation"))), (task["tags"]->includes(std::string("todo_update"))))), (task["tags"]->includes(std::string("todo_cancellation"))));
        }
        );
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback, auto responses) mutable
    {
        logger->info(std::string("[choiceAction] Handler started for message: "") + message->content->text + std::string("""));
        try
        {
            auto pendingTasks = std::async([=]() { runtime->getTasks(object{
                object::pair{std::string("roomId"), message->roomId}, 
                object::pair{std::string("tags"), array<string>{ std::string("AWAITING_CHOICE") }}
            }); });
            if (!pendingTasks->length) {
                logger->warn(std::string("[choiceAction] No pending tasks found in handler, though validation passed."));
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I don't see any pending choices right now.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("NONE") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
                return std::shared_ptr<Promise<void>>();
            }
            auto tasksWithOptions = pendingTasks->filter([=](auto task) mutable
            {
                return task["metadata"]->options;
            }
            );
            if (!tasksWithOptions->length) {
                logger->warn(std::string("[choiceAction] Pending tasks found, but none have options metadata."));
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I found pending tasks, but none seem to have options to choose from.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("NONE") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
                return std::shared_ptr<Promise<void>>();
            }
            logger->debug(std::string("[choiceAction] Found ") + tasksWithOptions->length + std::string(" tasks with options."));
            shared taskId = nullptr;
            auto selectedOption = nullptr;
            shared targetTaskFullId = nullptr;
            if (tasksWithOptions->length == 1) {
                auto singleTask = const_(tasksWithOptions)[0];
                targetTaskFullId = singleTask->id;
                auto availableOptions = (OR(((as<array<any>>(singleTask->metadata->options))), (array<any>())))->map([=](auto opt) mutable
                {
                    return (type_of(opt) == std::string("string")) ? object{
                        object::pair{std::string("name"), opt}, 
                        object::pair{std::string("description"), opt}
                    } : object{
                        object::pair{std::string("name"), opt["name"]}, 
                        object::pair{std::string("description"), OR((opt["description"]), (opt["name"]))}
                    };
                }
                );
                availableOptions->push(object{
                    object::pair{std::string("name"), std::string("ABORT")}, 
                    object::pair{std::string("description"), std::string("Cancel this task")}
                });
                auto simplePrompt = std::string("# Task: Extract selected option for task "") + singleTask->name + std::string(""\
\
## User Message:\
") + message->content->text + std::string("\
\
## Available Options for this task:\
") + availableOptions->map([=](auto opt) mutable
                {
                    return std::string("- ") + opt["name"] + std::string(": ") + opt["description"] + string_empty;
                }
                )->join(std::string("\
")) + std::string("\
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
                logger->debug(std::string("[choiceAction] Using simple prompt for single task:"), simplePrompt);
                auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
                    object::pair{std::string("prompt"), simplePrompt}, 
                    object::pair{std::string("stopSequences"), array<any>()}
                }); });
                auto parsed = parseKeyValueXml(result);
                logger->debug(std::string("[choiceAction] Simple XML Parsing Result:"), parsed);
                if (AND((parsed), (parsed->selectedOption))) {
                    selectedOption = parsed->selectedOption;
                } else {
                    logger->warn(std::string("[choiceAction] Failed to extract option even for single task."));
                }
            } else {
                auto formattedTasks = tasksWithOptions->map([=](auto task) mutable
                {
                    try
                    {
                        auto shortId = (as<string>(task["id"]))->substring(0, 8);
                        auto optionsMeta = OR(((as<array<any>>(task["metadata"]["options"]))), (array<any>()));
                        return object{
                            object::pair{std::string("taskId"), shortId}, 
                            object::pair{std::string("fullId"), task["id"]}, 
                            object::pair{std::string("name"), task["name"]}, 
                            object::pair{std::string("options"), optionsMeta->map([=](auto opt) mutable
                            {
                                return (object{
                                    object::pair{std::string("name"), (type_of(opt) == std::string("string")) ? any(opt) : any(opt["name"])}, 
                                    object::pair{std::string("description"), (type_of(opt) == std::string("string")) ? any(opt) : any(OR((opt["description"]), (opt["name"])))}
                                });
                            }
                            )}
                        };
                    }
                    catch (const any& mapError)
                    {
                        logger->error(std::string("[choiceAction] Error formatting task ") + task["id"] + std::string(":"), mapError);
                        return nullptr;
                    }
                }
                )->filter(Boolean);
                logger->debug(std::string("[choiceAction] Formatted tasks object:"), formattedTasks);
                auto prompt = composePromptFromState(object{
                    object::pair{std::string("state"), object{
                        object::pair{std::string("values"), object{
                            object::pair{std::string("tasks"), formattedTasks}, 
                            object::pair{std::string("recentMessages"), message->content->text}
                        }}, 
                        object::pair{std::string("data"), object{}}, 
                        object::pair{std::string("text"), message->content->text}
                    }}, 
                    object::pair{std::string("template"), optionExtractionTemplate}
                });
                auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
                    object::pair{std::string("prompt"), std::string("prompt")}, 
                    object::pair{std::string("stopSequences"), array<any>()}
                }); });
                auto parsed = parseKeyValueXml(result);
                logger->debug(std::string("[choiceAction] Multi-task XML Parsing Result:"), parsed);
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
                        logger->warn(std::string("[choiceAction] Could not find task matching parsed shortId: ") + parsed->taskId + string_empty);
                    }
                } else {
                    logger->warn(std::string("[choiceAction] Failed to parse valid taskId and selectedOption from multi-task XML response."));
                }
            }
            if (AND((targetTaskFullId), (selectedOption))) {
                logger->debug(std::string("[choiceAction] Proceeding - TaskId: ") + targetTaskFullId + std::string(", Option: ") + selectedOption + string_empty);
                auto selectedTask = tasksWithOptions->find([=](auto task) mutable
                {
                    return task["id"] == targetTaskFullId;
                }
                );
                if (!selectedTask) {
                    logger->error(std::string("[choiceAction] Internal error: Could not re-find task with full ID ") + targetTaskFullId + string_empty);
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("An internal error occurred. Please try again.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("SELECT_OPTION_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                    return std::shared_ptr<Promise<void>>();
                }
                if (selectedOption == std::string("ABORT")) {
                    std::async([=]() { runtime->deleteTask(selectedTask->id); });
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Task "") + selectedTask->name + std::string("" has been cancelled.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CHOOSE_OPTION_CANCELLED") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                    return std::shared_ptr<Promise<void>>();
                }
                auto originalAction = as<string>(selectedTask->metadata->originalAction);
                auto originalMetadata = utils::assign(object{
                }, selectedTask->metadata);
                logger->debug(std::string("[choiceAction] Original Action: ") + originalAction + string_empty, originalMetadata);
                if (!originalAction) {
                    logger->error(std::string("Task ") + selectedTask->id + std::string(" is awaiting choice but missing originalAction metadata."));
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Error: Could not determine the original action for this choice.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("SELECT_OPTION_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                    return std::shared_ptr<Promise<void>>();
                }
                auto handlerWorkerName = string_empty;
                static switch_type __switch12240_13500 = {
                    { any(std::string("CREATE_TODO")), 1 },
                    { any(std::string("UPDATE_TODO")), 2 },
                    { any(std::string("CANCEL_TODO")), 3 },
                    { any(std::string("TWITTER_POST")), 4 }
                };
                switch (__switch12240_13500[originalAction])
                {
                case 1:
                    handlerWorkerName = std::string("HANDLE_TODO_CREATION_CONFIRM");
                    break;
                case 2:
                    handlerWorkerName = std::string("HANDLE_TODO_UPDATE_CONFIRM");
                    break;
                case 3:
                    handlerWorkerName = std::string("HANDLE_TODO_CANCELLATION_CONFIRM");
                    break;
                case 4:
                    handlerWorkerName = std::string("HANDLE_TWITTER_POST_CONFIRM");
                    break;
                default:
                    logger->error(std::string("No handler worker defined for originalAction: ") + originalAction + string_empty);
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Error: Cannot process choice for action type ") + originalAction + std::string(".")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("SELECT_OPTION_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                    return std::shared_ptr<Promise<void>>();
                }
                logger->debug(std::string("[choiceAction] Determined handler worker name: ") + handlerWorkerName + string_empty);
                auto updatedTags = (OR((selectedTask->tags), (array<any>())))["filter"]([=](auto tag) mutable
                {
                    return tag != std::string("AWAITING_CHOICE");
                }
                );
                updatedTags["push"](std::string("CHOICE_MADE"));
                auto updatedMetadata = utils::assign(object{
                    , 
                    object::pair{std::string("selectedOption"), selectedOption}
                }, originalMetadata);
                try
                {
                    std::async([=]() { runtime->updateTask(selectedTask->id, object{
                        object::pair{std::string("name"), handlerWorkerName}, 
                        object::pair{std::string("tags"), updatedTags}, 
                        object::pair{std::string("metadata"), updatedMetadata}
                    }); });
                    logger->info(std::string("[choiceAction] Successfully updated task ") + selectedTask->id + std::string(" to trigger worker ") + handlerWorkerName + std::string(" with option ") + selectedOption + string_empty);
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Got it. Processing your choice: ") + selectedOption + string_empty}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CHOOSE_OPTION_PROCESSING") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                catch (const any& updateError)
                {
                    logger->error(std::string("[choiceAction] Failed to update task ") + selectedTask->id + std::string(" to trigger handler:"), updateError);
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Sorry, there was an error recording your choice. Please try again.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("SELECT_OPTION_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
            } else {
                logger->warn(std::string("[choiceAction] No valid selection determined. Listing options."));
                shared optionsText = std::string("Please select a valid option from one of these tasks:\
\
");
                tasksWithOptions->forEach([=](auto task) mutable
                {
                    auto shortId = task["id"]["substring"](0, 8);
                    optionsText += std::string("**") + task["name"] + std::string("** (ID: ") + shortId + std::string("):\
");
                    auto options = (OR(((as<array<any>>(task["metadata"]["options"]))), (array<any>())))->map([=](auto opt) mutable
                    {
                        return ((type_of(opt) == std::string("string")) ? any(opt) : any(opt["name"]));
                    }
                    );
                    options->push(std::string("ABORT"));
                    optionsText += options->map([=](auto opt) mutable
                    {
                        return std::string("- ") + opt + string_empty;
                    }
                    )->join(std::string("\
"));
                    optionsText += std::string("\
\
");
                }
                );
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), optionsText}, 
                    object::pair{std::string("actions"), array<string>{ std::string("SELECT_OPTION_INVALID") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in select option handler:"), error);
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("There was an error processing the option selection.")}, 
                object::pair{std::string("actions"), array<string>{ std::string("SELECT_OPTION_ERROR") }}, 
                object::pair{std::string("source"), message->content->source}
            }); });
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("post")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Selected option: post for task: Confirm Twitter Post")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CHOOSE_OPTION") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I choose cancel")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Selected option: cancel for task: Confirm Twitter Post")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CHOOSE_OPTION") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
