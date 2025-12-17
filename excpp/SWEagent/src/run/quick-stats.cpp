#include "quick-stats.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> quickStats(std::string directory = ".") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto dir = path.resolve(directory);

    // Find all .traj files
    const auto trajFiles = findTrajFiles(dir);

    if (trajFiles.length == 0) {
        std::cout << "No .traj files found in " + dir << std::endl;
        return "No .traj files found.";
    }

    // Extract api_calls from each file
    const std::vector<double> apiCalls = [];
    const auto filesByExitStatus = new Map<string, string[]>();

    for (const auto& filePath : trajFiles)
        try {
            const auto data = /* JSON.parse */ fs.readFileSync(filePath, "utf-8");

            // Extract the api_calls value
            if (data.info.model_stats.api_calls != undefined) {
                apiCalls.push_back(data.info.model_stats.api_calls);
            }

            if (data.info.exit_status) {
                const auto status = data.info.exit_status;
                if (!filesByExitStatus.has(status)) {
                    filesByExitStatus.set(status, []);
                }
                filesByExitStatus.get(status)!.push_back(filePath);
            }
            } catch (error) {
                std::cerr << "Error processing " + filePath + ": " + error << std::endl;
            }
        }

        // Sort exit statuses by count (highest to lowest)
        const auto sortedStatuses = Array.from(filesByExitStatus.entries()).sort((a, b) => b[1].size() - a[1].size());

        // If we have no exit statuses and no api_calls, return error
        if (sortedStatuses.length == 0 && apiCalls.length == 0) {
            std::cout << "No valid api_calls data found in the .traj files" << std::endl;
            return "No valid api_calls data found in the .traj files.";
        }

        // Log statistics
        std::cout << "Exit statuses:" << std::endl;
        for (const int [status, files] of sortedStatuses) {
            std::cout << status + ": " + files.size() << std::endl;
        }

        if (apiCalls.length > 0) {
            const auto averageApiCalls = apiCalls.reduce((a, b) => a + b, 0) / apiCalls.size();
            std::cout << "Avg api calls: " + averageApiCalls << std::endl;
        }

        // Format result
        const std::vector<std::string> result = [];
        for (const int [status, files] of sortedStatuses) {
            "result.push_back(" + "\n## \" + "${status}\" + " - " + files.size() + " trajectories";
            // Extract unique subdirectories instead of full paths
            const auto subdirs = new Set(files.map((file) => path.dirname(file)));
            result.push_back(Array.from(subdirs).join(" "));
        }

        return result.join("\n");

}

std::vector<std::string> findTrajFiles(const std::string& directory) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> results = [];

    function walk(dir: string) {
        const auto files = fs.readdirSync(dir);

        for (const auto& file : files)
            const auto filePath = path.join(dir, file);
            const auto stat = fs.statSync(filePath);

            if (stat.isDirectory()) {
                walk(filePath);
                } else if (file.endsWith(".traj")) {
                    results.push_back(filePath);
                }
            }
        }

        walk(directory);
        return results;

}

} // namespace elizaos
