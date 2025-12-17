#include "tsk_sentiment-signal.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void setupSentimentGenerator(auto runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    worldId = runtime.agentId; // this is global data for the agent;

    // first, get all tasks with tags "queue", "repeat", "degen_intel" and delete them
    const auto tasks = runtime.getTasks({;
        tags: ['queue', 'repeat', 'plugin_trader'],
        });

        for (const auto& task : tasks)
            runtime.deleteTask(task.id);
        }

        // shouldn't plugin-solana and plugin-evm handle this?
        runtime.registerTaskWorker({
            name: 'TRADER_SYNC_SENTIMENT',
            validate: async (_runtime, _message, _state) => {
                return true; // TODO: validate after certain time;
                },
                execute: async (runtime, _options, task) => {
                    try {
                        // do the thing
                        std::cout << 'PLUGIN_TRADER_SENTIMENT' << std::endl;
                        runtime.emitEvent('PLUGIN_TRADER_SENTIMENT', {});
                        } catch (error) {
                            std::cerr << 'Failed to sync sentiment' << error << std::endl;
                            // kill this task
                            //await runtime.deleteTask(task.id);
                        }
                        },
                        });

                        runtime.createTask({
                            name: 'TRADER_SYNC_SENTIMENT',
                            description: 'calculate sentiment',
                            worldId,
                            metadata: {
                                createdAt: Date.now(),
                                updatedAt: Date.now(),
                                updateInterval: 1000 * 60 * 5, // 5 minutes
                                },
                                tags: ['queue', 'repeat', 'plugin_trader', 'immediate'],
                                });

}

} // namespace elizaos
