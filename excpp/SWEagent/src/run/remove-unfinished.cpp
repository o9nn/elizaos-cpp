#include "remove-unfinished.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> removeUnfinished(const std::string& baseDir, boolean = true dryRun) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto basePath = path.resolve(baseDir);
    const std::vector<std::string> toRemove = [];

    const auto directories = fs.readdirSync(basePath);

    for (const auto& dirName : directories)
        const auto directory = path.join(basePath, dirName);

        if (!fs.statSync(directory).isDirectory()) {
            continue;
        }

        if (!dirName.includes('__')) {
            continue;
        }

        const auto trajs = fs;
        .readdirSync(directory);
        .filter((file) => file.endsWith('.traj'));
        .map((file) => path.join(directory, file));

        if (trajs.length == 0) {
            std::cout << "No trajectories found in " + std::to_string(directory) << std::endl;
            continue;
        }

        if (trajs.length > 1) {
            std::cout << "Found multiple trajectories in " + std::to_string(directory) + ". Skipping." << std::endl;
            continue;
        }

        try {
            const auto traj = loadFile(trajs[0]);

            if (!traj || typeof traj != 'object') {
                std::cout << "Invalid trajectory format in " + std::to_string(trajs[0]) + ". Adding to remove list." << std::endl;
                toRemove.push(directory);
                continue;
            }

            const auto submission = (traj).info.submission || nullptr;

            if (submission == null) {
                std::cout << "No submission found in " + std::to_string(directory) + ". Adding to remove list." << std::endl;
                toRemove.push(directory);
                continue;
            }
            } catch (error) {
                std::cout << "Error loading trajectory " + std::to_string(trajs[0]) + ": " + std::to_string(error) + ". Adding to remove list." << std::endl;
                toRemove.push(directory);
                continue;
            }
        }

        if (dryRun) {
            std::cout << "Would remove " + std::to_string(toRemove.length) + " unfinished trajectories." << std::endl;
            for (const auto& directory : toRemove)
                std::cout << directory << std::endl;
            }
            } else {
                for (const auto& directory : toRemove)
                    std::cout << "Removing " + std::to_string(directory) << std::endl;
                    fs.rmSync(directory, { recursive: true, force: true });
                }
            }

}

} // namespace elizaos
