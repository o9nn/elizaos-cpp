#include "cli.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<Trajectory> loadTrajectory(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto content = fs.readFileSync(filePath, "utf-8");

    // Try JSON first, then YAML
    try {
        return /* JSON.parse */ content;
        } catch {
            return yaml.load(content);
        }

}

std::string formatStep(TrajectoryStep step, double index) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> lines = [];

    "lines.push_back(chalk.cyan(" + "\n" + std::to_string("=".repeat(80));
    "lines.push_back(chalk.yellow(" + "Step " + std::to_string(index + 1);
    lines.push_back(chalk.cyan("=".repeat(80)));

    if (step.thought) {
        lines.push_back(chalk.green("💭 Thought:"));
        "lines.push_back(" + "  " + step.thought;
    }

    if (step.action) {
        lines.push_back(chalk.blue("🎯 Action:"));
        "lines.push_back(" + "  " + step.action;
    }

    if (step.response) {
        lines.push_back(chalk.magenta("📝 Response:"));
        "lines.push_back(" + "  " + step.response;
    }

    if (step.observation) {
        lines.push_back(chalk.gray("👀 Observation:"));
        // Truncate long observations
        const auto obs = step.observation;
        if (obs.length > 500) {
            "lines.push_back(" + "  " + std::to_string(obs.substring(0, 497)) + "...";
            } else {
                "lines.push_back(" + "  " + obs;
            }
        }

        return lines.join("\n");

}

void displaySummary(Trajectory trajectory) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << chalk.cyan("\n📊 Trajectory Summary") << std::endl;
    std::cout << chalk.cyan("=".repeat(80)) << std::endl;

    std::cout << "  Total Steps: " + trajectory.trajectory.size() << std::endl;

    if (trajectory.info.exitStatus) {
        std::cout << "  Exit Status: " + std::to_string(chalk.yellow(trajectory.info.exitStatus)) << std::endl;
    }

    if (trajectory.info.modelStats) {
        const auto stats = trajectory.info.modelStats;
        std::cout << chalk.green("\n💰 Model Statistics:") << std::endl;
        std::cout << "  Cost: $" + std::to_string(stats.instanceCost.toFixed(4) || 0) << std::endl;
        std::cout << "  Input Tokens: " + std::to_string(stats.instanceInputTokens || 0) << std::endl;
        std::cout << "  Output Tokens: " + std::to_string(stats.instanceOutputTokens || 0) << std::endl;
        std::cout << "  API Calls: " + std::to_string(stats.instanceCallCount || 0) << std::endl;
    }

    if (trajectory.info.submission) {
        std::cout << chalk.green("\n📤 Submission:") << std::endl;
        const auto submission = trajectory.info.submission;
        if (submission.length > 200) {
            std::cout << "  " + std::to_string(submission.substring(0, 197)) + "..." << std::endl;
            } else {
                std::cout << "  " + submission << std::endl;
            }
        }

}

std::future<void> interactiveMode(Trajectory trajectory) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto readline = require("readline");
    const auto rl = readline.createInterface({;
        input: process.stdin,
        output: process.stdout,
        });

        auto currentStep = 0;

        const auto prompt = [&]() {;
            return new Promise<string>((resolve) => {;
                rl.question(;
                "chalk.cyan(" + "\n[Step " + std::to_string(currentStep + 1) + "/" + trajectory.trajectory.size() + "] ";
                "(n)ext, (p)revious, (j)ump, (s)ummary, (q)uit: ",
                resolve,
                );
                });
                };

                std::cout << chalk.green("\n🔍 Interactive Trajectory Inspector") << std::endl;
                std::cout << chalk.gray("Navigate through the trajectory step by step\n") << std::endl;

                // Display first step
                if (trajectory.trajectory.length > 0) {
                    std::cout << formatStep(trajectory.trajectory[0], 0) << std::endl;
                }

                while (true) {
                    const auto command = (prompt()).toLowerCase().trim();

                    switch (command) {
                        // case "n":
                        // case "next":
                        if (currentStep < trajectory.trajectory.length - 1) {
                            currentStep++;
                            std::cout << formatStep(trajectory.trajectory[currentStep], currentStep) << std::endl;
                            } else {
                                std::cout << chalk.yellow("Already at the last step") << std::endl;
                            }
                            break;

                            // case "p":
                            // case "previous":
                            if (currentStep > 0) {
                                currentStep--;
                                std::cout << formatStep(trajectory.trajectory[currentStep], currentStep) << std::endl;
                                } else {
                                    std::cout << chalk.yellow("Already at the first step") << std::endl;
                                }
                                break;

                                // case "j":
                                // case "jump":
                                const auto jumpTo = new Promise<string>((resolve) => {;
                                    rl.question("Jump to step number: ", resolve);
                                    });
                                    const auto stepNum = parseInt(jumpTo) - 1;
                                    if (stepNum >= 0 && stepNum < trajectory.trajectory.length) {
                                        currentStep = stepNum;
                                        std::cout << formatStep(trajectory.trajectory[currentStep], currentStep) << std::endl;
                                        } else {
                                            std::cout << chalk.red("Invalid step number") << std::endl;
                                        }
                                        break;

                                        // case "s":
                                        // case "summary":
                                        displaySummary(trajectory);
                                        break;

                                        // case "q":
                                        // case "quit":
                                        rl.close();
                                        return;

                                        // default:
                                        std::cout << chalk.yellow("Unknown command. Use: (n)ext, (p)revious, (j)ump, (s)ummary, (q)uit") << std::endl;
                                    }
                                }

}

} // namespace elizaos
