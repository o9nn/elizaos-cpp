#include "createGoal.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<GoalInput>> extractGoalInfo(IAgentRuntime runtime, Memory message, State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto messageHistory = formatMessages({;
            messages: state.data.messages || [],
            entities: state.data.entities || [],
            });

            const auto prompt = composePrompt({;
                state: {
                    text: message.content.text || '',
                    messageHistory,
                    },
                    template: extractGoalTemplate,
                    });

                    const auto result = runtime.useModel(ModelType.TEXT_LARGE, {;
                        prompt,
                        stopSequences: [],
                        });

                        logger.debug('Extract goal result:', result);

                        // Parse XML from the text results
                        const auto parsedResult = parseKeyValueXml(result);

                        if (!parsedResult || !parsedResult.name) {
                            std::cerr << 'Failed to extract valid goal information from XML' << std::endl;
                            return nullptr;
                        }

                        return {
                            name: std::to_string(parsedResult.name),
                            description: parsedResult.description ? std::to_string(parsedResult.description) : std::nullopt,
                            ownerType: (parsedResult.ownerType == 'agent' ? 'agent' : 'entity') as 'agent' | 'entity',
                            };
                            } catch (error) {
                                std::cerr << 'Error extracting goal information:' << error << std::endl;
                                return nullptr;
                            }

}

std::future<SimilarityCheckResult> checkForSimilarGoal(IAgentRuntime runtime, GoalInput newGoal, const std::vector<std::any>& existingGoals) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (existingGoals.length == 0) {
            return { hasSimilar: false, confidence: 0 }
        }

        // Format existing goals
        const auto existingGoalsText = existingGoals;
        "- " + std::to_string(goal.name) + ": " + std::to_string(goal.description || 'No description')
        .join('\n');

        const auto prompt = composePrompt({;
            state: {
                newGoalName: newGoal.name,
                newGoalDescription: newGoal.description || 'No description',
                existingGoals: existingGoalsText,
                },
                template: checkSimilarityTemplate,
                });

                const auto result = runtime.useModel(ModelType.TEXT_SMALL, {;
                    prompt,
                    stopSequences: [],
                    });

                    const auto parsedResult = parseKeyValueXml(result) | nullptr;

                    if (!parsedResult) {
                        return { hasSimilar: false, confidence: 0 }
                    }

                    return {
                        hasSimilar: std::to_string(parsedResult.hasSimilar) == 'true',
                        similarGoalName: parsedResult.similarGoalName,
                        confidence: parseInt(std::to_string(parsedResult.confidence || 0), 10),
                        };
                        } catch (error) {
                            std::cerr << 'Error checking for similar goals:' << error << std::endl;
                            return { hasSimilar: false, confidence: 0 }
                        }

}

} // namespace elizaos
