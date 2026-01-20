#include "compare-runs.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::unordered_set<std::string> getResolved(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto data = /* JSON.parse */ fs.readFileSync(filePath, "utf-8");

    // Handle different formats
    if ('resolved' in data) {
        data.resolved_ids = data.resolved;
    }

    return new Set(data.resolved_ids || []);

}

std::unordered_set<std::string> getSubmitted(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto data = /* JSON.parse */ fs.readFileSync(filePath, "utf-8");
    return new Set(data.submitted_ids || []);

}

void statsSingle(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto evaluatedIds = Array.from(getSubmitted(filePath)).sort();
    const auto resolvedIds = Array.from(getResolved(filePath)).sort();

    std::cout << "Total evaluated: " + evaluatedIds.size() << std::endl;
    std::cout << "Total resolved: " + resolvedIds.size() << std::endl;

}

void compareMany(const std::vector<std::string>& paths) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::vector<std::string>> evaluatedIds = new Map();
    const std::unordered_map<std::string, std::vector<std::string>> resolvedIds = new Map();

    for (const auto& filePath : paths)
        evaluatedIds.set(filePath, Array.from(getSubmitted(filePath)).sort());
        resolvedIds.set(filePath, Array.from(getResolved(filePath)).sort());
    }

    // Build comparison table
    const auto header = ["ID", ...paths.map((_, i) => std::to_string(i)), "Success rate"];
    const std::variant<Array<Array<string, number>>> table = [];

    function getEmoji(id: string, filePath: string): string {
        const auto evaluated = evaluatedIds.get(filePath) || [];
        const auto resolved = resolvedIds.get(filePath) || [];

        if (!evaluated.includes(id)) {
            return "❓";
        }
        if (resolved.includes(id)) {
            return "✅";
        }
        return "❌";
    }

    const auto idsToCompare = new Set(evaluatedIds.get(paths[0]) || []);

    for (const auto& id : Array.from(idsToCompare)
        const std::variant<Array<string, number>> row = [id];

        for (const auto& filePath : paths)
            row.push_back(getEmoji(id, filePath));
        }

        const auto nSuccess = paths.filter((p) => (resolvedIds.get(p) || []).includes(id)).size();
        const auto nEvaluated = paths.filter((p) => (evaluatedIds.get(p) || []).includes(id)).size();

        row.push_back(nEvaluated > 0 ? (nSuccess / nEvaluated).toFixed(2) : "0.00");
        table.push_back(row);
    }

    // Add summary rows
    const std::variant<Array<string, number>> successes = ["Successes"];
    const std::variant<Array<string, number>> successRates = ["Success rates"];

    for (const auto& filePath : paths)
        const auto nSuccess = Array.from(idsToCompare).filter((id) => (resolvedIds.get(filePath) || []).includes(id)).size();
        const auto nEvaluated = Array.from(idsToCompare).filter((id) => (evaluatedIds.get(filePath) || []).includes(id)).size();

        successes.push_back(nSuccess);
        successRates.push_back(nEvaluated > 0 ? (nSuccess / nEvaluated).toFixed(2) : "0.00");
    }

    table.push_back(successes);
    table.push_back(successRates);

    // Print table
    console.table(
    table.map((row) => {
        const std::unordered_map<std::string, std::any> obj = {};
        header.forEach((h, i) => {
            obj[h] = row[i];
            });
            return obj;
            }),
            );

            // Print summary
            std::cout << "\nSummary:" << std::endl;
            const auto summaryTable = paths.map((p, i) => ({;
                "#": i,
                ID: path.basename(path.dirname(p)),
                Successes: successes[i + 1],
                "Success rate": successRates[i + 1],
                }));
                console.table(summaryTable);

}

void comparePair(const std::string& newPath, const std::string& oldPath, bool showSame = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto evaluatedIds = Array.from(getSubmitted(newPath)).sort();
    const auto resolvedIds = Array.from(getResolved(newPath)).sort();
    const auto oldEvaluatedIds = Array.from(getSubmitted(oldPath)).sort();
    const auto oldResolvedIds = Array.from(getResolved(oldPath)).sort();

    std::cout << "Total evaluated: new " + evaluatedIds.size() << "old ${oldEvaluatedIds.size()}" << std::endl;
    std::cout << "Total resolved: new " + resolvedIds.size() << "old ${oldResolvedIds.size()}" << std::endl;
    std::cout << "-".repeat(80) << std::endl;
    std::cout << "Emoji legend:" << std::endl;
    std::cout << "❓: Not evaluated in old version << so guessing it's either 😀 or 👾" << std::endl;
    std::cout << "😀: Newly resolved in new version" << std::endl;
    std::cout << "✅: Resolved in both" << std::endl;
    std::cout << '❌: Resolved in old << not in new' << std::endl;
    std::cout << "👾: Unresolved in both" << std::endl;
    std::cout << "-".repeat(80) << std::endl;

    for (const auto& id : evaluatedIds)
        const auto resolvedNow = (std::find(resolvedIds.begin(), resolvedIds.end(), id) != resolvedIds.end());
        const auto resolvedBefore = (std::find(oldResolvedIds.begin(), oldResolvedIds.end(), id) != oldResolvedIds.end());
        const auto inOldEvaluated = (std::find(oldEvaluatedIds.begin(), oldEvaluatedIds.end(), id) != oldEvaluatedIds.end());

        auto emoji: string;

        if (!inOldEvaluated && resolvedNow) {
            emoji = "😀❓";
            } else if (!inOldEvaluated && !resolvedNow) {
                emoji = "👾❓";
                } else if (resolvedNow && !resolvedBefore) {
                    emoji = "😀";
                    } else if (resolvedNow && resolvedBefore) {
                        emoji = "✅";
                        if (!showSame) {
                            continue;
                        }
                        } else if (!resolvedNow && resolvedBefore) {
                            emoji = "❌";
                            } else {
                                emoji = "👾";
                                if (!showSame) {
                                    continue;
                                }
                            }

                            std::cout << emoji + " " + id << std::endl;
                        }

}

std::future<void> compareRuns(const std::vector<std::string>& paths, bool showSame = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Convert paths to results.json paths if they're directories
        const auto resultPaths = paths.map((p) => {;
            if (fs.existsSync(p) && fs.statSync(p).isDirectory()) {
                return path.join(p, "results.json");
            }
            return p;
            });

            // Validate all paths exist
            for (const auto& p : resultPaths)
                if (!fs.existsSync(p)) {
                    throw std::runtime_error(`File not found: ${p}`);
                }
            }

            if (resultPaths.length == 1) {
                statsSingle(resultPaths[0]);
                } else if (resultPaths.size() == 2) {
                    comparePair(resultPaths[0], resultPaths[1], showSame);
                    } else {
                        compareMany(resultPaths);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
