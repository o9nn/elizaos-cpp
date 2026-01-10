#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/actions/updateGoal.h"

std::shared_ptr<Promise<std::shared_ptr<GoalSelection>>> extractGoalSelection(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<GoalData>> availableGoals)
{
    try
    {
        auto goalsText = availableGoals->map([=](auto goal) mutable
        {
            return std::string("ID: ") + goal->id + std::string("\
Name: ") + goal->name + std::string("\
Description: ") + (OR((goal->description), (goal->name))) + std::string("\
Owner Type: ") + goal->ownerType + std::string("\
Tags: ") + (OR((goal->tags->join(std::string(", "))), (std::string("none")))) + std::string("\
");
        }
        )->join(std::string("\
---\
"));
        auto prompt = composePrompt(object{
            object::pair{std::string("state"), object{
                object::pair{std::string("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std::string("availableGoals"), goalsText}
            }}, 
            object::pair{std::string("template"), extractGoalTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("stopSequences"), array<any>()}
        }); });
        auto parsedResult = as<any>(parseKeyValueXml(result));
        if (OR((!parsedResult), (type_of(parsedResult->isFound) == std::string("undefined")))) {
            logger->error(std::string("Failed to parse valid goal selection information from XML"));
            return object{
                object::pair{std::string("goalId"), string_empty}, 
                object::pair{std::string("goalName"), string_empty}, 
                object::pair{std::string("isFound"), false}
            };
        }
        auto finalResult = object{
            object::pair{std::string("goalId"), (parsedResult->goalId == std::string("null")) ? any(string_empty) : any(String(OR((parsedResult->goalId), (string_empty))))}, 
            object::pair{std::string("goalName"), (parsedResult->goalName == std::string("null")) ? any(string_empty) : any(String(OR((parsedResult->goalName), (string_empty))))}, 
            object::pair{std::string("isFound"), String(parsedResult->isFound) == std::string("true")}
        };
        return finalResult;
    }
    catch (const any& error)
    {
        logger->error(std::string("Error extracting goal selection information:"), error);
        return object{
            object::pair{std::string("goalId"), string_empty}, 
            object::pair{std::string("goalName"), string_empty}, 
            object::pair{std::string("isFound"), false}
        };
    }
};


std::shared_ptr<Promise<any>> extractGoalUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<GoalData> goal)
{
    try
    {
        auto goalDetails = std::string("Name: ") + goal->name + std::string("\
");
        if (goal->description) {
            goalDetails += std::string("Description: ") + goal->description + std::string("\
");
        }
        goalDetails += std::string("Owner Type: ") + goal->ownerType + std::string("\
");
        goalDetails += std::string("Created: ") + (OR((goal->createdAt->toLocaleDateString()), (std::string("Unknown")))) + std::string("\
");
        auto prompt = composePrompt(object{
            object::pair{std::string("state"), object{
                object::pair{std::string("text"), OR((message->content->text), (string_empty))}, 
                object::pair{std::string("goalDetails"), std::string("goalDetails")}
            }}, 
            object::pair{std::string("template"), extractUpdateTemplate}
        });
        auto result = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("stopSequences"), array<any>()}
        }); });
        auto parsedUpdate = as<any>(parseKeyValueXml(result));
        if (OR((!parsedUpdate), (Object->keys(parsedUpdate)->get_length() == 0))) {
            logger->error(std::string("Failed to extract valid goal update information from XML"));
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
            logger->warn(std::string("No valid update fields found after parsing XML."));
            return nullptr;
        }
        return finalUpdate;
    }
    catch (const any& error)
    {
        logger->error(std::string("Error extracting goal update information:"), error);
        return nullptr;
    }
};


string extractGoalTemplate = std::string("\
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
string extractUpdateTemplate = std::string("\
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
    object::pair{std::string("name"), std::string("UPDATE_GOAL")}, 
    object::pair{std::string("similes"), array<string>{ std::string("EDIT_GOAL"), std::string("MODIFY_GOAL"), std::string("CHANGE_GOAL"), std::string("REVISE_GOAL") }}, 
    object::pair{std::string("description"), std::string("Updates an existing goal's name or description. Can be chained with LIST_GOALS to see updated goals or COMPLETE_GOAL to mark it done.")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        try
        {
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std::string("goals")));
            auto agentGoalCount = std::async([=]() { dataService->countGoals(std::string("agent"), runtime->agentId, false); });
            auto entityGoalCount = (message->entityId) ? any(std::async([=]() { dataService->countGoals(std::string("entity"), as<std::shared_ptr<UUID>>(message->entityId), false); })) : any(0);
            return agentGoalCount + entityGoalCount > 0;
        }
        catch (const any& error)
        {
            logger->error(std::string("Error validating UPDATE_GOAL action:"), error);
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
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL_ERROR") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("UPDATE_GOAL")}, 
                        object::pair{std::string("error"), std::string("No state context")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("No state context")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std::string("goals")));
            auto agentGoals = std::async([=]() { dataService->getGoals(object{
                object::pair{std::string("ownerType"), std::string("agent")}, 
                object::pair{std::string("ownerId"), runtime->agentId}, 
                object::pair{std::string("isCompleted"), false}
            }); });
            auto entityGoals = (message->entityId) ? std::async([=]() { dataService->getGoals(object{
                object::pair{std::string("ownerType"), std::string("entity")}, 
                object::pair{std::string("ownerId"), as<std::shared_ptr<UUID>>(message->entityId)}, 
                object::pair{std::string("isCompleted"), false}
            }); }) : array<any>();
            auto availableGoals = array<std::shared_ptr<GoalData>>{ agentGoals, entityGoals };
            if (availableGoals->get_length() == 0) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("There are no active goals to update. Would you like to create a new goal?")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL_NO_GOALS") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("UPDATE_GOAL")}, 
                        object::pair{std::string("error"), std::string("No active goals")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("No active goals")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            shared goalSelection = std::async([=]() { extractGoalSelection(runtime, message, availableGoals); });
            if (!goalSelection->isFound) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I couldn't determine which goal you want to update. Could you be more specific? Here are the current goals:\
\
") + availableGoals->map([=](auto goal) mutable
                        {
                            return std::string("- ") + goal->name + std::string(" (") + goal->ownerType + std::string(" goal)");
                        }
                        )->join(std::string("\
")) + string_empty}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("UPDATE_GOAL")}, 
                        object::pair{std::string("error"), std::string("Goal not found")}, 
                        object::pair{std::string("availableGoals"), availableGoals->map([=](auto g) mutable
                        {
                            return (object{
                                object::pair{std::string("id"), g->id}, 
                                object::pair{std::string("name"), g->name}, 
                                object::pair{std::string("ownerType"), g->ownerType}
                            });
                        }
                        )}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("Goal not found")}, 
                        object::pair{std::string("needsClarification"), true}
                    }}, 
                    object::pair{std::string("success"), false}
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
                        object::pair{std::string("text"), std::string("I couldn't find a goal matching "") + goalSelection->goalName + std::string("". Please try again with the exact goal name.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL_NOT_FOUND") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("UPDATE_GOAL")}, 
                        object::pair{std::string("error"), std::string("Goal not found")}, 
                        object::pair{std::string("attemptedGoalName"), goalSelection->goalName}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("Goal not found")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            auto update = std::async([=]() { extractGoalUpdate(runtime, message, goal); });
            if (!update) {
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I couldn't determine what changes you want to make to "") + goal->name + std::string("". You can update the goal's name or description.")}, 
                        object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL_INVALID_UPDATE") }}, 
                        object::pair{std::string("source"), message->content->source}
                    }); });
                }
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("UPDATE_GOAL")}, 
                        object::pair{std::string("error"), std::string("Invalid update")}, 
                        object::pair{std::string("goalId"), goal->id}, 
                        object::pair{std::string("goalName"), goal->name}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("Invalid update")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            std::async([=]() { dataService->updateGoal(goal->id, update); });
            auto ownerText = (goal->ownerType == std::string("agent")) ? std::string("Agent") : std::string("User");
            auto updateText = array<string>();
            if (update->name) {
                updateText->push(std::string("name to "") + update->name + std::string("""));
            }
            if (update->description) {
                updateText->push(std::string("description to "") + update->description + std::string("""));
            }
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("✓ ") + ownerText + std::string(" goal updated: Changed ") + updateText->join(std::string(" and ")) + std::string(".")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL_SUCCESS") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("UPDATE_GOAL")}, 
                    object::pair{std::string("updatedGoalId"), goal->id}, 
                    object::pair{std::string("updatedGoalName"), goal->name}, 
                    object::pair{std::string("updates"), update}, 
                    object::pair{std::string("updateText"), updateText->join(std::string(" and "))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("goalId"), goal->id}, 
                    object::pair{std::string("goalName"), goal->name}, 
                    object::pair{std::string("updatedFields"), Object->keys(update)}
                }}, 
                object::pair{std::string("success"), true}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in updateGoal handler:"), error);
            if (callback) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I encountered an error while trying to update your goal. Please try again.")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL_ERROR") }}, 
                    object::pair{std::string("source"), message->content->source}
                }); });
            }
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("UPDATE_GOAL")}, 
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
            object::pair{std::string("text"), std::string("Update my French learning goal to Spanish and show me all my goals")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've updated your goal to "Learn Spanish fluently". Now let me show you all your goals.")}, 
            object::pair{std::string("thought"), std::string("The user wants to modify an existing goal and then see their complete goal list. I need to chain UPDATE_GOAL with LIST_GOALS to show the modification took effect in the context of all their goals.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL"), std::string("LIST_GOALS") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Change my exercise goal description to "30 minutes daily" and mark it complete")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've updated the description. Now I'll mark it as complete.")}, 
            object::pair{std::string("thought"), std::string("The user wants to update a goal's details and then immediately complete it. This shows the update-complete workflow where we refine the goal definition before marking it as achieved.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL"), std::string("COMPLETE_GOAL") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Update my French learning goal to be about Spanish instead")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✓ User goal updated: Changed name to "Learn Spanish fluently".")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL_SUCCESS") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Change the description of my marathon goal to include a specific time target")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("✓ User goal updated: Changed description to "Complete a marathon in under 4 hours".")}, 
            object::pair{std::string("actions"), array<string>{ std::string("UPDATE_GOAL_SUCCESS") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
