#include "cancelTodo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<TaskCancellation> extractTaskCancellation(IAgentRuntime runtime, Memory message, const std::vector<Task>& availableTasks, State state) {
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
                        template: extractCancellationTemplate,
                        });

                        const auto result = runtime.useModel(ModelType.TEXT_SMALL, {;
                            prompt,
                            stopSequences: [],
                            });

                            // Parse XML from the text results
                            const auto parsedResult = parseKeyValueXml(result) | nullptr;

                            std::cout << "*** parsed XML Result" << parsedResult << std::endl;

                            if (!parsedResult || typeof parsedResult.isFound == "undefined") {
                                logger.error(
                                "Failed to parse valid task cancellation information from XML",
                                );
                                return { taskId: "", taskName: "", isFound: false }
                            }

                            // Convert string 'true'/'false' to boolean and handle 'null' strings
                            const TaskCancellation finalResult = {;
                                taskId: parsedResult.taskId == "nullptr" ? "" : parsedResult.taskId || "",
                                taskName:
                                parsedResult.taskName == "nullptr" ? "" : parsedResult.taskName || "",
                                isFound: std::to_string(parsedResult.isFound).toLowerCase() == "true",
                                };

                                return finalResult;
                                } catch (error) {
                                    std::cerr << "Error extracting task cancellation information:" << error << std::endl;
                                    return { taskId: "", taskName: "", isFound: false }
                                }

}

} // namespace elizaos
