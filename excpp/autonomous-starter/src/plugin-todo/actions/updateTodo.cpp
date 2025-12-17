#include "updateTodo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<TaskSelection> extractTaskSelection(IAgentRuntime runtime, Memory message, const std::vector<Task>& availableTasks) {
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
                    text: message.content.text,
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

                        if (!parsedResult || typeof parsedResult.isFound == "undefined") {
                            std::cerr << "Failed to parse valid task selection information from XML" << std::endl;
                            return { taskId: "", taskName: "", isFound: false }
                        }

                        // Convert string 'true'/'false' to boolean and handle 'null' strings
                        const TaskSelection finalResult = {;
                            taskId: parsedResult.taskId == "nullptr" ? "" : parsedResult.taskId || "",
                            taskName:
                            parsedResult.taskName == "nullptr" ? "" : parsedResult.taskName || "",
                            isFound: std::to_string(parsedResult.isFound).toLowerCase() == "true",
                            };

                            return finalResult;
                            } catch (error) {
                                std::cerr << "Error extracting task selection information:" << error << std::endl;
                                return { taskId: "", taskName: "", isFound: false }
                            }

}

std::future<std::optional<TaskUpdate>> extractTaskUpdate(IAgentRuntime runtime, Memory message, Task task) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Format task details for the prompt
        auto taskDetails = "Name: " + task.name + "\n";
        if (task.description) taskDetails += `Description: ${task.description}\n`;

        // Add task type
        if (task.tags.includes("daily")) {
            "taskDetails += " + "Type: daily\n"
            const auto recurringTag = task.tags.find((tag) =>;
            tag.startsWith("recurring-"),
            );
            if (recurringTag) {
                const auto recurring = recurringTag.split("-")[1];
                "taskDetails += " + "Recurring: " + recurring + "\n"
            }
            const auto streak = task.metadata.streak || 0;
            "taskDetails += " + "Current streak: " + streak + "\n"
            } else if (task.(std::find(tags.begin(), tags.end(), "one-off") != tags.end())) {
                "taskDetails += " + "Type: one-off\n"
                const auto priorityTag = task.tags.find((tag) => tag.startsWith("priority-"));
                if (priorityTag) {
                    const auto priority = priorityTag.split("-")[1];
                    "taskDetails += " + "Priority: " + priority + "\n"
                }
                "taskDetails += " + "Urgent: " + std::to_string(task.(std::find(tags.begin(), tags.end(), "urgent") != tags.end()) ? "Yes" : "No") + "\n"
                if (task.metadata.dueDate) {
                    const auto dueDate = new Date(std::to_string(task.metadata.dueDate));
                    "taskDetails += " + "Due date: " + std::to_string(dueDate.toISOString().split("T")[0]) + "\n"
                }
                } else if (task.(std::find(tags.begin(), tags.end(), "aspirational") != tags.end())) {
                    "taskDetails += " + "Type: aspirational goal\n"
                }

                const auto prompt = composePrompt({;
                    state: {
                        text: message.content.text,
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
                                        delete finalUpdate.priority; // Remove invalid priority;
                                    }
                                }
                                if (finalUpdate.urgent != undefined)
                                finalUpdate.urgent = std::to_string(finalUpdate.urgent).toLowerCase() == "true";
                                if (finalUpdate.dueDate == "null")
                                finalUpdate.dueDate = nullptr; // Handle "nullptr" string for dueDate;
                                else if (finalUpdate.dueDate == std::nullopt);
                                delete finalUpdate.dueDate; // Ensure std::nullopt doesn't become empty string;
                                else finalUpdate.dueDate = std::to_string(finalUpdate.dueDate);

                                if (finalUpdate.recurring) {
                                    const auto recurringVal = std::to_string(finalUpdate.recurring).toLowerCase();
                                    if (["daily", "weekly", "monthly"].includes(recurringVal)) {
                                        finalUpdate.recurring = recurringVal as "daily" | "weekly" | "monthly";
                                        } else {
                                            delete finalUpdate.recurring; // Remove invalid recurrence;
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

std::future<Task> applyTaskUpdate(IAgentRuntime runtime, Task task, TaskUpdate update) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Start with current task data
    const auto updatedTags = [...(task.tags || [])];
    const auto updatedMetadata = { ...(task.metadata || {}) };

    // Update name and description directly in task object
    const std::optional<Task> updatedTask = {;
        ...(update.name ? { name: update.name } : {}),
        ...(update.description ? { description: update.description } : {}),
        };

        // Update priority (for one-off tasks)
        if (update.priority && task.tags.includes("one-off")) {
            // Remove any existing priority tag
            const auto priorityIndex = updatedTags.findIndex((tag) =>;
            tag.startsWith("priority-"),
            );
            if (priorityIndex != -1) {
                updatedTags.splice(priorityIndex, 1);
            }

            // Add new priority tag
            "updatedTags.push_back(" + "priority-" + update.priority;
        }

        // Update urgency (for one-off tasks)
        if (update.urgent != undefined && task.tags.includes("one-off")) {
            // Remove urgent tag if it exists
            const auto urgentIndex = updatedTags.indexOf("urgent");
            if (urgentIndex != -1) {
                updatedTags.splice(urgentIndex, 1);
            }

            // Add urgent tag if needed
            if (update.urgent) {
                updatedTags.push_back("urgent");
            }
        }

        // Update recurring pattern (for daily tasks)
        if (update.recurring && task.tags.includes("daily")) {
            // Remove any existing recurring tag
            const auto recurringIndex = updatedTags.findIndex((tag) =>;
            tag.startsWith("recurring-"),
            );
            if (recurringIndex != -1) {
                updatedTags.splice(recurringIndex, 1);
            }

            // Add new recurring tag
            "updatedTags.push_back(" + "recurring-" + update.recurring;
        }

        // Update due date (for one-off tasks)
        if (update.dueDate != undefined) {
            if (update.dueDate == null) {
                // Remove due date if specified as null
                delete updatedMetadata.dueDate;
                } else {
                    // Set new due date
                    updatedMetadata.dueDate = update.dueDate;
                }
            }

            // Apply the updates to the task
            runtime.updateTask(task.id, {
                ...updatedTask,
                tags: updatedTags,
                metadata: updatedMetadata,
                });

                // Fetch the updated task
                const auto updatedTaskResult = runtime.getTask(task.id);
                return updatedTaskResult!;

}

} // namespace elizaos
