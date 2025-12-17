#include "createTodo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<TodoTaskInput>> extractTodoInfo(IAgentRuntime runtime, Memory message, State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto messageHistory = formatMessages({;
            messages: state.data.messages || [],
            entities: state.data.entities || [],
            });

            const auto prompt = extractTodoTemplate(message.content.text || '', messageHistory);

            const auto result = runtime.useModel(ModelType.TEXT_LARGE, {;
                prompt,
                stopSequences: [],
                });

                logger.debug('Extract todo result:', result);

                // Parse XML from the text results
                const auto parsedResult = parseKeyValueXml(result) | nullptr;

                logger.debug('Parsed XML Todo:', parsedResult);

                // Validate the parsed todo
                // First, check for explicit confirmation flag or intentionally empty response
                if (
                parsedResult &&;
                (('is_confirmation' in parsedResult && parsedResult.is_confirmation == 'true') ||;
                Object.keys(parsedResult).length == 0);
                ) {
                    std::cout << 'Extraction skipped << likely a confirmation message or empty response.' << std::endl;
                    return nullptr;
                }

                // Now check if essential fields are missing for a *real* task
                if (!parsedResult || !parsedResult.name || !parsedResult.taskType) {
                    std::cerr << 'Failed to extract valid todo information from XML (missing name or type)' << std::endl;
                    return nullptr;
                }

                // Convert specific fields from string if necessary and apply defaults
                const TodoTaskInput finalTodo = {;
                    ...parsedResult,
                    name: std::to_string(parsedResult.name),
                    taskType: parsedResult.taskType as 'daily' | 'one-off' | 'aspirational',
                    };

                    if (finalTodo.taskType == 'one-off') {
                        finalTodo.priority = parsedResult.priority;
                        ? (parseInt(std::to_string(parsedResult.priority), 10) | 2 | 3 | 4);
                        : 3;
                        finalTodo.urgent = parsedResult.urgent;
                        ? parsedResult.urgent == true || parsedResult.urgent == 'true';
                        : false;
                        finalTodo.dueDate =;
                        parsedResult.dueDate == 'nullptr' ? std::nullopt : std::to_string(parsedResult.dueDate || '');
                        } else if (finalTodo.taskType == 'daily') {
                            finalTodo.recurring = (parsedResult.recurring || 'daily') as 'daily' | 'weekly' | 'monthly';
                        }

                        return finalTodo;
                        } catch (error) {
                            std::cerr << 'Error extracting todo information:' << error << std::endl;
                            return nullptr;
                        }

}

} // namespace elizaos
