#include "confirmGoal.hpp"

std::shared_ptr<Promise<std::shared_ptr<ConfirmationResponse>>> extractConfirmationIntent(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, any pendingTask, std::shared_ptr<State> state)
{
    try
    {
        if (!pendingTask) {
            return object{
                object::pair{std:("isConfirmation"), false}, 
                object::pair{std:("shouldProceed"), false}
            };
        }
        auto messageHistory = formatMessages(object{
            object::pair{std:("messages"), OR((state->data->messages), (array<any>()))}, 
            object::pair{std:("entities"), OR((state->data->entities), (array<any>()))}
        });
        auto pendingTaskText = std:("\
Name: ") + pendingTask->name + std:("\
Type: ") + pendingTask->taskType + std:("\
") + (pendingTask->priority) ? any(std:("Priority: ") + pendingTask->priority + string_empty) (string_empty) + std:("\
") + (pendingTask->urgent) ? std:("Urgent: Yes") : string_empty + std:("\
") + (pendingTask->dueDate) ? any(std:("Due Date: ") + pendingTask->dueDate + string_empty) (string_empty) + std:("\
") + (pendingTask->recurring) ? any(std:("Recurring: ") + pendingTask->recurring + string_empty) (string_empty) + std:("\
");
        auto prompt = composePrompt(object{
            object::pair{std:("state"), object{
                object::pair{std:("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std:("messageHistory"), std:("messageHistory")}, 
                object::pair{std:("pendingTask"), pendingTaskText}
            }}, 
            object::pair{std:("template"), extractConfirmationTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std:("prompt"), std:("prompt")}, 
            object::pair{std:("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        if (!parsedResult) {
            logger->error(std:("Failed to parse confirmation response"));
            return object{
                object::pair{std:("isConfirmation"), false}, 
                object::pair{std:("shouldProceed"), false}
            };
        }
        return object{
            object::pair{std:("isConfirmation"), String(parsedResult->isConfirmation) == std:("true")}, 
            object::pair{std:("shouldProceed"), String(parsedResult->shouldProceed) == std:("true")}, 
            object::pair{std:("modifications"), (parsedResult->modifications == std:("none")) ? any(undefined) (parsedResult->modifications)}
        };
    }
    catch (const any& error)
    {
        logger->error(std:("Error extracting confirmation intent:"), error);
        return object{
            object::pair{std:("isConfirmation"), false}, 
            object::pair{std:("shouldProceed"), false}
        };
    }
};


string extractConfirmationTemplate = std:("\
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
    object::pair{std:("name"), std:("CONFIRM_GOAL")}, 
    object::pair{std:("similes"), array<string>{ std:("CONFIRM_TASK"), std:("APPROVE_GOAL"), std:("APPROVE_TASK"), std:("GOAL_CONFIRM") }}, 
    object::pair{std:("description"), std:("Confirms or cancels a pending goal creation after user review. Can be chained with LIST_GOALS to see updated list or UPDATE_GOAL to modify the confirmed goal.")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        auto pendingGoal = as<any>(state->data->pendingGoal);
        return !!pendingGoal;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto options, auto callback = undefined) mutable
    {
        try
        {
            if (!state) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("Unable to process confirmation without state context.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_ERROR") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("text"), std:("Unable to process confirmation without state context.")}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CONFIRM_GOAL")}, 
                        object::pair{std:("error"), std:("missing_state_context")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("missing_state_context")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            shared pendingGoal = as<any>(state["data"]["pendingGoal"]);
            if (!pendingGoal) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I don't have a pending task to confirm. Would you like to create a new task?")}, 
                        object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_NO_PENDING") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("text"), std:("I don't have a pending task to confirm. Would you like to create a new task?")}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CONFIRM_GOAL")}, 
                        object::pair{std:("error"), std:("no_pending_goal")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("no_pending_goal")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            if (OR((!message->roomId), (!message->entityId))) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I cannot confirm a goal without a room and entity context.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_ERROR") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("text"), std:("I cannot confirm a goal without a room and entity context.")}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CONFIRM_GOAL")}, 
                        object::pair{std:("error"), std:("missing_context")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("missing_context")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            auto confirmation = std::async([=]() { extractConfirmationIntent(runtime, message, pendingGoal, state); });
            if (!confirmation->isConfirmation) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I'm still waiting for your confirmation on the task "") + pendingGoal->name + std:("". Would you like me to create it?")}, 
                        object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_WAITING") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("text"), std:("I'm still waiting for your confirmation on the task "") + pendingGoal->name + std:("". Would you like me to create it?")}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CONFIRM_GOAL")}, 
                        object::pair{std:("status"), std:("waiting_for_confirmation")}, 
                        object::pair{std:("pendingGoalName"), pendingGoal->name}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("awaiting_confirmation"), true}, 
                        object::pair{std:("goalName"), pendingGoal->name}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            if (!confirmation->shouldProceed) {
                state["data"].Delete("pendingGoal");
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("Okay, I've cancelled the task creation. Let me know if you'd like to create a different task.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_CANCELLED") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("text"), std:("Okay, I've cancelled the task creation. Let me know if you'd like to create a different task.")}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CONFIRM_GOAL")}, 
                        object::pair{std:("status"), std:("cancelled")}, 
                        object::pair{std:("cancelledGoalName"), pendingGoal->name}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), true}, 
                        object::pair{std:("goalCancelled"), true}, 
                        object::pair{std:("goalName"), pendingGoal->name}
                    }}, 
                    object::pair{std:("success"), true}
                };
            }
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std:("goals")));
            auto existingGoals = std::async([=]() { dataService->getGoals(object{
                object::pair{std:("ownerId"), message->entityId}, 
                object::pair{std:("ownerType"), std:("entity")}, 
                object::pair{std:("isCompleted"), false}
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
                        object::pair{std:("text"), std:("It looks like you already have an active goal named "") + pendingGoal->name + std:("". I haven't added a duplicate.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_DUPLICATE") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("text"), std:("It looks like you already have an active goal named "") + pendingGoal->name + std:("". I haven't added a duplicate.")}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CONFIRM_GOAL")}, 
                        object::pair{std:("error"), std:("duplicate_goal")}, 
                        object::pair{std:("duplicateGoalName"), pendingGoal->name}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("duplicate_goal")}, 
                        object::pair{std:("goalName"), pendingGoal->name}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            auto createdGoalId = std::async([=]() { dataService->createGoal(object{
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("ownerType"), std:("entity")}, 
                object::pair{std:("ownerId"), message->entityId}, 
                object::pair{std:("name"), pendingGoal->name}, 
                object::pair{std:("description"), OR((pendingGoal->description), (pendingGoal->name))}, 
                object::pair{std:("metadata"), utils::assign(object{
                    , 
                    object::pair{std:("taskType"), pendingGoal->taskType}, 
                    object::pair{std:("priority"), pendingGoal->priority}, 
                    object::pair{std:("urgent"), pendingGoal->urgent}, 
                    object::pair{std:("dueDate"), pendingGoal->dueDate}, 
                    object::pair{std:("recurring"), pendingGoal->recurring}
                }, pendingGoal->metadata)}, 
                object::pair{std:("tags"), OR((pendingGoal->tags), (array<any>()))}
            }); });
            if (!createdGoalId) {
                throw any(std::make_shared<Error>(std:("Failed to create goal")));
            }
            state["data"].Delete("pendingGoal");
            auto successMessage = string_empty;
            if (pendingGoal->taskType == std:("daily")) {
                successMessage = std:("✅ Created daily task: "") + pendingGoal->name + std:("".");
            } else if (pendingGoal->taskType == std:("one-off")) {
                auto priorityText = std:("Priority ") + (OR((pendingGoal->priority), (3))) + string_empty;
                auto urgentText = (pendingGoal->urgent) ? std:(", Urgent") : string_empty;
                auto dueDateText = (pendingGoal->dueDate) ? any(std:(", Due: ") + ((std::make_shared<Date>(pendingGoal->dueDate)))->toLocaleDateString() + string_empty) (string_empty);
                successMessage = std:("✅ Created task: "") + pendingGoal->name + std:("" (") + priorityText + string_empty + urgentText + string_empty + dueDateText + std:(")");
            } else {
                successMessage = std:("✅ Created aspirational goal: "") + pendingGoal->name + std:(""");
            }
            if (confirmation->modifications) {
                successMessage += std:("\
\
Note: I created the task as originally described. The modifications you mentioned ("") + confirmation->modifications + std:("") weren't applied. You can use UPDATE_GOAL to make changes.");
            }
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), successMessage}, 
                    object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_SUCCESS") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std:("text"), successMessage}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("CONFIRM_GOAL")}, 
                    object::pair{std:("status"), std:("success")}, 
                    object::pair{std:("goalId"), createdGoalId}, 
                    object::pair{std:("goalName"), pendingGoal->name}, 
                    object::pair{std:("taskType"), pendingGoal->taskType}, 
                    object::pair{std:("priority"), pendingGoal->priority}, 
                    object::pair{std:("urgent"), pendingGoal->urgent}, 
                    object::pair{std:("dueDate"), pendingGoal->dueDate}, 
                    object::pair{std:("modifications"), confirmation->modifications}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("goalCreated"), true}, 
                    object::pair{std:("goalId"), createdGoalId}, 
                    object::pair{std:("goalName"), pendingGoal->name}
                }}, 
                object::pair{std:("success"), true}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in confirmGoal handler:"), error);
            auto errorMessage = std:("I encountered an error while confirming your goal. Please try again.");
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), errorMessage}, 
                    object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_ERROR") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std:("text"), errorMessage}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("CONFIRM_GOAL")}, 
                    object::pair{std:("error"), std:("execution_error")}, 
                    object::pair{std:("errorDetails"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("errorType"), std:("execution_error")}, 
                    object::pair{std:("hasError"), true}
                }}, 
                object::pair{std:("success"), false}
            };
        }
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Add a goal to finish my taxes by April 15 and show me all my goals after")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll create a one-off goal: 'Finish taxes' with Priority 2, Due April 15. After you confirm, I'll show you all your goals.")}, 
            object::pair{std:("thought"), std:("The user wants to create a goal with confirmation and then see their complete goal list. I need to chain CREATE_GOAL, CONFIRM_GOAL, and LIST_GOALS to handle the full workflow from creation through confirmation to viewing the updated list.")}, 
            object::pair{std:("actions"), array<string>{ std:("CREATE_GOAL_PREVIEW") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Yes, that looks good")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Created task: 'Finish taxes' (Priority 2, Due: 4/15/2024). Now showing all your goals.")}, 
            object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_SUCCESS"), std:("LIST_GOALS") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Add a goal to finish my taxes by April 15")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll create a one-off goal: 'Finish taxes' with Priority 2, Due April 15.\
\
Is this correct?")}, 
            object::pair{std:("actions"), array<string>{ std:("CREATE_GOAL_PREVIEW") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Yes, that looks good")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Created task: 'Finish taxes' (Priority 2, Due: 4/15/2024)")}, 
            object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_SUCCESS") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want to add a daily task to exercise")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll create a daily goal: 'Exercise'.\
\
Is this correct?")}, 
            object::pair{std:("actions"), array<string>{ std:("CREATE_GOAL_PREVIEW") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Actually, nevermind")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Okay, I've cancelled the task creation. Let me know if you'd like to create a different task.")}, 
            object::pair{std:("actions"), array<string>{ std:("CONFIRM_GOAL_CANCELLED") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
