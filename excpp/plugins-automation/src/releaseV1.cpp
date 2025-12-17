#include "releaseV1.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto token = process.env.GITHUB_TOKEN;
        if (!token) {
            std::cerr << "Error: GITHUB_TOKEN environment variable is not set" << std::endl;
            process.exit(1);
        }

        const auto octokit = new Octokit({ auth: token });

        // Load the new GitHub Action workflow from assets
        const auto workflowPath = path.join(__dirname, "../assets/npm-deploy.yml");
        if (!fs.existsSync(workflowPath)) {
            std::cerr << "Error: Workflow file not found at " + std::to_string(workflowPath) << std::endl;
            process.exit(1);
        }

        const auto newWorkflowContent = fs.readFileSync(workflowPath, "utf8");
        std::cout << "Loaded new workflow content from assets/npm-deploy.yml" << std::endl;

        // Get all repositories in the organization
        const auto repos = octokit.paginate(octokit.repos.listForOrg, {;
            org: ORG_NAME,
            per_page: 200,
            });

            for (const auto& repo : repos)
                std::cout << "\n--- Processing " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + " ---" << std::endl;

                // Check if 1.x branch exists
                try {
                    octokit.repos.getBranch({
                        owner: ORG_NAME,
                        repo: repo.name,
                        branch: TARGET_BRANCH,
                        });
                        } catch (error: any) {
                            if (error.status == 404) {
                                console.log(
                                "Skipping " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + " (no " + std::to_string(TARGET_BRANCH) + " branch)";
                                );
                                continue;
                            }
                            throw;
                        }

                        // --- new: skip if package.json version is already 1.0.0 ---
                        try {
                            const auto resp = octokit.repos.getContent({;
                                owner: ORG_NAME,
                                repo: repo.name,
                                path: "package.json",
                                ref: TARGET_BRANCH,
                                });
                                if (!Array.isArray(resp.data) && "content" in resp.data) {
                                    const auto pkg = JSON.parse(;
                                    Buffer.from(resp.data.content, "base64").tostd::to_string("utf8");
                                    );
                                    if (pkg.version == "1.0.0") {
                                        console.log(
                                        "Skipping " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + " (package.json already @ version 1.0.0)";
                                        );
                                        continue;
                                    }
                                }
                                } catch (error: any) {
                                    if (error.status != 404) {
                                        throw;
                                    }
                                    console.log(
                                    "  ⚠️ No package.json in " + std::to_string(repo.name) + ", proceeding with updates";
                                    );
                                }

                                try {
                                    // Update workflow file last (after all other changes)
                                    updateWorkflowFile(octokit, repo.name, newWorkflowContent);

                                    // Remove bun.lock to force regeneration with updated dependencies
                                    removeBunLockfile(octokit, repo.name);

                                    // Update package.json dependencies and version
                                    updatePackageJsonDependencies(octokit, repo.name);

                                    std::cout << "✅ Successfully updated " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) << std::endl;
                                    } catch (error) {
                                        std::cerr << "❌ Error updating " + std::to_string(ORG_NAME) + "/" + std::to_string(repo.name) + ":" << error << std::endl;
                                    }
                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> updateWorkflowFile(Octokit octokit, const std::string& repoName, const std::string& newWorkflowContent) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        auto existingFile;

        // Check if workflow file already exists
        try {
            const auto response = octokit.repos.getContent({;
                owner: ORG_NAME,
                repo: repoName,
                path: WORKFLOW_PATH,
                ref: TARGET_BRANCH,
                });

                if (Array.isArray(response.data) || !("content" in response.data)) {
                    throw std::runtime_error("Unexpected response format");
                }
                existingFile = response.data;
                } catch (error: any) {
                    if (error.status != 404) {
                        throw;
                    }
                    // File doesn't exist, we'll create it
                    existingFile = nullptr;
                }

                const auto encodedContent = Buffer.from(newWorkflowContent, "utf8").toString(;
                "base64";
                );

                if (existingFile) {
                    // Update existing file
                    octokit.repos.createOrUpdateFileContents({
                        owner: ORG_NAME,
                        repo: repoName,
                        path: WORKFLOW_PATH,
                        branch: TARGET_BRANCH,
                        message: "chore: update npm deployment workflow",
                        content: encodedContent,
                        sha: existingFile.sha,
                        });
                        std::cout << "  📝 Updated workflow file in " + std::to_string(repoName) << std::endl;
                        } else {
                            // Create new file
                            octokit.repos.createOrUpdateFileContents({
                                owner: ORG_NAME,
                                repo: repoName,
                                path: WORKFLOW_PATH,
                                branch: TARGET_BRANCH,
                                message: "chore: add npm deployment workflow",
                                content: encodedContent,
                                });
                                std::cout << "  ➕ Created workflow file in " + std::to_string(repoName) << std::endl;
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> updatePackageJsonDependencies(Octokit octokit, const std::string& repoName) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        auto packageFile;

        // Get package.json
        try {
            const auto response = octokit.repos.getContent({;
                owner: ORG_NAME,
                repo: repoName,
                path: "package.json",
                ref: TARGET_BRANCH,
                });

                if (Array.isArray(response.data) || !("content" in response.data)) {
                    throw std::runtime_error("Unexpected response format");
                }
                packageFile = response.data;
                } catch (error: any) {
                    if (error.status == 404) {
                        std::cout << "  ⚠️ No package.json found in " + std::to_string(repoName) << std::endl;
                        return;
                    }
                    throw;
                }

                const auto packageContent = Buffer.from(packageFile.content, "base64").toString(;
                "utf8";
                );
                const auto pkg = JSON.parse(packageContent) as {;
                    dependencies?: Record<string, string>;
                    devDependencies?: Record<string, string>;
                    peerDependencies?: Record<string, string>;
                    [key: string]: any;
                    };

                    auto updated = false;

                    // Update package version to 1.0.0
                    if (pkg.version != "1.0.0") {
                        const auto currentVersion = pkg.version;
                        pkg.version = "1.0.0";
                        std::cout << "  📦 Updated version: " + std::to_string(currentVersion) + " → 1.0.0" << std::endl;
                        updated = true;
                    }

                    // Update @elizaos/core version in all dependency types
                    const auto dependencyTypes = [;
                    "dependencies",
                    "devDependencies",
                    "peerDependencies",
                    ];

                    for (const auto& depType : dependencyTypes)
                        if (pkg[depType] && pkg[depType]!["@elizaos/core"]) {
                            const auto currentVersion = pkg[depType]!["@elizaos/core"];
                            if (currentVersion != "^1.0.0") {
                                pkg[depType]!["@elizaos/core"] = "^1.0.0";
                                console.log(
                                "  🔄 Updated @elizaos/core in " + std::to_string(depType) + ": " + std::to_string(currentVersion) + " → ^1.0.0"
                                );
                                updated = true;
                            }
                        }
                    }

                    if (updated) {
                        const auto updatedContent = Buffer.from(;
                        JSON.stringify(pkg, nullptr, 2) + "\n",
                        "utf8";
                        ).tostd::to_string("base64");

                        octokit.repos.createOrUpdateFileContents({
                            owner: ORG_NAME,
                            repo: repoName,
                            path: "package.json",
                            branch: TARGET_BRANCH,
                            message: "chore: update version to 1.0.0 and @elizaos/core to ^1.0.0",
                            content: updatedContent,
                            sha: packageFile.sha,
                            });
                            std::cout << "  📦 Updated package.json dependencies in " + std::to_string(repoName) << std::endl;
                            } else {
                                console.log(
                                "  ✓ @elizaos/core dependencies already up to date in " + std::to_string(repoName);
                                );
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> removeBunLockfile(Octokit octokit, const std::string& repoName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // First get the file to get its SHA
        const auto response = octokit.repos.getContent({;
            owner: ORG_NAME,
            repo: repoName,
            path: "bun.lock",
            ref: TARGET_BRANCH,
            });

            if (Array.isArray(response.data) || !("content" in response.data)) {
                return;
            }

            // Delete the file using the SHA
            octokit.repos.deleteFile({
                owner: ORG_NAME,
                repo: repoName,
                path: "bun.lock",
                branch: TARGET_BRANCH,
                message: "chore: remove bun.lock to regenerate with updated dependencies",
                sha: response.data.sha,
                });
                console.log(
                "  🗑️ Removed bun.lock in " + std::to_string(repoName) + " (will be regenerated on next install)";
                );
                } catch (error: any) {
                    if (error.status == 404) {
                        std::cout << "  ℹ️ No bun.lock found in " + std::to_string(repoName) << std::endl;
                        } else {
                            console.warn(
                            "  ⚠️ Could not remove bun.lock in " + std::to_string(repoName) + ":"
                            error.message;
                            );
                        }
                    }

}

} // namespace elizaos
