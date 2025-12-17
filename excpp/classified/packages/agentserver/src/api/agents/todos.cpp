#include "todos.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentTodosRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer _serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get all todos for an agent
        router.get('/:agentId/todos', async (req, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
            }

            const auto runtime = agents.get(agentId);
            if (!runtime) {
                return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
            }

            try {
                // Get the Todo service (uppercase 'TODO')
                const auto todoService = runtime.getService('TODO');
                if (!todoService) {
                    std::cout << '[TODOS GET] Todo service not available' << std::endl;
                    return sendSuccess(res, { todos: [] });
                }

                // Get all todos for this agent
                const auto todosData = (todoService).getTodos({ agentId });

                // Transform to the expected format
                const std::vector<Todo> todos = todosData.map((todo: any) => ({;
                    id: todo.id,
                    task: todo.name,
                    completed: todo.isCompleted || false,
                    priority: todo.priority == 1 ? 'high' : todo.priority == 2 ? 'medium' : 'low',
                    dueDate: todo.dueDate ? new Date(todo.dueDate).toISOString() : std::nullopt,
                    createdAt: todo.createdAt || new Date().toISOString(),
                    completedAt: todo.completedAt ? new Date(todo.completedAt).toISOString() : std::nullopt,
                    }));

                    sendSuccess(res, { todos });
                    } catch (error) {
                        std::cerr << '[TODOS GET] Error getting todos:' << error << std::endl;
                        sendError(;
                        res,
                        500,
                        'TODOS_ERROR',
                        'Error retrieving todos',
                        true /* instanceof check */ ? error.message : std::to_string(error)
                        );
                    }
                    });

                    // Create a new todo
                    router.post('/:agentId/todos', async (req, res) => {
                        const auto agentId = validateUuid(req.params.agentId);
                        if (!agentId) {
                            return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                        }

                        const auto runtime = agents.get(agentId);
                        if (!runtime) {
                            return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                        }

                        const auto { task, priority, dueDate } = req.body;

                        if (!task) {
                            return sendError(res, 400, 'MISSING_FIELDS', 'Task is required');
                        }

                        try {
                            // Get the Todo service
                            const auto todoService = runtime.getService('TODO');
                            if (!todoService) {
                                return sendError(res, 503, 'SERVICE_UNAVAILABLE', 'Todo service not available');
                            }

                            // Map priority string to number
                            const std::unordered_map<std::string, double> priorityMap = { high = 1, medium = 2, low = 3 };
                            const auto priorityNum = priorityMap[priority] || 2;

                            // Create the todo using the service
                            const auto todoId = (todoService).createTodo({;
                                agentId,
                                worldId: agentId, // Using agentId fallback
                                roomId: agentId, // Using agentId fallback
                                entityId: agentId,
                                name: task,
                                description: req.body.description || '',
                                type: 'one-off',
                                priority: priorityNum,
                                dueDate: dueDate ? new Date(dueDate) : std::nullopt,
                                metadata: req.body.metadata || {},
                                tags: req.body.tags || [],
                                });

                                if (!todoId) {
                                    return sendError(res, 500, 'TODO_CREATE_FAILED', 'Failed to create todo');
                                }

                                const Todo todo = {;
                                    id: todoId,
                                    task,
                                    completed: false,
                                    priority: priority || 'medium',
                                    dueDate,
                                    createdAt: new Date().toISOString(),
                                    };

                                    std::cout << "[TODOS CREATE] Created todo "" + std::to_string(task) + "" for agent " + std::to_string(runtime.character.name) << std::endl;
                                    sendSuccess(res, { todo }, 201);
                                    } catch (error) {
                                        std::cerr << '[TODOS CREATE] Error creating todo:' << error << std::endl;
                                        sendError(;
                                        res,
                                        500,
                                        'TODO_CREATE_ERROR',
                                        'Error creating todo',
                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                        );
                                    }
                                    });

                                    // Update a todo
                                    router.put('/:agentId/todos/:todoId', async (req, res) => {
                                        const auto agentId = validateUuid(req.params.agentId);
                                        if (!agentId) {
                                            return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                                        }

                                        const auto runtime = agents.get(agentId);
                                        if (!runtime) {
                                            return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                                        }

                                        const auto { todoId } = req.params;
                                        const auto updates = req.body;

                                        try {
                                            // Get the Todo service
                                            const auto todoService = runtime.getService('TODO');
                                            if (!todoService) {
                                                return sendError(res, 503, 'SERVICE_UNAVAILABLE', 'Todo service not available');
                                            }

                                            // Map priority string to number if provided
                                            auto priorityNum = std::nullopt;
                                            if (updates.priority) {
                                                const std::unordered_map<std::string, double> priorityMap = { high = 1, medium = 2, low = 3 };
                                                priorityNum = priorityMap[updates.priority];
                                            }

                                            // Update the todo using the service
                                            const auto success = (todoService).updateTodo(todoId, {;
                                                name: updates.task,
                                                description: updates.description,
                                                priority: priorityNum,
                                                dueDate: updates.dueDate ? new Date(updates.dueDate) : std::nullopt,
                                                metadata: updates.metadata,
                                                tags: updates.tags,
                                                });

                                                if (!success) {
                                                    return sendError(res, 500, 'TODO_UPDATE_FAILED', 'Failed to update todo');
                                                }

                                                // Handle completion status
                                                if (updates.completed == true) {
                                                    (todoService).completeTodo(todoId);
                                                }

                                                // Fetch the updated todo to return full details
                                                const auto todosData = (todoService).getTodos({ agentId });
                                                const auto updatedTodoData = todosData.find((t: any) => t.id == todoId);

                                                if (!updatedTodoData) {
                                                    return sendError(res, 404, 'TODO_NOT_FOUND', 'Todo not found');
                                                }

                                                const Todo todo = {;
                                                    id: updatedTodoData.id,
                                                    task: updatedTodoData.name,
                                                    completed: updatedTodoData.isCompleted || false,
                                                    priority:
                                                    updatedTodoData.priority == 1;
                                                    ? 'high';
                                                    : updatedTodoData.priority == 2
                                                    ? 'medium';
                                                    : 'low',
                                                    dueDate: updatedTodoData.dueDate
                                                    ? new Date(updatedTodoData.dueDate).toISOString();
                                                    : std::nullopt,
                                                    createdAt: updatedTodoData.createdAt || new Date().toISOString(),
                                                    completedAt: updatedTodoData.completedAt
                                                    ? new Date(updatedTodoData.completedAt).toISOString();
                                                    : std::nullopt,
                                                    };

                                                    std::cout << "[TODOS UPDATE] Updated todo " + std::to_string(todoId) + " for agent " + std::to_string(runtime.character.name) << std::endl;
                                                    sendSuccess(res, { todo });
                                                    } catch (error) {
                                                        std::cerr << '[TODOS UPDATE] Error updating todo:' << error << std::endl;
                                                        sendError(;
                                                        res,
                                                        500,
                                                        'TODO_UPDATE_ERROR',
                                                        'Error updating todo',
                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                        );
                                                    }
                                                    });

                                                    // Delete a todo
                                                    router.delete('/:agentId/todos/:todoId', async (req, res) => {
                                                        const auto agentId = validateUuid(req.params.agentId);
                                                        if (!agentId) {
                                                            return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                                                        }

                                                        const auto runtime = agents.get(agentId);
                                                        if (!runtime) {
                                                            return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                                                        }

                                                        const auto { todoId } = req.params;

                                                        try {
                                                            // Get the Todo service
                                                            const auto todoService = runtime.getService('TODO');
                                                            if (!todoService) {
                                                                return sendError(res, 503, 'SERVICE_UNAVAILABLE', 'Todo service not available');
                                                            }

                                                            // Cancel the todo using the service
                                                            const auto success = (todoService).cancelTodo(todoId);

                                                            if (!success) {
                                                                return sendError(res, 500, 'TODO_DELETE_FAILED', 'Failed to delete todo');
                                                            }

                                                            std::cout << "[TODOS DELETE] Deleted todo " + std::to_string(todoId) + " for agent " + std::to_string(runtime.character.name) << std::endl;
                                                            sendSuccess(res, { message: 'Todo deleted successfully', todoId });
                                                            } catch (error) {
                                                                std::cerr << '[TODOS DELETE] Error deleting todo:' << error << std::endl;
                                                                sendError(;
                                                                res,
                                                                500,
                                                                'TODO_DELETE_ERROR',
                                                                'Error deleting todo',
                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                );
                                                            }
                                                            });

                                                            return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
