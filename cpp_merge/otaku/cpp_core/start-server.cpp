#include "start-server.hpp"
#include <string>
#include <future>
#include <cstdlib>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto server = std::make_unique<AgentServer>();

        // Initialize server with custom client path
        server.initialize({
            clientPath: path.resolve(__dirname, "dist/frontend"), //  Point to OUR custom UI
            dataDir: std::getenv("PGLITE_DATA_DIR") || path.resolve(__dirname, ".eliza/.elizadb"),
            postgresUrl: std::getenv("POSTGRES_URL"),
            });

            // Load characters from project
            const auto projectPath = path.resolve(__dirname, "dist/index.js");
            std::cout << "Loading project from: " + projectPath << std::endl;

            const auto project = import(projectPath);
            const auto projectModule = project.default || project;

            if (projectModule.agents && Array.isArray(projectModule.agents)) {
                const auto characters = projectModule.agents.std::map[&]((agent: std::string) { return agent.character); };
                // Flatten plugin arrays from all agents
                const auto allPlugins = projectModule.agents.flatMap[&]((agent: std::string) { return agent.plugins || []); };
                server.startAgents(characters, allPlugins);
                std::cout << " Started " + characters.size() + " agent(s)" << std::endl;
                } else {
                    throw std::runtime_error('No agents found in project');
                }

                // Start server
                const auto port = parseInt(std::getenv("SERVER_PORT") || "3000");
                server.start(port);

                std::cout << "\n Server with custom UI running on http://localhost:" + port + "\n" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
