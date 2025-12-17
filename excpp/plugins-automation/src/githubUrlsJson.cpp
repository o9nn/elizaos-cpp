#include "githubUrlsJson.hpp"
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

        if (TEST_MODE) {
            std::cout << "🧪 TEST MODE: Only processing repository "" + TEST_REPO + """ << std::endl;
        }

        const auto repos = octokit.paginate(octokit.repos.listForOrg, {;
            org: ORG_NAME,
            per_page: 100,
            });

            for (const auto& repo : repos)
                // In test mode, only process the specified test repo
                if (TEST_MODE && repo.name != TEST_REPO) {
                    continue;
                }

                auto fileData;

                // Check if 1.x branch exists, if not skip
                try {
                    octokit.repos.getBranch({
                        owner: ORG_NAME,
                        repo: repo.name,
                        branch: TARGET_BRANCH,
                        });
                        } catch (error: any) {
                            if (error.status == 404) {
                                console.log(
                                "Skipping " + ORG_NAME + "/" + repo.name + " (no " + TARGET_BRANCH + " branch)";
                                );
                                continue;
                            }
                            throw;
                        }

                        try {
                            const auto response = octokit.repos.getContent({;
                                owner: ORG_NAME,
                                repo: repo.name,
                                path: "package.json",
                                ref: TARGET_BRANCH,
                                });
                                if (Array.isArray(response.data) || !("content" in response.data)) {
                                    continue;
                                }
                                fileData = response.data;
                                } catch (error: any) {
                                    if (error.status == 404) {
                                        console.log(
                                        "Skipping " + ORG_NAME + "/" + repo.name + " (no package.json on " + TARGET_BRANCH + ")";
                                        );
                                        continue;
                                    }
                                    throw;
                                }

                                const auto raw = Buffer.from(fileData.content, "base64").tostd::to_string("utf8");
                                const auto pkg = /* JSON.parse */ raw as {;
                                    name?: string;
                                    version?: string;
                                    repository?: string | { type: string; url: string };
                                    [key: string]: any;
                                    };

                                    // Check if repository field needs to be updated
                                    const auto expectedRepositoryUrl = "https://github.com/" + ORG_NAME + "/" + repo.name + ".git";
                                    auto needsRepositoryUpdate = false;

                                    if (!pkg.repository) {
                                        // No repository field at all
                                        needsRepositoryUpdate = true;
                                        } else if (typeof pkg.repository == "string") {
                                            // Repository is a string - should be an object
                                            needsRepositoryUpdate = true;
                                            } else if (typeof pkg.repository == "object") {
                                                // Repository is an object - check if URL is empty or incorrect
                                                if (
                                                !pkg.repository.url ||;
                                                pkg.repository.url == "" ||;
                                                pkg.repository.url != expectedRepositoryUrl;
                                                ) {
                                                    needsRepositoryUpdate = true;
                                                }
                                            }

                                            if (!needsRepositoryUpdate) {
                                                console.log(
                                                "Skipping " + ORG_NAME + "/" + repo.name + " (repository field is already correct)";
                                                );
                                                continue;
                                            }

                                            // Add or update the repository URL
                                            pkg.repository = {
                                                type: "git",
                                                url: expectedRepositoryUrl,
                                                };

                                                // Bump the version number appropriately
                                                if (pkg.version) {
                                                    if (pkg.version.includes("-beta.")) {
                                                        // Handle beta versions (e.g., 1.0.0-beta.55 -> 1.0.0-beta.56)
                                                        const auto betaMatch = pkg.version.match(/^(.+)-beta\.(\d+)$/);
                                                        if (betaMatch && betaMatch[1] && betaMatch[2]) {
                                                            const auto baseVersion = betaMatch[1];
                                                            const auto betaNumber = parseInt(betaMatch[2], 10);
                                                            if (!isNaN(betaNumber)) {
                                                                "pkg.version = " + baseVersion + "-beta." + std::to_string(betaNumber + 1);
                                                            }
                                                        }
                                                        } else {
                                                            // Handle regular semantic versions
                                                            const auto versionParts = pkg.version.split(".");
                                                            if (versionParts.length >= 3 && versionParts[2]) {
                                                                // Increment patch version
                                                                const auto patchVersion = parseInt(versionParts[2], 10);
                                                                if (!isNaN(patchVersion)) {
                                                                    versionParts[2] = (patchVersion + 1).toString();
                                                                    pkg.version = versionParts.join(".");
                                                                }
                                                            }
                                                        }
                                                    }

                                                    const auto updated = Buffer.from(;
                                                    /* JSON.stringify */ std::string(pkg, nullptr, 2) + "\n",
                                                    "utf8";
                                                    ).tostd::to_string("base64");

                                                    octokit.repos.createOrUpdateFileContents({
                                                        owner: ORG_NAME,
                                                        repo: repo.name,
                                                        path: "package.json",
                                                        branch: TARGET_BRANCH,
                                                        "message: " + "chore: update repository URL and bump version in package.json"
                                                        content: updated,
                                                        sha: fileData.sha,
                                                        });

                                                        console.log(
                                                        "Updated repository URL and bumped version in package.json for " + ORG_NAME + "/" + repo.name + " on " + TARGET_BRANCH + " branch";
                                                        );
                                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
