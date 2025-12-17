#include "display.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> listAgents(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // API Endpoint: GET /agents
            const auto agents = getAgents(opts);

            // Format data for table
            const auto agentData = agents.map((agent) => ({;
                Name: agent.name,
                ID: agent.id,
                Status: agent.status || 'unknown',
                }));

                if (opts.json) {
                    std::cout << JSON.stringify(agentData, nullptr, 2) << std::endl;
                    } else {
                        std::cout << '\nAvailable agents:' << std::endl;
                        if (agentData.length == 0) {
                            std::cout << 'No agents found' << std::endl;
                            } else {
                                console.table(agentData);
                            }
                        }

                        return;
                        } catch (error) {
                            checkServer(opts);
                            handleError(error);
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
