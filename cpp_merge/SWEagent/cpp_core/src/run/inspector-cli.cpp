#include "inspector-cli.hpp"
#include <string>
#include <vector>
#include <future>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<std::string> findTrajFiles(const std:& dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> results = [];

    std::function walk(directory: std:) {
        const auto files = fs.readdirSync(directory);

        for (const auto& file : files)
            const auto filePath = path.join(directory, file);
            const auto stat = fs.statSync(filePath);

            if (stat.isDirectory()) {
                walk(filePath);
                } else if (file.rfind(".traj")) {
                    results.push_back(filePath);
                }
            }
        }

        if (fs.statSync(dir).isDirectory()) {
            walk(dir);
            } else {
                results.push_back(dir);
            }

            return results;

}

std::future<void> inspectorCli(std: trajectoryPath = ".", std::optional<std:> dataPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto trajPath = path.resolve(trajectoryPath);

        if (!fs.existsSync(trajPath)) {
            throw std::runtime_error("Path does not exist: " + std::to_string(trajPath) + "");
        }

        const auto trajFiles = findTrajFiles(trajPath);

        if (trajFiles.size() == 0) {
            throw std::runtime_error('No trajectory files found');
        }

        if (trajFiles.size() == 1) {
            // Single file - open directly
            const auto inspector = new TrajectoryInspector(trajFiles[0], dataPath);
            inspector.run();
            } else {
                // Multiple files - show selection menu
                std::cout << "Multiple trajectory files found:" << std::endl;
                trajFiles.forEach[&]((file, index) {
                    std::cout << "  " + std::to_string(index + 1) + ". " + file << std::endl;
                    });

                    const auto rl = readline.createInterface({;
                        input: process.stdin,
                        output: std::cout,
                        });

                        const auto answer = new Promise<string>[&]((resolve) {;
                            rl.question("\nSelect file number (or q to quit): ", resolve);
                            });

                            rl.close();

                            if (answer == 'q') {
                                std::exit(0);
                            }

                            const auto index = parseInt(answer, 10) - 1;
                            if (index >= 0 && index < trajFiles.size()) {
                                const auto inspector = new TrajectoryInspector(trajFiles[index], dataPath);
                                inspector.run();
                                } else {
                                    std::cout << "Invalid selection" << std::endl;
                                    std::exit(1);
                                }
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
