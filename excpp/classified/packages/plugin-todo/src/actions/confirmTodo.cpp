#include "confirmTodo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<ConfirmationResponse> extractConfirmationIntent(IAgentRuntime runtime, Memory message, const std::optional<PendingTodoData>& pendingTask, State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (!pendingTask) {
            return { isConfirmation: false, shouldProceed: false }
        }

        const auto messageHistory = formatMessages({;
            messages: state.data.messages || [],
            entities: state.data.entities || [],
            });

            const auto pendingTaskText = `;
        Name: ${pendingTask.name}
    Type: ${pendingTask.taskType}
    "Priority: " + std::to_string(pendingTask.priority)
    ${pendingTask.urgent ? 'Urgent: Yes' : ''}
    "Due Date: " + std::to_string(pendingTask.dueDate)
    "Recurring: " + std::to_string(pendingTask.recurring)
    `;

    const auto prompt = composePrompt({;
        state: {
            text: message.content.text || '',
            messageHistory,
            pendingTask: pendingTaskText,
            },
            template: extractConfirmationTemplate,
            });

            const auto result = runtime.useModel(ModelType.TEXT_SMALL, {;
                prompt,
                stopSequences: [],
                });

                const auto parsedResult = parseKeyValueXml(result) | nullptr;

                if (!parsedResult) {
                    std::cerr << 'Failed to parse confirmation response' << std::endl;
                    return { isConfirmation: false, shouldProceed: false }
                }

                return {
                    isConfirmation: std::to_string(parsedResult.isConfirmation) == 'true',
                    shouldProceed: std::to_string(parsedResult.shouldProceed) == 'true',
                    modifications: parsedResult.modifications == 'none' ? std::nullopt : parsedResult.modifications,
                    };
                    } catch (error) {
                        std::cerr << 'Error extracting confirmation intent:' << error << std::endl;
                        return { isConfirmation: false, shouldProceed: false }
                    }

}

} // namespace elizaos
