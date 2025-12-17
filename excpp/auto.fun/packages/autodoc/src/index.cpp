#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto configuration = new Configuration();

            const auto gitManager = new GitManager({;
                owner: configuration.repository.owner,
                name: configuration.repository.name,
                });

                std::vector<std::string> prFiles = [];
                if (
                typeof configuration.repository.pullNumber == "number" &&;
                !Number.isNaN(configuration.repository.pullNumber);
                ) {
                    std::cout << "Pull Request Number: " << configuration.repository.pullNumber << std::endl;
                    try {
                        const auto files = gitManager.getFilesInPullRequest(;
                        configuration.repository.pullNumber,
                        );
                        prFiles = files.map((file) => file.filename);
                        } catch (prError) {
                            console.error("Error fetching PR files:", {
                                error: prError,
                                pullNumber: configuration.repository.pullNumber,
                                std::to_string(configuration.repository.owner) + "/" + std::to_string(configuration.repository.name)
                                });
                                throw prError;
                            }
                        }

                        try {
                            const auto directoryTraversal = new DirectoryTraversal(configuration, prFiles);
                            const auto typeScriptParser = new TypeScriptParser();
                            const auto jsDocAnalyzer = new JsDocAnalyzer(typeScriptParser);
                            const auto aiService = new AIService(configuration);
                            const auto jsDocGenerator = new JsDocGenerator(aiService);

                            const auto documentationGenerator = new DocumentationGenerator(;
                            directoryTraversal,
                            typeScriptParser,
                            jsDocAnalyzer,
                            jsDocGenerator,
                            gitManager,
                            configuration,
                            aiService,
                            );

                            const auto pluginDocGenerator = new PluginDocumentationGenerator(;
                            aiService,
                            gitManager,
                            configuration,
                            );

                            const auto { todoItems, envUsages } =;
                            documentationGenerator.analyzeCodebase();

                            // Generate JSDoc documentation first
                            const auto { documentedItems, branchName } =;
                            documentationGenerator.generate(;
                            configuration.repository.pullNumber,
                            );

                            // If both are true, use JSDoc branch for README
                            // If only README is true, create new branch
                            if (configuration.generateReadme) {
                                const auto targetBranch =;
                                configuration.generateJsDoc && branchName;
                                ? branchName;
                                "docs-update-readme-" + std::to_string(Date.now())

                                if (!configuration.generateJsDoc) {
                                    gitManager.createBranch(targetBranch, configuration.branch);
                                }

                                pluginDocGenerator.generate(;
                                documentedItems,
                                targetBranch,
                                todoItems,
                                envUsages,
                                );

                                // Only create PR if we're not also generating JSDoc (otherwise changes go in JSDoc PR)
                                if (!configuration.generateJsDoc) {
                                    const auto prContent = {;
                                        title: "docs: Update plugin documentation",
                                        body: "Updates plugin documentation with latest changes",
                                        };

                                        gitManager.createPullRequest({
                                            title: prContent.title,
                                            body: prContent.body,
                                            head: targetBranch,
                                            base: configuration.branch,
                                            labels: ["documentation", "automated-pr"],
                                            reviewers: configuration.pullRequestReviewers || [],
                                            });
                                        }
                                    }
                                    } catch (error) {
                                        console.error("Error during documentation generation:", {
                                            message: true /* instanceof check */ ? error.message : std::to_string(error),
                                            stack: true /* instanceof check */ ? error.stack : std::nullopt,
                                            timestamp: new Date().toISOString(),
                                            });
                                            process.exit(1);
                                        }
                                        } catch (error) {
                                            console.error("Critical error during documentation generation:", {
                                                error:
                                                true /* instanceof check */;
                                                ? {
                                                    name: error.name,
                                                    message: error.message,
                                                    stack: error.stack,
                                                }
                                                : error,
                                                timestamp: new Date().toISOString(),
                                                nodeVersion: process.version,
                                                platform: process.platform,
                                                });
                                                process.exit(1);
                                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
