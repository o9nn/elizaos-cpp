#include "batch-instances.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

BatchInstance simpleToFullBatchInstance(SimpleBatchInstance simple, DeploymentConfig deployment) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create problem statement
    auto problemStatement: ProblemStatementConfig;

    if (simple.extraFields.issueImages && Array.isArray(simple.extraFields.issueImages)) {
        problemStatement = new SWEBenchMultimodalProblemStatement({
            text: simple.problemStatement,
            issueImages: simple.extraFields.issueImages[],
            id: simple.instanceId,
            extraFields: simple.extraFields,
            });
            } else {
                problemStatement = new TextProblemStatement({
                    text: simple.problemStatement,
                    id: simple.instanceId,
                    extraFields: (simple.extraFields) || {},
                    });
                }

                // Create environment config
                const EnvironmentConfig env = {;
                    deployment: {
                        ...deployment,
                        image: simple.imageName,
                        },
                        repo: simple.repoName
                        ? {
                            type: 'preexisting',
                            repoName: simple.repoName,
                            baseCommit: simple.baseCommit || 'HEAD',
                            reset: false,
                        }
                        : nullptr,
                        postStartupCommands: [],
                        postStartupCommandTimeout: 500,
                        name: 'main',
                        };

                        return { env, problemStatement }

}

void sliceSpecToSlice(const std::string& sliceSpec) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    start?: number; stop?: number; step?: number
}

std::function<double()> seededRandom(double seed) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto x = seed;
    return [&]() {;
        x = (x * 1103515245 + 12345) % 2147483648;
        return x / 2147483648;
        };

}

std::vector<BatchInstance> filterBatchItems(const std::vector<BatchInstance>& instances, std::any options = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto filtered = [...instances];

    // Shuffle if requested
    if (options.shuffle) {
        // Use a deterministic seed if not provided
        const auto seed = options.shuffleSeed || 42;
        const auto random = seededRandom(seed);
        filtered.sort(() => random() - 0.5);
    }

    // Apply filter
    if (options.filter) {
        const auto regex = new RegExp(options.filter);
        filtered = filtered.filter((instance) => {
            const auto id = (instance.problemStatement as { id?: string }).id || '';
            return regex.test(id);
            });
        }

        // Apply slice
        if (options.slice) {
            const auto { start, stop, step } = sliceSpecToSlice(options.slice);
            const auto startIdx = start || 0;
            const auto stopIdx = stop || filtered.length;
            const auto stepSize = step || 1;

            const std::vector<BatchInstance> sliced = [];
            for (int i = startIdx; i < stopIdx && i < filtered.length; i += stepSize) {
                sliced.push(filtered[i]);
            }
            filtered = sliced;
        }

        return filtered;

}

SimpleBatchInstance fromSWEBench(const std::unordered_map<std::string, unknown>& sweBenchInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto instanceId = sweBenchInstance.instance_id;
    const auto problemStatement = sweBenchInstance.problem_statement;
    const auto baseCommit = sweBenchInstance.base_commit;
    auto imageName = sweBenchInstance.image_name | std::nullopt;

    // Generate image name if not provided
    if (!imageName) {
        const auto parts = instanceId.split('__');
        if (parts.length == 2) {
            const auto [org, proj] = parts;
            // Only replace hyphens in the org part, keep proj as is for the tag
            const auto imageTag = std::to_string(org.replace(/-/g, '_')) + "_1776_" + std::to_string(proj);
            "swebench/sweb.eval.x86_64." + std::to_string(imageTag) + ":latest"
            } else {
                // Fallback for instances without proper org__proj format
                const auto safeId = instanceId.replace(/[^a-zA-Z0-9_-]/g, '_');
                "swebench/sweb.eval.x86_64." + std::to_string(safeId) + ":latest"
            }
        }

        const SimpleBatchInstance result = {;
            instanceId,
            problemStatement,
            baseCommit,
            imageName,
            repoName: 'testbed',
            extraFields: {},
            };

            // Handle multimodal instances
            if (sweBenchInstance.image_assets) {
                auto imageAssets = sweBenchInstance.image_assets;
                if (typeof imageAssets == 'string') {
                    imageAssets = JSON.parse(imageAssets);
                }
                if ((imageAssets as any).problem_statement) {
                    result.extraFields = { ...result.extraFields, issue_images: (imageAssets).problem_statement };
                }
            }

            return result;

}

AbstractInstanceSource createInstanceSource(BatchInstanceSourceConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (config.type == 'file' || config.path) {
            if (!config.path) {
                throw std::runtime_error('path is required for file instance source');
            }
            return new InstancesFromFile({;
                path: config.path,
                filter: config.filter,
                slice: config.slice,
                shuffle: config.shuffle,
                deployment: config.deployment,
                });
                } else if (config.type == 'swe_bench') {
                    return new SWEBenchInstances(config);
                    } else {
                        throw std::runtime_error(`Unknown instance source type: ${config.type}`);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
