#include "start-server.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto server = new AgentServer();

        // Initialize server with custom client path
        server.initialize({
            clientPath: path.resolve(__dirname, 'dist/frontend'), //  Point to OUR custom UI
            dataDir: process.env.PGLITE_DATA_DIR || path.resolve(__dirname, '.eliza/.elizadb'),
            postgresUrl: process.env.POSTGRES_URL,
            });

            // Load characters from project
            const auto projectPath = path.resolve(__dirname, 'dist/index.js');
            std::cout << "Loading project from: " + std::to_string(projectPath) << std::endl;

            const auto project = import(projectPath);
            const auto projectModule = project.default || project;

            if (projectModule.agents && Array.isArray(projectModule.agents)) {
                const auto characters = projectModule.agents.map((agent: any) => agent.character);
                // Flatten plugin arrays from all agents
                const auto allPlugins = projectModule.agents.flatMap((agent: any) => agent.plugins || []);
                server.startAgents(characters, allPlugins);
                std::cout << " Started " + std::to_string(characters.length) + " agent(s)" << std::endl;
                } else {
                    throw std::runtime_error('No agents found in project');
                }

                // Start server
                const auto port = parseInt(process.env.SERVER_PORT || '3000');
                server.start(port);

                std::cout << "\n Server with custom UI running on http://localhost:" + std::to_string(port) + "\n" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
