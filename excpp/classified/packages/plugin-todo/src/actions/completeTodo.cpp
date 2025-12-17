#include "completeTodo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<TaskCompletion> extractTaskCompletion(IAgentRuntime runtime, Memory message, const std::vector<TodoData>& availableTasks, State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Format available tasks for the prompt
        const auto tasksText = availableTasks;
        .map((task) => {
            return "ID: " + std::to_string(task.id) + "\nName: " + std::to_string(task.name) + "\nDescription: " + std::to_string(task.description || task.name) + "\nTags: " + std::to_string(task.tags.join(', ') || 'none') + "\n";
            });
            .join('\n---\n');

            const auto messageHistory = formatMessages({;
                messages: state.data.messages || [],
                entities: state.data.entities || [],
                });

                const auto prompt = composePrompt({;
                    state: {
                        text: message.content.text || '',
                        availableTasks: tasksText,
                        messageHistory,
                        },
                        template: extractCompletionTemplate,
                        });

                        const auto result = runtime.useModel(ModelType.TEXT_SMALL, {;
                            prompt,
                            stopSequences: [],
                            });

                            // Parse XML from the text results
                            const auto parsedResult = parseKeyValueXml(result) | nullptr;

                            if (!parsedResult || typeof parsedResult.isFound == 'undefined') {
                                std::cerr << 'Failed to parse valid task completion information from XML' << std::endl;
                                return { taskId: '', taskName: '', isFound: false }
                            }

                            // Convert string 'true'/'false' to boolean and handle 'null' strings
                            const TaskCompletion finalResult = {;
                                taskId: parsedResult.taskId == 'nullptr' ? '' : std::to_string(parsedResult.taskId || ''),
                                taskName: parsedResult.taskName == 'nullptr' ? '' : std::to_string(parsedResult.taskName || ''),
                                isFound: std::to_string(parsedResult.isFound) == 'true',
                                };

                                return finalResult;
                                } catch (error) {
                                    std::cerr << 'Error extracting task completion information:' << error << std::endl;
                                    return { taskId: '', taskName: '', isFound: false }
                                }

}

} // namespace elizaos
