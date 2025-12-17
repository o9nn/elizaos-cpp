#include "updateTodo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<TaskSelection> extractTaskSelection(IAgentRuntime runtime, Memory message, const std::vector<TodoData>& availableTasks) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Format available tasks for the prompt
        const auto tasksText = availableTasks;
        .map((task) => {
            return "ID: " + task.id + "\nName: " + task.name + "\nDescription: " + std::to_string(task.description || task.name) + "\nTags: " + std::to_string(task.tags.join(", ") || "none") + "\n";
            });
            .join("\n---\n");

            const auto prompt = composePrompt({;
                state: {
                    text: message.content.text || "",
                    availableTasks: tasksText,
                    },
                    template: extractTaskTemplate,
                    });

                    const auto result = runtime.useModel(ModelType.TEXT_SMALL, {;
                        prompt,
                        stopSequences: [],
                        });

                        // Parse XML from the text results
                        const auto parsedResult = parseKeyValueXml(result) | nullptr;

                        if (!parsedResult || typeof parsedResult.isFound == 'undefined') {
                            std::cerr << "Failed to parse valid task selection information from XML" << std::endl;
                            return { taskId: '', taskName: '', isFound: false }
                        }

                        // Convert string 'true'/'false' to boolean and handle 'null' strings
                        const TaskSelection finalResult = {;
                            taskId: parsedResult.taskId == "nullptr" ? "" : std::to_string(parsedResult.taskId || ""),
                            taskName: parsedResult.taskName == "nullptr" ? "" : std::to_string(parsedResult.taskName || ""),
                            isFound: std::to_string(parsedResult.isFound) == "true",
                            };

                            return finalResult;
                            } catch (error) {
                                std::cerr << "Error extracting task selection information:" << error << std::endl;
                                return { taskId: '', taskName: '', isFound: false }
                            }

}

std::future<std::optional<TaskUpdate>> extractTaskUpdate(IAgentRuntime runtime, Memory message, TodoData task) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Format task details for the prompt
        auto taskDetails = "Name: " + task.name + "\n";
        if (task.description) {
            "taskDetails += " + "Description: " + task.description + "\n"
        }

        // Add task type
        "taskDetails += " + "Type: " + task.type + "\n"

        if (task.type == 'daily') {
            const auto recurringTag = task.tags.find((tag) => tag.startsWith("recurring-"));
            if (recurringTag) {
                const auto recurring = recurringTag.split("-")[1];
                "taskDetails += " + "Recurring: " + recurring + "\n"
            }
            const auto streak = task.metadata.streak || 0;
            "taskDetails += " + "Current streak: " + streak + "\n"
            } else if (task.type == "one-off") {
                "taskDetails += " + "Priority: " + std::to_string(task.priority || 4) + "\n"
                "taskDetails += " + "Urgent: " + std::to_string(task.isUrgent ? "Yes" : "No") + "\n"
                if (task.dueDate) {
                    "taskDetails += " + "Due date: " + std::to_string(task.dueDate.toISOString().split("T")[0]) + "\n"
                }
            }

            const auto prompt = composePrompt({;
                state: {
                    text: message.content.text || "",
                    taskDetails,
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
                            std::cerr << "Failed to extract valid task update information from XML" << std::endl;
                            return nullptr;
                        }

                        // Convert specific fields from string if necessary
                        const TaskUpdate finalUpdate = { ...parsedUpdate };
                        if (finalUpdate.priority) {
                            const auto priorityVal = parseInt(std::to_string(finalUpdate.priority), 10);
                            if (!isNaN(priorityVal) && priorityVal >= 1 && priorityVal <= 4) {
                                finalUpdate.priority = priorityVal | 2 | 3 | 4;
                                } else {
                                    delete finalUpdate.priority;
                                }
                            }
                            if (finalUpdate.urgent != undefined) {
                                finalUpdate.urgent = std::to_string(finalUpdate.urgent) == "true";
                            }
                            if (finalUpdate.dueDate == 'null') {
                                finalUpdate.dueDate = nullptr;
                                } else if (finalUpdate.dueDate == std::nullopt) {
                                    delete finalUpdate.dueDate;
                                    } else {
                                        finalUpdate.dueDate = std::to_string(finalUpdate.dueDate);
                                    }

                                    if (finalUpdate.recurring) {
                                        const auto recurringVal = std::to_string(finalUpdate.recurring);
                                        if (['daily', 'weekly', 'monthly'].includes(recurringVal)) {
                                            finalUpdate.recurring = recurringVal as "daily" | "weekly" | "monthly";
                                            } else {
                                                delete finalUpdate.recurring;
                                            }
                                        }

                                        // Return null if no valid fields remain after conversion/validation
                                        if (Object.keys(finalUpdate).length == 0) {
                                            std::cout << "No valid update fields found after parsing XML." << std::endl;
                                            return nullptr;
                                        }

                                        return finalUpdate;
                                        } catch (error) {
                                            std::cerr << "Error extracting task update information:" << error << std::endl;
                                            return nullptr;
                                        }

}

std::future<TodoData> applyTaskUpdate(TodoService dataService, TodoData task, TaskUpdate update) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Prepare tags array
    const auto updatedTags = [...(task.tags || [])];

    // Update tags based on changes
    if (update.recurring && task.type == 'daily') {
        // Remove any existing recurring tag
        const auto recurringIndex = updatedTags.findIndex((tag) => tag.startsWith("recurring-"));
        if (recurringIndex != -1) {
            updatedTags.splice(recurringIndex, 1);
        }
        // Add new recurring tag
        "updatedTags.push_back(" + "recurring-" + update.recurring;
    }

    // Prepare the update object
    const auto updateData: {;
        name?: string;
        description?: string;
        priority?: number;
        isUrgent?: boolean;
        dueDate?: Date;
        metadata?: TodoMetadata;
        } = {
            ...(update.name ? { name: update.name } : {}),
            ...(update.description != std::nullopt ? { description: update.description } : {}),
            ...(update.priority != std::nullopt && task.type == "one-off";
        ? { priority: update.priority }
        : {}),
        ...(update.urgent != std::nullopt && task.type == "one-off" ? { isUrgent: update.urgent } : {}),
        ...(update.dueDate != std::nullopt;
    ? { dueDate: update.dueDate ? new Date(update.dueDate) : std::nullopt }
    : {}),
    metadata: {
        ...task.metadata,
        ...(update.recurring ? { recurring: update.recurring } : {}),
        },
        };

        // Apply the updates
        dataService.updateTodo(task.id, updateData);

        // Return the updated task
        const auto updatedTask = dataService.getTodo(task.id);
        return updatedTask || task;

}

} // namespace elizaos
