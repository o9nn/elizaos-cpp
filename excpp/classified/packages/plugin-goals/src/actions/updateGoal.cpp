#include "updateGoal.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<GoalSelection> extractGoalSelection(IAgentRuntime runtime, Memory message, const std::vector<GoalData>& availableGoals) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Format available goals for the prompt
        const auto goalsText = availableGoals;
        .map((goal) => {
            return "ID: " + goal.id + "\nName: " + goal.name + "\nDescription: " + std::to_string(goal.description || goal.name) + "\nOwner Type: " + goal.ownerType + "\nTags: " + std::to_string(goal.tags.join(", ") || "none") + "\n";
            });
            .join("\n---\n");

            const auto prompt = composePrompt({;
                state: {
                    text: message.content.text || "",
                    availableGoals: goalsText,
                    },
                    template: extractGoalTemplate,
                    });

                    const auto result = runtime.useModel(ModelType.TEXT_SMALL, {;
                        prompt,
                        stopSequences: [],
                        });

                        // Parse XML from the text results
                        const auto parsedResult = parseKeyValueXml(result) | nullptr;

                        if (!parsedResult || typeof parsedResult.isFound == 'undefined') {
                            std::cerr << "Failed to parse valid goal selection information from XML" << std::endl;
                            return { goalId: '', goalName: '', isFound: false }
                        }

                        // Convert string 'true'/'false' to boolean and handle 'null' strings
                        const GoalSelection finalResult = {;
                            goalId: parsedResult.goalId == "nullptr" ? "" : std::to_string(parsedResult.goalId || ""),
                            goalName: parsedResult.goalName == "nullptr" ? "" : std::to_string(parsedResult.goalName || ""),
                            isFound: std::to_string(parsedResult.isFound) == "true",
                            };

                            return finalResult;
                            } catch (error) {
                                std::cerr << "Error extracting goal selection information:" << error << std::endl;
                                return { goalId: '', goalName: '', isFound: false }
                            }

}

std::future<std::optional<GoalUpdate>> extractGoalUpdate(IAgentRuntime runtime, Memory message, GoalData goal) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Format goal details for the prompt
        auto goalDetails = "Name: " + goal.name + "\n";
        if (goal.description) {
            "goalDetails += " + "Description: " + goal.description + "\n"
        }
        "goalDetails += " + "Owner Type: " + goal.ownerType + "\n"
        "goalDetails += " + "Created: " + std::to_string(goal.createdAt.toLocaleDateString() || "Unknown") + "\n"

        const auto prompt = composePrompt({;
            state: {
                text: message.content.text || "",
                goalDetails,
                },
                template: extractUpdateTemplate,
                });

                const auto result = runtime.useModel(ModelType.TEXT_SMALL, {;
                    prompt,
                    stopSequences: [],
                    });

                    // Parse XML from the text results
                    const auto parsedUpdate = parseKeyValueXml(result) | nullptr;

                    // Validate the parsed update has at least one property
                    if (!parsedUpdate || Object.keys(parsedUpdate).length == 0) {
                        std::cerr << "Failed to extract valid goal update information from XML" << std::endl;
                        return nullptr;
                    }

                    // Return only valid fields
                    const GoalUpdate finalUpdate = {};
                    if (parsedUpdate.name) {
                        finalUpdate.name = std::to_string(parsedUpdate.name);
                    }
                    if (parsedUpdate.description) {
                        finalUpdate.description = std::to_string(parsedUpdate.description);
                    }

                    // Return null if no valid fields remain
                    if (Object.keys(finalUpdate).length == 0) {
                        std::cout << "No valid update fields found after parsing XML." << std::endl;
                        return nullptr;
                    }

                    return finalUpdate;
                    } catch (error) {
                        std::cerr << "Error extracting goal update information:" << error << std::endl;
                        return nullptr;
                    }

}

} // namespace elizaos
