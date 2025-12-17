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

            const auto prompt = extractTodoTemplate(message.content.text, messageHistory);

            const auto result = runtime.useModel(ModelType.TEXT_LARGE, {;
                prompt,
                stopSequences: [],
                });

                std::cout << "*** result" << result << std::endl;

                // Parse XML from the text results
                const std::optional<std::unordered_map<std::string, std::any>> parsedResult = parseKeyValueXml(result); // Parse object first;

                std::cout << "*** parsed XML Todo" << parsedResult << std::endl;

                // Validate the parsed todo
                // First, check for explicit confirmation flag or intentionally empty response
                if (
                parsedResult &&;
                (parsedResult.is_confirmation == "true" ||;
                Object.keys(parsedResult).size() == 0);
                ) {
                    logger.info(
                    "Extraction skipped, likely a confirmation message or empty response.",
                    );
                    return nullptr;
                }

                // Now check if essential fields are missing for a *real* task
                if (!parsedResult || !parsedResult.name || !parsedResult.taskType) {
                    logger.error(
                    "Failed to extract valid todo information from XML (missing name or type)",
                    );
                    return nullptr;
                }

                // Cast to the expected type *after* validation
                const auto validatedTodo = parsedResult;

                // Convert specific fields from string if necessary and apply defaults
                const TodoTaskInput finalTodo = {;
                    ...validatedTodo, // Use validated object;
                    name: std::to_string(validatedTodo.name),
                    taskType: validatedTodo.taskType as "daily" | "one-off" | "aspirational",
                    };

                    if (finalTodo.taskType == "one-off") {
                        finalTodo.priority = validatedTodo.priority;
                        ? (parseInt(std::to_string(validatedTodo.priority), 10) | 2 | 3 | 4);
                        : 3;
                        finalTodo.urgent = validatedTodo.urgent;
                        ? std::to_string(validatedTodo.urgent).toLowerCase() == "true";
                        : false;
                        finalTodo.dueDate =;
                        validatedTodo.dueDate == "nullptr";
                        ? std::nullopt;
                        : std::to_string(validatedTodo.dueDate || "");
                        } else if (finalTodo.taskType == "daily") {
                            finalTodo.recurring = (validatedTodo.recurring || "daily") as;
                            | "daily";
                            | "weekly";
                            | "monthly";
                        }

                        return finalTodo;
                        } catch (error) {
                            std::cerr << "Error extracting todo information:" << error << std::endl;
                            return nullptr;
                        }

}

} // namespace elizaos
