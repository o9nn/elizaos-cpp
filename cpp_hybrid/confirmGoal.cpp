#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/actions/confirmGoal.h"

std::shared_ptr<Promise<std::shared_ptr<ConfirmationResponse>>> extractConfirmationIntent(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, any pendingTask, std::shared_ptr<State> state)
{
    try
    {
        if (!pendingTask) {
            return object{
                object::pair{std::string("isConfirmation"), false}, 
                object::pair{std::string("shouldProceed"), false}
            };
        }
        auto messageHistory = formatMessages(object{
            object::pair{std::string("messages"), OR((state->data->messages), (array<any>()))}, 
            object::pair{std::string("entities"), OR((state->data->entities), (array<any>()))}
        });
        auto pendingTaskText = std::string("\
Name: ") + pendingTask->name + std::string("\
Type: ") + pendingTask->taskType + std::string("\
") + (pendingTask->priority) ? any(std::string("Priority: ") + pendingTask->priority + string_empty) : any(string_empty) + std::string("\
") + (pendingTask->urgent) ? std::string("Urgent: Yes") : string_empty + std::string("\
") + (pendingTask->dueDate) ? any(std::string("Due Date: ") + pendingTask->dueDate + string_empty) : any(string_empty) + std::string("\
") + (pendingTask->recurring) ? any(std::string("Recurring: ") + pendingTask->recurring + string_empty) : any(string_empty) + std::string("\
");
        auto prompt = composePrompt(object{
            object::pair{std::string("state"), object{
                object::pair{std::string("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std::string("messageHistory"), std::string("messageHistory")}, 
                object::pair{std::string("pendingTask"), pendingTaskText}
            }}, 
            object::pair{std::string("template"), extractConfirmationTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        if (!parsedResult) {
            logger->error(std::string("Failed to parse confirmation response"));
            return object{
                object::pair{std::string("isConfirmation"), false}, 
                object::pair{std::string("shouldProceed"), false}
            };
        }
        return object{
            object::pair{std::string("isConfirmation"), String(parsedResult->isConfirmation) == std::string("true")}, 
            object::pair{std::string("shouldProceed"), String(parsedResult->shouldProceed) == std::string("true")}, 
            object::pair{std::string("modifications"), (parsedResult->modifications == std::string("none")) ? any(undefined) : any(parsedResult->modifications)}
        };
    }
    catch (const any& error)
    {
        logger->error(std::string("Error extracting confirmation intent:"), error);
        return object{
            object::pair{std::string("isConfirmation"), false}, 
            object::pair{std::string("shouldProceed"), false}
        };
    }
};


string extractConfirmationTemplate = std::string("\
# Task: Extract Confirmation Intent\
\
## User Message\
{{text}}\
\
## Message History\
{{messageHistory}}\
\
## Pending Task Details\
{{pendingTask}}\
\
## Instructions\
Determine if the user is confirming, rejecting, or modifying the pending task creation.\
Look for:\
- Affirmative responses (yes, confirm, ok, do it, go ahead, etc.)\
- Negative responses (no, cancel, nevermind, stop, etc.)\
- Modification requests (change X to Y, make it priority 1, etc.)\
\
Return an XML object with:\
<response>\
  <isConfirmation>true/false - whether this is a response to the pending task</isConfirmation>\
  <shouldProceed>true/false - whether to create the task</shouldProceed>\
  <modifications>Any requested changes to the task, or 'none'</modifications>\
</response>\
\
## Example Output\
<response>\
  <isConfirmation>true</isConfirmation>\
  <shouldProceed>true</shouldProceed>\
  <modifications>none</modifications>\
</response>\
");
std::shared_ptr<Action> confirmGoalAction = object{
    object::pair{std::string("name"), std::string("CONFIRM_GOAL")}, 
    object::pair{std::string("similes"), array<string>{ std::string("CONFIRM_TASK"), std::string("APPROVE_GOAL"), std::string("APPROVE_TASK"), std::string("GOAL_CONFIRM") }}, 
    object::pair{std::string("description"), std::string("Confirms or cancels a pending goal creation after user review. Can be chained with LIST_GOALS to see updated list or UPDATE_GOAL to modify the confirmed goal.")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto pendingGoal = as<any>(state->data->pendingGoal);
        return !!pendingGoal;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto options, auto callback = undefined) mutable
    {
        try
        {
            if (!state) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Unable to process confirmation without state context.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("text"), std::string("Unable to process confirmation without state context.")}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CONFIRM_GOAL")}, 
                        object::pair{std::string("error"), std::string("missing_state_context")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("missing_state_context")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            shared pendingGoal = as<any>(state["data"]["pendingGoal"]);
            if (!pendingGoal) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I don't have a pending task to confirm. Would you like to create a new task?")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_NO_PENDING") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("text"), std::string("I don't have a pending task to confirm. Would you like to create a new task?")}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CONFIRM_GOAL")}, 
                        object::pair{std::string("error"), std::string("no_pending_goal")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("no_pending_goal")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            if (OR((!message->roomId), (!message->entityId))) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I cannot confirm a goal without a room and entity context.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("text"), std::string("I cannot confirm a goal without a room and entity context.")}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CONFIRM_GOAL")}, 
                        object::pair{std::string("error"), std::string("missing_context")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("missing_context")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            auto confirmation = std::async([=]() { extractConfirmationIntent(runtime, message, pendingGoal, state); });
            if (!confirmation->isConfirmation) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I'm still waiting for your confirmation on the task "") + pendingGoal->name + std::string("". Would you like me to create it?")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_WAITING") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("text"), std::string("I'm still waiting for your confirmation on the task "") + pendingGoal->name + std::string("". Would you like me to create it?")}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CONFIRM_GOAL")}, 
                        object::pair{std::string("status"), std::string("waiting_for_confirmation")}, 
                        object::pair{std::string("pendingGoalName"), pendingGoal->name}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("awaiting_confirmation"), true}, 
                        object::pair{std::string("goalName"), pendingGoal->name}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            if (!confirmation->shouldProceed) {
                state["data"].Delete("pendingGoal");
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Okay, I've cancelled the task creation. Let me know if you'd like to create a different task.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_CANCELLED") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("text"), std::string("Okay, I've cancelled the task creation. Let me know if you'd like to create a different task.")}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CONFIRM_GOAL")}, 
                        object::pair{std::string("status"), std::string("cancelled")}, 
                        object::pair{std::string("cancelledGoalName"), pendingGoal->name}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), true}, 
                        object::pair{std::string("goalCancelled"), true}, 
                        object::pair{std::string("goalName"), pendingGoal->name}
                    }}, 
                    object::pair{std::string("success"), true}
                };
            }
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std::string("goals")));
            auto existingGoals = std::async([=]() { dataService->getGoals(object{
                object::pair{std::string("ownerId"), message->entityId}, 
                object::pair{std::string("ownerType"), std::string("entity")}, 
                object::pair{std::string("isCompleted"), false}
            }); });
            auto duplicateGoal = existingGoals->find([=](auto g) mutable
            {
                return g->name->trim() == pendingGoal->name->trim();
            }
            );
            if (duplicateGoal) {
                state["data"].Delete("pendingGoal");
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("It looks like you already have an active goal named "") + pendingGoal->name + std::string("". I haven't added a duplicate.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_DUPLICATE") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("text"), std::string("It looks like you already have an active goal named "") + pendingGoal->name + std::string("". I haven't added a duplicate.")}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CONFIRM_GOAL")}, 
                        object::pair{std::string("error"), std::string("duplicate_goal")}, 
                        object::pair{std::string("duplicateGoalName"), pendingGoal->name}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("duplicate_goal")}, 
                        object::pair{std::string("goalName"), pendingGoal->name}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            auto createdGoalId = std::async([=]() { dataService->createGoal(object{
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("ownerType"), std::string("entity")}, 
                object::pair{std::string("ownerId"), message->entityId}, 
                object::pair{std::string("name"), pendingGoal->name}, 
                object::pair{std::string("description"), OR((pendingGoal->description), (pendingGoal->name))}, 
                object::pair{std::string("metadata"), utils::assign(object{
                    , 
                    object::pair{std::string("taskType"), pendingGoal->taskType}, 
                    object::pair{std::string("priority"), pendingGoal->priority}, 
                    object::pair{std::string("urgent"), pendingGoal->urgent}, 
                    object::pair{std::string("dueDate"), pendingGoal->dueDate}, 
                    object::pair{std::string("recurring"), pendingGoal->recurring}
                }, pendingGoal->metadata)}, 
                object::pair{std::string("tags"), OR((pendingGoal->tags), (array<any>()))}
            }); });
            if (!createdGoalId) {
                throw any(std::make_shared<Error>(std::string("Failed to create goal")));
            }
            state["data"].Delete("pendingGoal");
            auto successMessage = string_empty;
            if (pendingGoal->taskType == std::string("daily")) {
                successMessage = std::string("✅ Created daily task: "") + pendingGoal->name + std::string("".");
            } else if (pendingGoal->taskType == std::string("one-off")) {
                auto priorityText = std::string("Priority ") + (OR((pendingGoal->priority), (3))) + string_empty;
                auto urgentText = (pendingGoal->urgent) ? std::string(", Urgent") : string_empty;
                auto dueDateText = (pendingGoal->dueDate) ? any(std::string(", Due: ") + ((std::make_shared<Date>(pendingGoal->dueDate)))->toLocaleDateString() + string_empty) : any(string_empty);
                successMessage = std::string("✅ Created task: "") + pendingGoal->name + std::string("" (") + priorityText + string_empty + urgentText + string_empty + dueDateText + std::string(")");
            } else {
                successMessage = std::string("✅ Created aspirational goal: "") + pendingGoal->name + std::string(""");
            }
            if (confirmation->modifications) {
                successMessage += std::string("\
\
Note: I created the task as originally described. The modifications you mentioned ("") + confirmation->modifications + std::string("") weren't applied. You can use UPDATE_GOAL to make changes.");
            }
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), successMessage}, 
                    object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_SUCCESS") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std::string("text"), successMessage}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("CONFIRM_GOAL")}, 
                    object::pair{std::string("status"), std::string("success")}, 
                    object::pair{std::string("goalId"), createdGoalId}, 
                    object::pair{std::string("goalName"), pendingGoal->name}, 
                    object::pair{std::string("taskType"), pendingGoal->taskType}, 
                    object::pair{std::string("priority"), pendingGoal->priority}, 
                    object::pair{std::string("urgent"), pendingGoal->urgent}, 
                    object::pair{std::string("dueDate"), pendingGoal->dueDate}, 
                    object::pair{std::string("modifications"), confirmation->modifications}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("goalCreated"), true}, 
                    object::pair{std::string("goalId"), createdGoalId}, 
                    object::pair{std::string("goalName"), pendingGoal->name}
                }}, 
                object::pair{std::string("success"), true}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in confirmGoal handler:"), error);
            auto errorMessage = std::string("I encountered an error while confirming your goal. Please try again.");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), errorMessage}, 
                    object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_ERROR") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std::string("text"), errorMessage}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("CONFIRM_GOAL")}, 
                    object::pair{std::string("error"), std::string("execution_error")}, 
                    object::pair{std::string("errorDetails"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("errorType"), std::string("execution_error")}, 
                    object::pair{std::string("hasError"), true}
                }}, 
                object::pair{std::string("success"), false}
            };
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Add a goal to finish my taxes by April 15 and show me all my goals after")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll create a one-off goal: 'Finish taxes' with Priority 2, Due April 15. After you confirm, I'll show you all your goals.")}, 
            object::pair{std::string("thought"), std::string("The user wants to create a goal with confirmation and then see their complete goal list. I need to chain CREATE_GOAL, CONFIRM_GOAL, and LIST_GOALS to handle the full workflow from creation through confirmation to viewing the updated list.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CREATE_GOAL_PREVIEW") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Yes, that looks good")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Created task: 'Finish taxes' (Priority 2, Due: 4/15/2024). Now showing all your goals.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_SUCCESS"), std::string("LIST_GOALS") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Add a goal to finish my taxes by April 15")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll create a one-off goal: 'Finish taxes' with Priority 2, Due April 15.\
\
Is this correct?")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CREATE_GOAL_PREVIEW") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Yes, that looks good")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Created task: 'Finish taxes' (Priority 2, Due: 4/15/2024)")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_SUCCESS") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want to add a daily task to exercise")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll create a daily goal: 'Exercise'.\
\
Is this correct?")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CREATE_GOAL_PREVIEW") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Actually, nevermind")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Okay, I've cancelled the task creation. Let me know if you'd like to create a different task.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CONFIRM_GOAL_CANCELLED") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
