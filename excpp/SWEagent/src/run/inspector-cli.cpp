#include "inspector-cli.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<std::string> findTrajFiles(const std::string& dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> results = [];

    function walk(directory: string) {
        const auto files = fs.readdirSync(directory);

        for (const auto& file : files)
            const auto filePath = path.join(directory, file);
            const auto stat = fs.statSync(filePath);

            if (stat.isDirectory()) {
                walk(filePath);
                } else if (file.endsWith('.traj')) {
                    results.push(filePath);
                }
            }
        }

        if (fs.statSync(dir).isDirectory()) {
            walk(dir);
            } else {
                results.push(dir);
            }

            return results;

}

std::future<void> inspectorCli(string = '::' trajectoryPath, std::optional<std::string> dataPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto trajPath = path.resolve(trajectoryPath);

        if (!fs.existsSync(trajPath)) {
            throw std::runtime_error(`Path does not exist: ${trajPath}`);
        }

        const auto trajFiles = findTrajFiles(trajPath);

        if (trajFiles.length == 0) {
            throw std::runtime_error('No trajectory files found');
        }

        if (trajFiles.length == 1) {
            // Single file - open directly
            const auto inspector = new TrajectoryInspector(trajFiles[0], dataPath);
            inspector.run();
            } else {
                // Multiple files - show selection menu
                std::cout << 'Multiple trajectory files found:' << std::endl;
                trajFiles.forEach((file, index) => {
                    std::cout << "  " + std::to_string(index + 1) + ". " + std::to_string(file) << std::endl;
                    });

                    const auto rl = readline.createInterface({;
                        input: process.stdin,
                        output: process.stdout,
                        });

                        const auto answer = new Promise<string>((resolve) => {;
                            rl.question('\nSelect file number (or q to quit): ', resolve);
                            });

                            rl.close();

                            if (answer == 'q') {
                                process.exit(0);
                            }

                            const auto index = parseInt(answer, 10) - 1;
                            if (index >= 0 && index < trajFiles.length) {
                                const auto inspector = new TrajectoryInspector(trajFiles[index], dataPath);
                                inspector.run();
                                } else {
                                    std::cout << 'Invalid selection' << std::endl;
                                    process.exit(1);
                                }
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
