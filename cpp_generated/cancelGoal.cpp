#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/actions/cancelGoal.h"

std::shared_ptr<Promise<std::shared_ptr<TaskCancellation>>> extractTaskCancellation(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<GoalData>> availableGoals, std::shared_ptr<State> state)
{
    try
    {
        auto tasksText = availableGoals->map([=](auto task) mutable
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
            object::pair{std::string("template"), extractCancellationTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        logger->debug(std::string("Parsed XML Result"), parsedResult);
        if (OR((!parsedResult), (type_of(parsedResult->isFound) == std::string("undefined")))) {
            logger->error(std::string("Failed to parse valid task cancellation information from XML"));
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
        logger->error(std::string("Error extracting task cancellation information:"), error);
        return object{
            object::pair{std::string("taskId"), string_empty}, 
            object::pair{std::string("taskName"), string_empty}, 
            object::pair{std::string("isFound"), false}
        };
    }
};


string extractCancellationTemplate = std::string("\
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
    object::pair{std::string("name"), std::string("CANCEL_GOAL")}, 
    object::pair{std::string("similes"), array<string>{ std::string("DELETE_GOAL"), std::string("REMOVE_TASK"), std::string("DELETE_TASK"), std::string("REMOVE_GOAL") }}, 
    object::pair{std::string("description"), std::string("Cancels and deletes a goal item from the user's task list immediately. Can be chained with LIST_GOALS to see remaining goals or CREATE_GOAL to add a new one.")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        try
        {
            if (!message->roomId) {
                return false;
            }
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std::string("goals")));
            auto goals = std::async([=]() { dataService->getGoals(object{
                object::pair{std::string("ownerType"), std::string("entity")}, 
                object::pair{std::string("ownerId"), message->entityId}, 
                object::pair{std::string("isCompleted"), false}
            }); });
            return goals->get_length() > 0;
        }
        catch (const any& error)
        {
            logger->error(std::string("Error validating CANCEL_GOAL action:"), error);
            return false;
        }
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto options, auto callback = undefined) mutable
    {
        try
        {
            if (!state) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("Unable to process request without state context.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CANCEL_GOAL")}, 
                        object::pair{std::string("error"), std::string("No state context")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("No state context")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            if (!message->roomId) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I cannot manage goals without a room context.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CANCEL_GOAL")}, 
                        object::pair{std::string("error"), std::string("Missing room context")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("Missing room context")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std::string("goals")));
            auto activeGoals = std::async([=]() { dataService->getGoals(object{
                object::pair{std::string("ownerType"), std::string("entity")}, 
                object::pair{std::string("ownerId"), message->entityId}, 
                object::pair{std::string("isCompleted"), false}
            }); });
            if (activeGoals->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("You don't have any active goals to cancel.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_NONE") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CANCEL_GOAL")}, 
                        object::pair{std::string("error"), std::string("No active goals")}, 
                        object::pair{std::string("activeGoalsCount"), 0}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("No active goals")}, 
                        object::pair{std::string("hasActiveGoals"), false}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            shared cancelInfo = std::async([=]() { extractTaskCancellation(runtime, message, activeGoals, state); });
            if (OR((!cancelInfo->isFound), (!cancelInfo->taskId))) {
                auto goalsList = activeGoals->map([=](auto goal, auto index) mutable
                {
                    return string_empty + (index + 1) + std::string(". ") + goal->name + string_empty;
                }
                )->join(std::string("\
"));
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I couldn't determine which goal you want to cancel. Here are your active goals:\
\
") + goalsList + std::string("\
\
Please specify which one you'd like to cancel.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CANCEL_GOAL")}, 
                        object::pair{std::string("error"), std::string("Goal not found")}, 
                        object::pair{std::string("availableGoals"), goalsList}, 
                        object::pair{std::string("activeGoalsCount"), activeGoals->get_length()}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("Goal not found")}, 
                        object::pair{std::string("needsClarification"), true}
                    }}, 
                    object::pair{std::string("success"), false}
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
                        object::pair{std::string("text"), std::string("I couldn't find a goal matching "") + cancelInfo->taskName + std::string("". Please try again.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CANCEL_GOAL")}, 
                        object::pair{std::string("error"), std::string("Goal not found")}, 
                        object::pair{std::string("attemptedGoalName"), cancelInfo->taskName}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("Goal not found")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            auto success = std::async([=]() { dataService->deleteGoal(goalToCancel->id); });
            if (success) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("✅ Cancelled goal: "") + goalToCancel->name + std::string(""")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_SUCCESS") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("CANCEL_GOAL")}, 
                        object::pair{std::string("deletedGoalId"), goalToCancel->id}, 
                        object::pair{std::string("deletedGoalName"), goalToCancel->name}, 
                        object::pair{std::string("remainingGoals"), activeGoals->get_length() - 1}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), true}, 
                        object::pair{std::string("goalId"), goalToCancel->id}, 
                        object::pair{std::string("goalName"), goalToCancel->name}, 
                        object::pair{std::string("remainingGoalsCount"), activeGoals->get_length() - 1}
                    }}, 
                    object::pair{std::string("success"), true}
                };
            } else {
                throw any(std::make_shared<Error>(std::string("Failed to delete goal")));
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in cancelGoal handler:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I encountered an error while trying to cancel your task. Please try again.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_ERROR") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("CANCEL_GOAL")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? error->message : String(error)}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? error->message : String(error)}
                }}, 
                object::pair{std::string("success"), false}
            };
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Cancel my task to finish taxes and show me what's left")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✅ Cancelled goal: "Finish taxes". Now let me show you your remaining goals.")}, 
            object::pair{std::string("thought"), std::string("The user wants to remove a goal and see their updated goal list. I need to chain CANCEL_GOAL with LIST_GOALS to remove the unwanted goal and then display the current active goals.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL"), std::string("LIST_GOALS") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Remove my exercise goal and add a new one for yoga")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've cancelled your exercise goal. Now I'll create a new goal for yoga.")}, 
            object::pair{std::string("thought"), std::string("The user wants to replace one goal with another. I'll chain CANCEL_GOAL with CREATE_GOAL to remove the old exercise goal and create the new yoga goal, ensuring smooth goal management transition.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL"), std::string("CREATE_GOAL") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Cancel my task to finish taxes")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Are you sure you want to cancel this one-off task: "Finish taxes" (Priority 2, due 4/15/2023)? Once cancelled, it will be permanently removed.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_CONFIRM") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Yes, please cancel it")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✓ Task cancelled: "Finish taxes" has been removed from your goal list.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I don't want to do 50 pushups anymore, please delete that task")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Are you sure you want to cancel this daily task: "Do 50 pushups"? Once cancelled, it will be permanently removed.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_CONFIRM") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("No, I changed my mind, I'll keep it")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've kept your daily task "Do 50 pushups" active. Keep up the good work!")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CANCEL_GOAL_REJECTED") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
