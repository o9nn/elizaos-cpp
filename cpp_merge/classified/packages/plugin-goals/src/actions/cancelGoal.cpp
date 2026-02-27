#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/actions/cancelGoal.h"

std::shared_ptr<Promise<std::shared_ptr<TaskCancellation>>> extractTaskCancellation(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<GoalData>> availableGoals, std::shared_ptr<State> state)
{
    try
    {
        auto tasksText = availableGoals->map([=](auto task) mutable
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
std::shared_ptr<Action> cancelGoalAction = object{
    object::pair{std:("name"), std:("CANCEL_GOAL")}, 
    object::pair{std:("similes"), array<string>{ std:("DELETE_GOAL"), std:("REMOVE_TASK"), std:("DELETE_TASK"), std:("REMOVE_GOAL") }}, 
    object::pair{std:("description"), std:("Cancels and deletes a goal item from the user's task list immediately. Can be chained with LIST_GOALS to see remaining goals or CREATE_GOAL to add a new one.")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        try
        {
            if (!message->roomId) {
                return false;
            }
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std:("goals")));
            auto goals = std::async([=]() { dataService->getGoals(object{
                object::pair{std:("ownerType"), std:("entity")}, 
                object::pair{std:("ownerId"), message->entityId}, 
                object::pair{std:("isCompleted"), false}
            }); });
            return goals->get_length() > 0;
        }
        catch (const any& error)
        {
            logger->error(std:("Error validating CANCEL_GOAL action:"), error);
            return false;
        }
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto options, auto callback = undefined) mutable
    {
        try
        {
            if (!state) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("Unable to process request without state context.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_ERROR") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CANCEL_GOAL")}, 
                        object::pair{std:("error"), std:("No state context")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("No state context")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            if (!message->roomId) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I cannot manage goals without a room context.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_ERROR") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CANCEL_GOAL")}, 
                        object::pair{std:("error"), std:("Missing room context")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("Missing room context")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std:("goals")));
            auto activeGoals = std::async([=]() { dataService->getGoals(object{
                object::pair{std:("ownerType"), std:("entity")}, 
                object::pair{std:("ownerId"), message->entityId}, 
                object::pair{std:("isCompleted"), false}
            }); });
            if (activeGoals->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("You don't have any active goals to cancel.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_NONE") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CANCEL_GOAL")}, 
                        object::pair{std:("error"), std:("No active goals")}, 
                        object::pair{std:("activeGoalsCount"), 0}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("No active goals")}, 
                        object::pair{std:("hasActiveGoals"), false}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            shared cancelInfo = std::async([=]() { extractTaskCancellation(runtime, message, activeGoals, state); });
            if (OR((!cancelInfo->isFound), (!cancelInfo->taskId))) {
                auto goalsList = activeGoals->map([=](auto goal, auto index) mutable
                {
                    return string_empty + (index + 1) + std:(". ") + goal->name + string_empty;
                }
                )->join(std:("\
"));
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I couldn't determine which goal you want to cancel. Here are your active goals:\
\
") + goalsList + std:("\
\
Please specify which one you'd like to cancel.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_NOT_FOUND") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CANCEL_GOAL")}, 
                        object::pair{std:("error"), std:("Goal not found")}, 
                        object::pair{std:("availableGoals"), goalsList}, 
                        object::pair{std:("activeGoalsCount"), activeGoals->get_length()}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("Goal not found")}, 
                        object::pair{std:("needsClarification"), true}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            auto goalToCancel = activeGoals->find([=](auto g) mutable
            {
                return g->id == cancelInfo->taskId;
            }
            );
            if (!goalToCancel) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I couldn't find a goal matching "") + cancelInfo->taskName + std:("". Please try again.")}, 
                        object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_NOT_FOUND") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CANCEL_GOAL")}, 
                        object::pair{std:("error"), std:("Goal not found")}, 
                        object::pair{std:("attemptedGoalName"), cancelInfo->taskName}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("Goal not found")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            auto success = std::async([=]() { dataService->deleteGoal(goalToCancel->id); });
            if (success) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("✅ Cancelled goal: "") + goalToCancel->name + std:(""")}, 
                        object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_SUCCESS") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("CANCEL_GOAL")}, 
                        object::pair{std:("deletedGoalId"), goalToCancel->id}, 
                        object::pair{std:("deletedGoalName"), goalToCancel->name}, 
                        object::pair{std:("remainingGoals"), activeGoals->get_length() - 1}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), true}, 
                        object::pair{std:("goalId"), goalToCancel->id}, 
                        object::pair{std:("goalName"), goalToCancel->name}, 
                        object::pair{std:("remainingGoalsCount"), activeGoals->get_length() - 1}
                    }}, 
                    object::pair{std:("success"), true}
                };
            } else {
                throw any(std::make_shared<Error>(std:("Failed to delete goal")));
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Error in cancelGoal handler:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I encountered an error while trying to cancel your task. Please try again.")}, 
                    object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_ERROR") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("CANCEL_GOAL")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}
                }}, 
                object::pair{std:("success"), false}
            };
        }
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Cancel my task to finish taxes and show me what's left")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✅ Cancelled goal: "Finish taxes". Now let me show you your remaining goals.")}, 
            object::pair{std:("thought"), std:("The user wants to remove a goal and see their updated goal list. I need to chain CANCEL_GOAL with LIST_GOALS to remove the unwanted goal and then display the current active goals.")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL"), std:("LIST_GOALS") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Remove my exercise goal and add a new one for yoga")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've cancelled your exercise goal. Now I'll create a new goal for yoga.")}, 
            object::pair{std:("thought"), std:("The user wants to replace one goal with another. I'll chain CANCEL_GOAL with CREATE_GOAL to remove the old exercise goal and create the new yoga goal, ensuring smooth goal management transition.")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL"), std:("CREATE_GOAL") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Cancel my task to finish taxes")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Are you sure you want to cancel this one-off task: "Finish taxes" (Priority 2, due 4/15/2023)? Once cancelled, it will be permanently removed.")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_CONFIRM") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Yes, please cancel it")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✓ Task cancelled: "Finish taxes" has been removed from your goal list.")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I don't want to do 50 pushups anymore, please delete that task")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Are you sure you want to cancel this daily task: "Do 50 pushups"? Once cancelled, it will be permanently removed.")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_CONFIRM") }}
        }}
    }, object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("No, I changed my mind, I'll keep it")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've kept your daily task "Do 50 pushups" active. Keep up the good work!")}, 
            object::pair{std:("actions"), array<string>{ std:("CANCEL_GOAL_REJECTED") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
