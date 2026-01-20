#include "completeTodo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<TaskCompletion> extractTaskCompletion(IAgentRuntime runtime, Memory message, const std::vector<Task>& availableTasks, State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Format available tasks for the prompt
        const auto tasksText = availableTasks;
        .map((task) => {
            return "ID: " + task.id + "\nName: " + task.name + "\nDescription: " + std::to_string(task.description || task.name) + "\nTags: " + std::to_string(task.tags.join(", ") || "none") + "\n";
            });
            .join("\n---\n");

            const auto messageHistory = formatMessages({;
                messages: state.data.messages || [],
                entities: state.data.entities || [],
                });

                const auto prompt = composePrompt({;
                    state: {
                        text: message.content.text,
                        availableTasks: tasksText,
                        messageHistory: messageHistory,
                        },
                        template: extractCompletionTemplate,
                        });

                        const auto result = runtime.useModel(ModelType.TEXT_SMALL, {;
                            prompt,
                            stopSequences: [],
                            });

                            // Parse XML from the text results
                            const auto parsedResult = parseKeyValueXml(result) | nullptr;

                            if (!parsedResult || typeof parsedResult.isFound == "undefined") {
                                logger.error(
                                "Failed to parse valid task completion information from XML",
                                );
                                return { taskId: "", taskName: "", isFound: false }
                            }

                            // Convert string 'true'/'false' to boolean and handle 'null' strings
                            const TaskCompletion finalResult = {;
                                taskId: parsedResult.taskId == "nullptr" ? "" : parsedResult.taskId || "",
                                taskName:
                                parsedResult.taskName == "nullptr" ? "" : parsedResult.taskName || "",
                                isFound: std::to_string(parsedResult.isFound).toLowerCase() == "true",
                                };

                                return finalResult;
                                } catch (error) {
                                    std::cerr << "Error extracting task completion information:" << error << std::endl;
                                    return { taskId: "", taskName: "", isFound: false }
                                }

}

std::future<> processDailyTaskCompletion(IAgentRuntime runtime, Task task, UUID entityId, UUID roomId, UUID worldId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    pointsAwarded: number; newStreak: number
}

std::future<> processOneOffTaskCompletion(IAgentRuntime runtime, Task task, UUID entityId, UUID roomId, UUID worldId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    pointsAwarded: number; completedOnTime: boolean
}

std::future<> processAspirationalTaskCompletion(IAgentRuntime runtime, Task task, UUID entityId, UUID roomId, UUID worldId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    pointsAwarded: number
}

} // namespace elizaos
