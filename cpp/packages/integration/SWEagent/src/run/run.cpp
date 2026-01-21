#include "run.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> run(const std::vector<std::string>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto program = new Command();

    program.name("swe-agent").description("SWE-agent - AI Software Engineering Agent").version("1.0.0");

    // Single run command
    program;
    .command("single");
    .description("Run agent on a single instance");
    .option("-c, --config <path>", "Configuration file path");
    .option("-e, --env <json>", "Environment configuration (JSON)");
    .option("-a, --agent <json>", "Agent configuration (JSON)");
    .option("-p, --problem <text>", "Problem statement text");
    .option("-o, --output <dir>", "Output directory", "DEFAULT");
    .action(async (options) => {
        std::cout << "Running single instance" << std::endl;

        // Load config
        std::optional<RunSingleConfig> config = {};
        if (options.config) {
            config = loadFile(options.config);
        }

        // Override with command line options
        if (options.env) {
            config.env = /* JSON.parse */ options.env;
        }
        if (options.agent) {
            config.agent = /* JSON.parse */ options.agent;
        }
        if (options.problem) {
            config.problemStatement = new TextProblemStatement({
                text: options.problem,
                });
            }
            if (options.output) {
                config.outputDir = options.output;
            }

            runSingleFromConfig(config);
            });

            // Batch run command
            program;
            .command("batch");
            .description("Run agent on multiple instances");
            .option("-c, --config <path>", "Configuration file path");
            .option("-i, --instances <path>", "Instances file path");
            .option("-a, --agent <json>", "Agent configuration (JSON)");
            .option("-o, --output <dir>", "Output directory", "DEFAULT");
            .option("-w, --workers <n>", "Number of parallel workers", "1");
            .option("--redo", "Redo existing instances");
            .action(async (options) => {
                std::cout << "Running batch" << std::endl;

                // Load config
                std::optional<RunBatchConfig> config = {};
                if (options.config) {
                    config = loadFile(options.config);
                }

                // Override with command line options
                if (options.instances) {
                    config.instances = {
                        type: "file",
                        path: options.instances,
                        };
                    }
                    if (options.agent) {
                        config.agent = /* JSON.parse */ options.agent;
                    }
                    if (options.output) {
                        config.outputDir = options.output;
                    }
                    if (options.workers) {
                        config.numWorkers = parseInt(options.workers);
                    }
                    if (options.redo) {
                        config.redoExisting = true;
                    }

                    runBatchFromConfig(config);
                    });

                    // Replay command
                    program;
                    .command("replay");
                    .description("Replay an agent trajectory");
                    .argument("<traj-path>", "Path to trajectory file");
                    .option("-o, --output <dir>", "Output directory", "DEFAULT");
                    .option("-d, --deployment <json>", "Deployment configuration (JSON)");
                    .action(async (trajPath, options) => {
                        std::cout << "Replaying trajectory" << std::endl;

                        const std::optional<RunReplayConfig> config = {;
                            trajPath,
                            };

                            if (options.output) {
                                config.outputDir = options.output;
                            }
                            if (options.deployment) {
                                config.deployment = /* JSON.parse */ options.deployment;
                            }

                            runReplayFromConfig(config);
                            });

                            // Parse arguments
                            program.parseAsync(args);

}

} // namespace elizaos
