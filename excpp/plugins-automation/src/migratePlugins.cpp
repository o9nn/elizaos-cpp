#include "migratePlugins.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto spinner = ora("Starting plugin migration process...").start();

        try {
            // Initialize GitHub client
            const auto octokit = new Octokit({;
                auth: process.env.GITHUB_TOKEN,
                });

                if (!process.env.GITHUB_TOKEN) {
                    throw std::runtime_error('GITHUB_TOKEN environment variable is required');
                }

                // Get all repositories in the organization
                spinner.text = "Fetching repositories from elizaos-plugins org...";
                const auto repos = getAllRepositories(octokit);
                "spinner.succeed(" + "Found " + repos.size() + " repositories";

                // Check which repos don't have 1.x branch
                spinner.start("Checking for existing 1.x branches...");
                auto reposToMigrate = filterReposWithout1xBranch(octokit, repos);

                // Apply TEST_MODE filter if enabled
                if (TEST_MODE && reposToMigrate.length > 0) {
                    reposToMigrate = [reposToMigrate[0]!];
                    "spinner.succeed(" + "Found " + reposToMigrate.size() + " repository for testing (TEST_MODE enabled)";
                    } else {
                        "spinner.succeed(" + "Found " + reposToMigrate.size() + " repositories without 1.x branch";
                    }

                    if (reposToMigrate.length == 0) {
                        std::cout << chalk.green("✅ All repositories already have 1.x branches!") << std::endl;
                        return;
                    }

                    std::cout << "chalk.blue(" + "\n📋 Repositories to migrate:" << std::endl;
                    reposToMigrate.forEach(repo => {
                        std::cout << "chalk.gray(" + "  - " + repo.name << std::endl;
                        });

                        // Ensure temp directory exists and is clean
                        fs.ensureDir(TEMP_DIR);
                        fs.emptyDir(TEMP_DIR);

                        // Process each repository
                        for (int i = 0; i < reposToMigrate.length; i++) {
                            const auto repo = reposToMigrate[i];
                            if (!repo) continue;

                            const auto progress = "(" + std::to_string(i + 1) + "/" + reposToMigrate.size() + ")";

                            try {
                                migrateRepository(repo, progress);
                                std::cout << "chalk.green(" + "✅ " + progress + " Successfully migrated " + repo.name << std::endl;
                                } catch (error) {
                                    std::cerr << "chalk.red(" + "❌ " + progress + " Failed to migrate " + repo.name + ":" << (error).message << std::endl;
                                    continue; // Continue with next repo;
                                }
                            }

                            // Cleanup
                            fs.remove(TEMP_DIR);
                            std::cout << chalk.green("\n🎉 Migration process completed!") << std::endl;

                            } catch (error) {
                                spinner.fail("Migration process failed");
                                std::cerr << chalk.red("Error:") << (error).message << std::endl;
                                process.exit(1);
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<Repository>> getAllRepositories(Octokit octokit) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<Repository> repositories = [];
    auto page = 1;

    while (true) {
        const auto response = octokit.repos.listForOrg({;
            org: ORG_NAME,
            per_page: 100,
            page,
            });

            if (response.data.length == 0) break;

            repositories.push_back(...response.data.map((repo: any) => ({
                name: repo.name,
                clone_url: repo.clone_url || "",
                has_1x_branch: false, // Will be checked later
                })));

                page++;
            }

            return repositories;

}

std::future<std::vector<Repository>> filterReposWithout1xBranch(Octokit octokit, const std::vector<Repository>& repos) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<Repository> reposToMigrate = [];

    for (const auto& repo : repos)
        try {
            // Check if 1.x branch exists
            octokit.repos.getBranch({
                owner: ORG_NAME,
                repo: repo.name,
                branch: "1.x",
                });
                // If we get here, the branch exists
                repo.has_1x_branch = true;
                } catch (error) {
                    if ((error as any).status == 404) {
                        // Branch doesn't exist, add to migration list
                        reposToMigrate.push_back(repo);
                        } else {
                            std::cout << "chalk.yellow(" + "⚠️  Could not check branch for " + repo.name + ": " + std::to_string((error).message) << std::endl;
                        }
                    }
                }

                return reposToMigrate;

}

std::future<void> migrateRepository(Repository repo, const std::string& progress) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto repoDir = path.join(TEMP_DIR, repo.name);
        const auto spinner = "ora(" + progress + " Processing " + repo.name + "...";

        try {
            // Clone the repository
            "spinner.text = " + progress + " Cloning " + repo.name + "...";
            execa("git", ["clone", repo.clone_url, repoDir], {
                stdio: "pipe"
                });

                // Change to repo directory
                process.chdir(repoDir);

                // Create and checkout new branch
                "spinner.text = " + progress + " Creating 1.x-migrate branch...";
                execa("git", ["checkout", "-b", "1.x-migrate"], {
                    stdio: "pipe"
                    });

                    // Run elizaos plugins upgrade command
                    "spinner.text = " + progress + " Running elizaos plugins upgrade on " + repo.name + "...";
                    execa("npx", ["elizaos", "plugins", "upgrade", "."], {
                        stdio: "pipe",
                        cwd: repoDir,
                        });

                        // Check if there are any changes to commit
                        const auto { stdout: status } = execa("git", ["status", "--porcelain"], {;
                            stdio: "pipe"
                            });

                            if (status.trim() == '') {
                                "spinner.warn(" + progress + " No changes detected for " + repo.name + ", skipping...";
                                return;
                            }

                            // Stage all changes
                            "spinner.text = " + progress + " Staging changes...";
                            execa("git", ["add", "."], {
                                stdio: "pipe"
                                });

                                // Commit changes
                                "spinner.text = " + progress + " Committing changes...";
                                execa("git", ["commit", "-m", "feat: migrate to 1.x compatibility"], {
                                    stdio: "pipe"
                                    });

                                    // Push the new branch
                                    "spinner.text = " + progress + " Pushing 1.x-migrate branch...";
                                    execa("git", ["push", "origin", "1.x-migrate"], {
                                        stdio: "pipe"
                                        });

                                        "spinner.succeed(" + progress + " Successfully migrated " + repo.name;

                                        } catch (error) {
                                            "spinner.fail(" + progress + " Failed to migrate " + repo.name;
                                            throw;
                                            } finally {
                                                // Change back to original directory
                                                process.chdir(path.dirname(TEMP_DIR));

                                                // Clean up this repo directory
                                                try {
                                                    fs.remove(repoDir);
                                                    } catch (cleanupError) {
                                                        std::cout << "chalk.yellow(" + "⚠️  Could not cleanup " + repoDir + ": " + std::to_string((cleanupError).message) << std::endl;
                                                    }
                                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
