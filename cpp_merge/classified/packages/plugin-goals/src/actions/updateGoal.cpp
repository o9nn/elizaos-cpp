#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/actions/updateGoal.h"

std::shared_ptr<Promise<std::shared_ptr<GoalSelection>>> extractGoalSelection(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<GoalData>> availableGoals)
{
    try
    {
        auto goalsText = availableGoals->map([=](auto goal) mutable
        {
            return std:("ID: ") + goal->id + std:("\
Name: ") + goal->name + std:("\
Description: ") + (OR((goal->description), (goal->name))) + std:("\
Owner Type: ") + goal->ownerType + std:("\
Tags: ") + (OR((goal->tags->join(std:(", "))), (std:("none")))) + std:("\
");
        }
        )->join(std:("\
---\
"));
        auto prompt = composePrompt(object{
            object::pair{std:("state"), object{
                object::pair{std:("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std:("availableGoals"), goalsText}
            }}, 
            object::pair{std:("template"), extractGoalTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std:("prompt"), std:("prompt")}, 
            object::pair{std:("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        if (OR((!parsedResult), (type_of(parsedResult->isFound) == std:("undefined")))) {
            logger->error(std:("Failed to parse valid goal selection information from XML"));
            return object{
                object::pair{std:("goalId"), string_empty}, 
                object::pair{std:("goalName"), string_empty}, 
                object::pair{std:("isFound"), false}
            };
        }
        auto finalResult = object{
            object::pair{std:("goalId"), (parsedResult->goalId == std:("null")) ? any(string_empty) (String(OR((parsedResult->goalId), (string_empty))))}, 
            object::pair{std:("goalName"), (parsedResult->goalName == std:("null")) ? any(string_empty) (String(OR((parsedResult->goalName), (string_empty))))}, 
            object::pair{std:("isFound"), String(parsedResult->isFound) == std:("true")}
        };
        return finalResult;
    }
    catch (const any& error)
    {
        logger->error(std:("Error extracting goal selection information:"), error);
        return object{
            object::pair{std:("goalId"), string_empty}, 
            object::pair{std:("goalName"), string_empty}, 
            object::pair{std:("isFound"), false}
        };
    }
};


std::shared_ptr<Promise<any>> extractGoalUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<GoalData> goal)
{
    try
    {
        auto goalDetails = std:("Name: ") + goal->name + std:("\
");
        if (goal->description) {
            goalDetails += std:("Description: ") + goal->description + std:("\
");
        }
        goalDetails += std:("Owner Type: ") + goal->ownerType + std:("\
");
        goalDetails += std:("Created: ") + (OR((goal->createdAt->toLocaleDateString()), (std:("Unknown")))) + std:("\
");
        auto prompt = composePrompt(object{
            object::pair{std:("state"), object{
                object::pair{std:("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std:("goalDetails"), std:("goalDetails")}
            }}, 
            object::pair{std:("template"), extractUpdateTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std:("prompt"), std:("prompt")}, 
            object::pair{std:("stopSequences"), array<any>()}
        }); });
        auto parsedUpdate = as<any>(parseKeyValueXml(result));
        if (OR((!parsedUpdate), (Object->keys(parsedUpdate)->get_length() == 0))) {
            logger->error(std:("Failed to extract valid goal update information from XML"));
            return nullptr;
        }
        auto finalUpdate = object{};
        if (parsedUpdate->name) {
            finalUpdate->name = String(parsedUpdate->name);
        }
        if (parsedUpdate->description) {
            finalUpdate->description = String(parsedUpdate->description);
        }
        if (Object->keys(finalUpdate)->get_length() == 0) {
            logger->warn(std:("No valid update fields found after parsing XML."));
            return nullptr;
        }
        return finalUpdate;
    }
    catch (const any& error)
    {
        logger->error(std:("Error extracting goal update information:"), error);
        return nullptr;
    }
};


string extractGoalTemplate = std:("\
# Task: Extract Goal Selection Information\
\
## User Message\
{{text}}\
\
## Available Goals\
{{availableGoals}}\
\
## Instructions\
Parse the user's message to identify which goal they want to update or modify.\
Match against the list of available goals by name or description.\
If multiple goals have similar names, choose the closest match.\
\
Return an XML object with:\
<response>\
  <goalId>ID of the goal being updated, or 'null' if not found</goalId>\
  <goalName>Name of the goal being updated, or 'null' if not found</goalName>\
  <isFound>'true' or 'false' indicating if a matching goal was found</isFound>\
</response>\
\
## Example Output Format\
<response>\
  <goalId>123e4567-e89b-12d3-a456-426614174000</goalId>\
  <goalName>Learn French fluently</goalName>\
  <isFound>true</isFound>\
</response>\
\
If no matching goal was found:\
<response>\
  <goalId>null</goalId>\
  <goalName>null</goalName>\
  <isFound>false</isFound>\
</response>\
");
string extractUpdateTemplate = std:("\
# Task: Extract Goal Update Information\
\
## User Message\
{{text}}\
\
## Current Goal Details\
{{goalDetails}}\
\
## Instructions\
Parse the user's message to determine what changes they want to make to the goal.\
Only name and description can be updated.\
\
Return an XML object with these potential fields (only include fields that should be changed):\
<response>\
  <name>New name for the goal</name>\
  <description>New description for the goal</description>\
</response>\
\
## Example Output Format\
<response>\
  <name>Learn Spanish fluently</name>\
  <description>Achieve conversational fluency in Spanish within 12 months</description>\
</response>\
");
std::shared_ptr<Action> updateGoalAction = object{
    object::pair{std:("name"), std:("UPDATE_GOAL")}, 
    object::pair{std:("similes"), array<string>{ std:("EDIT_GOAL"), std:("MODIFY_GOAL"), std:("CHANGE_GOAL"), std:("REVISE_GOAL") }}, 
    object::pair{std:("description"), std:("Updates an existing goal's name or description. Can be chained with LIST_GOALS to see updated goals or COMPLETE_GOAL to mark it done.")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        try
        {
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std:("goals")));
            auto agentGoalCount = std::async([=]() { dataService->countGoals(std:("agent"), runtime->agentId, false); });
            auto entityGoalCount = (message->entityId) ? any(std::async([=]() { dataService->countGoals(std:("entity"), as<std::shared_ptr<UUID>>(message->entityId), false); })) (0);
            return agentGoalCount + entityGoalCount > 0;
        }
        catch (const any& error)
        {
            logger->error(std:("Error validating UPDATE_GOAL action:"), error);
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
                        object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL_ERROR") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("UPDATE_GOAL")}, 
                        object::pair{std:("error"), std:("No state context")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("No state context")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std:("goals")));
            auto agentGoals = std::async([=]() { dataService->getGoals(object{
                object::pair{std:("ownerType"), std:("agent")}, 
                object::pair{std:("ownerId"), runtime->agentId}, 
                object::pair{std:("isCompleted"), false}
            }); });
            auto entityGoals = (message->entityId) ? std::async([=]() { dataService->getGoals(object{
                object::pair{std:("ownerType"), std:("entity")}, 
                object::pair{std:("ownerId"), as<std::shared_ptr<UUID>>(message->entityId)}, 
                object::pair{std:("isCompleted"), false}
            }); }) : array<any>();
            auto availableGoals = array<std::shared_ptr<GoalData>>{ agentGoals, entityGoals };
            if (availableGoals->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("There are no active goals to update. Would you like to create a new goal?")}, 
                        object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL_NO_GOALS") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("UPDATE_GOAL")}, 
                        object::pair{std:("error"), std:("No active goals")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("No active goals")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            shared goalSelection = std::async([=]() { extractGoalSelection(runtime, message, availableGoals); });
            if (!goalSelection->isFound) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I couldn't determine which goal you want to update. Could you be more specific? Here are the current goals:\
\
") + availableGoals->map([=](auto goal) mutable
                        {
                            return std:("- ") + goal->name + std:(" (") + goal->ownerType + std:(" goal)");
                        }
                        )->join(std:("\
")) + string_empty}, 
                        object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL_NOT_FOUND") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("UPDATE_GOAL")}, 
                        object::pair{std:("error"), std:("Goal not found")}, 
                        object::pair{std:("availableGoals"), availableGoals->map([=](auto g) mutable
                        {
                            return (object{
                                object::pair{std:("id"), g->id}, 
                                object::pair{std:("name"), g->name}, 
                                object::pair{std:("ownerType"), g->ownerType}
                            });
                        }
                        )}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("Goal not found")}, 
                        object::pair{std:("needsClarification"), true}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            auto goal = availableGoals->find([=](auto g) mutable
            {
                return g->id == goalSelection->goalId;
            }
            );
            if (!goal) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I couldn't find a goal matching "") + goalSelection->goalName + std:("". Please try again with the exact goal name.")}, 
                        object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL_NOT_FOUND") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("UPDATE_GOAL")}, 
                        object::pair{std:("error"), std:("Goal not found")}, 
                        object::pair{std:("attemptedGoalName"), goalSelection->goalName}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("Goal not found")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            auto update = std::async([=]() { extractGoalUpdate(runtime, message, goal); });
            if (!update) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I couldn't determine what changes you want to make to "") + goal->name + std:("". You can update the goal's name or description.")}, 
                        object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL_INVALID_UPDATE") }}, 
                        object::pair{std:("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("UPDATE_GOAL")}, 
                        object::pair{std:("error"), std:("Invalid update")}, 
                        object::pair{std:("goalId"), goal->id}, 
                        object::pair{std:("goalName"), goal->name}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("Invalid update")}
                    }}, 
                    object::pair{std:("success"), false}
                };
            }
            std::async([=]() { dataService->updateGoal(goal->id, update); });
            auto ownerText = (goal->ownerType == std:("agent")) ? std:("Agent") : std:("User");
            auto updateText = array<string>();
            if (update->name) {
                updateText->push(std:("name to "") + update->name + std:("""));
            }
            if (update->description) {
                updateText->push(std:("description to "") + update->description + std:("""));
            }
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("✓ ") + ownerText + std:(" goal updated: Changed ") + updateText->join(std:(" and ")) + std:(".")}, 
                    object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL_SUCCESS") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("UPDATE_GOAL")}, 
                    object::pair{std:("updatedGoalId"), goal->id}, 
                    object::pair{std:("updatedGoalName"), goal->name}, 
                    object::pair{std:("updates"), update}, 
                    object::pair{std:("updateText"), updateText->join(std:(" and "))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("goalId"), goal->id}, 
                    object::pair{std:("goalName"), goal->name}, 
                    object::pair{std:("updatedFields"), Object->keys(update)}
                }}, 
                object::pair{std:("success"), true}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in updateGoal handler:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std:("text"), std:("I encountered an error while trying to update your goal. Please try again.")}, 
                    object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL_ERROR") }}, 
                    object::pair{std:("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("UPDATE_GOAL")}, 
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
            object::pair{std:("text"), std:("Update my French learning goal to Spanish and show me all my goals")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've updated your goal to "Learn Spanish fluently". Now let me show you all your goals.")}, 
            object::pair{std:("thought"), std:("The user wants to modify an existing goal and then see their complete goal list. I need to chain UPDATE_GOAL with LIST_GOALS to show the modification took effect in the context of all their goals.")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL"), std:("LIST_GOALS") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Change my exercise goal description to "30 minutes daily" and mark it complete")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've updated the description. Now I'll mark it as complete.")}, 
            object::pair{std:("thought"), std:("The user wants to update a goal's details and then immediately complete it. This shows the update-complete workflow where we refine the goal definition before marking it as achieved.")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL"), std:("COMPLETE_GOAL") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Update my French learning goal to be about Spanish instead")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✓ User goal updated: Changed name to "Learn Spanish fluently".")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL_SUCCESS") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Change the description of my marathon goal to include a specific time target")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("✓ User goal updated: Changed description to "Complete a marathon in under 4 hours".")}, 
            object::pair{std:("actions"), array<string>{ std:("UPDATE_GOAL_SUCCESS") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
