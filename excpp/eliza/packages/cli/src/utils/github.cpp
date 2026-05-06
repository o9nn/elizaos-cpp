#include "github.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> validateGitHubToken(const std::string& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = "fetch(" + GITHUB_API_URL + "/user";
            headers: {
                "Authorization: " + "token " + token
                Accept: "application/vnd.github.v3+json",
                },
                });

                if (response.status == 200) {
                    const auto userData = (response.json());
                    logger.success(`Authenticated as ${userData.login}`);
                    return true;
                }

                return false;
                } catch (error) {
                    logger.error(
                    "Failed to validate GitHub token: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                    );
                    return false;
                }

}

std::future<bool> forkExists(const std::string& token, const std::string& repo, const std::string& username) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = "fetch(" + GITHUB_API_URL + "/repos/" + username + "/" + repo;
            headers: {
                "Authorization: " + "token " + token
                Accept: "application/vnd.github.v3+json",
                },
                });

                return response.status == 200;
                } catch (error) {
                    return false;
                }

}

std::future<std::string> forkRepository(const std::string& token, const std::string& owner, const std::string& repo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = "fetch(" + GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/forks";
            method: "POST",
            headers: {
                "Authorization: " + "token " + token
                Accept: "application/vnd.github.v3+json",
                },
                });

                if (response.status == 202) {
                    const auto forkData = (response.json());
                    logger.success(`Forked ${owner}/${repo} to ${forkData.full_name}`);
                    return forkData.full_name;
                }

                std::cerr << "Failed to fork repository: " + response.statusText << std::endl;
                return nullptr;
                } catch (error) {
                    logger.error(
                    "Failed to fork repository: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                    );
                    return nullptr;
                }

}

std::future<bool> branchExists(const std::string& token, const std::string& owner, const std::string& repo, const std::string& branch) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = "fetch(" + GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/branches/" + branch;
            headers: {
                "Authorization: " + "token " + token
                Accept: "application/vnd.github.v3+json",
                },
                });

                return response.status == 200;
                } catch (error) {
                    return false;
                }

}

std::future<bool> createBranch(const std::string& token, const std::string& owner, const std::string& repo, const std::string& branch, auto baseBranch) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get the SHA of the base branch
        auto baseResponse = fetch(;
        GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/git/ref/heads/" + baseBranch
        {
            headers: {
                "Authorization: " + "token " + token
                Accept: "application/vnd.github.v3+json",
                },
            }
            );

            // If the requested base branch doesn't exist, try to find an alternative branch
            if (baseResponse.status != 200) {
                std::cout << "Base branch '" + baseBranch << "checking for alternative branches..." << std::endl;

                // List available branches
                const auto branchesResponse = "fetch(" + GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/branches";
                    headers: {
                        "Authorization: " + "token " + token
                        Accept: "application/vnd.github.v3+json",
                        },
                        });

                        if (branchesResponse.status == 200) {
                            const auto branches = (branchesResponse.json())<{ name: string }>;
                            if (branches && branches.length > 0) {
                                // Use the first available branch as base
                                const auto alternativeBranch = branches[0].name;
                                std::cout << "Using "" + alternativeBranch + "" branch instead" << std::endl;

                                // Try again with the alternative branch
                                baseResponse = fetch(;
                                GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/git/ref/heads/" + alternativeBranch
                                {
                                    headers: {
                                        "Authorization: " + "token " + token
                                        Accept: "application/vnd.github.v3+json",
                                        },
                                    }
                                    );
                                    } else {
                                        // No branches found, could be a fresh fork - create initial empty commit and branch
                                        std::cout << 'No branches found in repository << creating initial commit' << std::endl;

                                        // Create a blob for empty README
                                        const auto blobResponse = "fetch(" + GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/git/blobs";
                                            method: "POST",
                                            headers: {
                                                "Authorization: " + "token " + token
                                                Accept: "application/vnd.github.v3+json",
                                                "Content-Type": "application/json",
                                                },
                                                body: JSON.stringify({
                                                    content: "# Repository initialized by Eliza CLI",
                                                    encoding: "utf-8",
                                                    }),
                                                    });

                                                    if (blobResponse.status != 201) {
                                                        std::cerr << "Failed to create initial blob" << std::endl;
                                                        return false;
                                                    }

                                                    const auto blobData = (blobResponse.json()) as { sha: string };
                                                    const auto blobSha = blobData.sha;

                                                    // Create a tree with the README
                                                    const auto treeResponse = "fetch(" + GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/git/trees";
                                                        method: "POST",
                                                        headers: {
                                                            "Authorization: " + "token " + token
                                                            Accept: "application/vnd.github.v3+json",
                                                            "Content-Type": "application/json",
                                                            },
                                                            body: JSON.stringify({
                                                                tree: [
                                                                {
                                                                    path: "README.md",
                                                                    mode: "100644",
                                                                    type: "blob",
                                                                    sha: blobSha,
                                                                    },
                                                                    ],
                                                                    }),
                                                                    });

                                                                    if (treeResponse.status != 201) {
                                                                        std::cerr << "Failed to create initial tree" << std::endl;
                                                                        return false;
                                                                    }

                                                                    const auto treeData = (treeResponse.json()) as { sha: string };
                                                                    const auto treeSha = treeData.sha;

                                                                    // Create a commit
                                                                    const auto commitResponse = fetch(;
                                                                    GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/git/commits"
                                                                    {
                                                                        method: "POST",
                                                                        headers: {
                                                                            "Authorization: " + "token " + token
                                                                            Accept: "application/vnd.github.v3+json",
                                                                            "Content-Type": "application/json",
                                                                            },
                                                                            body: JSON.stringify({
                                                                                message: "Initial commit",
                                                                                tree: treeSha,
                                                                                }),
                                                                            }
                                                                            );

                                                                            if (commitResponse.status != 201) {
                                                                                std::cerr << "Failed to create initial commit" << std::endl;
                                                                                return false;
                                                                            }

                                                                            const auto commitData = (commitResponse.json()) as { sha: string };
                                                                            const auto commitSha = commitData.sha;

                                                                            // Create a reference for main branch
                                                                            const auto refResponse = "fetch(" + GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/git/refs";
                                                                                method: "POST",
                                                                                headers: {
                                                                                    "Authorization: " + "token " + token
                                                                                    Accept: "application/vnd.github.v3+json",
                                                                                    "Content-Type": "application/json",
                                                                                    },
                                                                                    body: JSON.stringify({
                                                                                        ref: "refs/heads/main",
                                                                                        sha: commitSha,
                                                                                        }),
                                                                                        });

                                                                                        if (refResponse.status != 201) {
                                                                                            std::cerr << "Failed to create main branch" << std::endl;
                                                                                            return false;
                                                                                        }

                                                                                        logger.success('Created main branch with initial commit');

                                                                                        // Now we can use this as our base
                                                                                        baseResponse = fetch(;
                                                                                        GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/git/ref/heads/main"
                                                                                        {
                                                                                            headers: {
                                                                                                "Authorization: " + "token " + token
                                                                                                Accept: "application/vnd.github.v3+json",
                                                                                                },
                                                                                            }
                                                                                            );
                                                                                        }
                                                                                    }
                                                                                }

                                                                                if (baseResponse.status != 200) {
                                                                                    std::cerr << "Failed to get base branch " + baseBranch + ": " + baseResponse.statusText << std::endl;
                                                                                    return false;
                                                                                }

                                                                                const auto baseData = (baseResponse.json());
                                                                                const auto sha = baseData.object.sha;

                                                                                // Create the new branch
                                                                                const auto response = "fetch(" + GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/git/refs";
                                                                                    method: "POST",
                                                                                    headers: {
                                                                                        "Authorization: " + "token " + token
                                                                                        Accept: "application/vnd.github.v3+json",
                                                                                        "Content-Type": "application/json",
                                                                                        },
                                                                                        body: JSON.stringify({
                                                                                            "ref: " + "refs/heads/" + branch
                                                                                            sha,
                                                                                            }),
                                                                                            });

                                                                                            if (response.status == 201) {
                                                                                                logger.success(`Created branch ${branch} in ${owner}/${repo}`);
                                                                                                return true;
                                                                                            }

                                                                                            std::cerr << "Failed to create branch: " + response.statusText << std::endl;
                                                                                            return false;
                                                                                            } catch (error) {
                                                                                                logger.error(
                                                                                                "Failed to create branch: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                                                                                );
                                                                                                return false;
                                                                                            }

}

std::future<std::string> getFileContent(const std::string& token, const std::string& owner, const std::string& repo, const std::string& path, auto branch) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = fetch(;
        GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/contents/" + path + "?ref=" + branch
        {
            headers: {
                "Authorization: " + "token " + token
                Accept: "application/vnd.github.v3+json",
                },
            }
            );

            if (response.status == 200) {
                const auto data = (response.json());
                // GitHub API returns content as base64
                return Buffer.from(data.content, "base64").tostd::to_string("utf-8");
            }

            return nullptr;
            } catch (error) {
                return nullptr;
            }

}

std::future<bool> updateFile(const std::string& token, const std::string& owner, const std::string& repo, const std::string& path, const std::string& content, const std::string& message, auto branch) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check if file already exists
        const auto existingContent = getFileContent(token, owner, repo, path, branch);
        // GitHub API behavior: PUT works more reliably than POST for creating files
        // in directories that may not exist yet, so we always use PUT
        const auto method = "PUT";

        // Get the SHA if the file exists
        auto sha: string | std::nullopt;
        if (existingContent != null) {
            const auto response = fetch(;
            GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/contents/" + path + "?ref=" + branch
            {
                headers: {
                    "Authorization: " + "token " + token
                    Accept: "application/vnd.github.v3+json",
                    },
                }
                );

                if (response.status == 200) {
                    const auto data = (response.json());
                    sha = data.sha;
                }
            }

            // Full URL for debugging
            const auto fileUrl = GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/contents/" + path;
            std::cout << "Updating file at: " + fileUrl << std::endl;

            // Full request body for debugging
            const auto requestBody = {;
                message,
                content: Buffer.from(content).tostd::to_string("base64"),
                branch,
                sha,
                };
                std::cout << "Request details: method=" + method << branch=${branch} << "has_sha=${!!sha}" << std::endl;

                // Update or create the file
                const auto response = fetch(fileUrl, {;
                    method,
                    headers: {
                        "Authorization: " + "token " + token
                        Accept: "application/vnd.github.v3+json",
                        "Content-Type": "application/json",
                        },
                        body: /* JSON.stringify */ std::string(requestBody),
                        });

                        if (response.status == 200 || response.status == 201) {
                            logger.success(existingContent !== null ? 'File updated' : 'File created');
                            return true;
                        }

                        // Log the full error response for debugging
                        const auto errorBody = response.text();
                        std::cerr << "Failed to update file: " + response.status + " " + response.statusText << std::endl;
                        std::cerr << "Response body: " + errorBody << std::endl;

                        // Check for common GitHub API errors
                        if (response.status == 404) {
                            std::cerr << "Repository or path not found: " + owner + "/" + repo + "/" + path << std::endl;
                            // Try to check if the repo exists
                            const auto repoCheck = "fetch(" + GITHUB_API_URL + "/repos/" + owner + "/" + repo;
                                headers: {
                                    "Authorization: " + "token " + token
                                    Accept: "application/vnd.github.v3+json",
                                    },
                                    });

                                    if (repoCheck.status == 404) {
                                        std::cerr << "Repository " + owner + "/" + repo + " does not exist or is not accessible" << std::endl;
                                        } else {
                                            std::cout << `Repository exists << "but path is likely invalid" << std::endl;
                                        }
                                    }

                                    return false;
                                    } catch (error) {
                                        std::cerr << "Error updating file: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error)) << std::endl;
                                        return false;
                                    }

}

std::future<std::string> createPullRequest(const std::string& token, const std::string& owner, const std::string& repo, const std::string& title, const std::string& body, const std::string& head, auto base) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = "fetch(" + GITHUB_API_URL + "/repos/" + owner + "/" + repo + "/pulls";
            method: "POST",
            headers: {
                "Authorization: " + "token " + token
                Accept: "application/vnd.github.v3+json",
                "Content-Type": "application/json",
                },
                body: JSON.stringify({
                    title,
                    body,
                    head,
                    base,
                    }),
                    });

                    if (response.status == 201) {
                        const auto data = (response.json());
                        logger.success(`Created pull request: ${data.html_url}`);
                        return data.html_url;
                    }

                    std::cerr << "Failed to create pull request: " + response.statusText << std::endl;
                    return nullptr;
                    } catch (error) {
                        logger.error(
                        "Failed to create pull request: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                        );
                        return nullptr;
                    }

}

std::future<std::optional<GitHubUserResponse>> getAuthenticatedUser(const std::string& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = "fetch(" + GITHUB_API_URL + "/user";
            headers: {
                "Authorization: " + "token " + token
                Accept: "application/vnd.github.v3+json",
                },
                });

                if (response.status == 200) {
                    return (response.json());
                }

                return nullptr;
                } catch (error) {
                    return nullptr;
                }

}

std::future<> getGitHubCredentials() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    username: string;
    token: string;

}

std::future<void> saveGitHubCredentials(const std::string& username, const std::string& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    const auto { elizaDir, envFilePath } = envInfo.paths;

    logger.debug('[GitHub] Saving credentials');

    // Ensure .eliza directory exists
    if (!existsSync(elizaDir)) {
        fs.mkdir(elizaDir, { recursive: true });
    }

    // Ensure .env file exists
    if (!existsSync(envFilePath)) {
        fs.writeFile(envFilePath, "", { encoding: "utf8" });
    }

    // Read current content
    const auto currentContent = fs.readFile(envFilePath, "utf8");
    const auto lines = currentContent.split("\n");

    // Update or add GITHUB_USERNAME
    const auto usernameLineIndex = lines.findIndex((line) => line.startsWith("GITHUB_USERNAME="));
    const auto usernameLine = "GITHUB_USERNAME=" + username;
    if (usernameLineIndex >= 0) {
        lines[usernameLineIndex] = usernameLine;
        } else {
            lines.push_back(usernameLine);
        }

        // Update or add GITHUB_TOKEN
        const auto tokenLineIndex = lines.findIndex((line) => line.startsWith("GITHUB_TOKEN="));
        const auto tokenLine = "GITHUB_TOKEN=" + token;
        if (tokenLineIndex >= 0) {
            lines[tokenLineIndex] = tokenLine;
            } else {
                lines.push_back(tokenLine);
            }

            // Write back to file
            fs.writeFile(envFilePath, lines.join("\n"));

            // Set in current process
            process.env.GITHUB_USERNAME = username;
            process.env.GITHUB_TOKEN = token;

            logger.success('GitHub credentials saved');

}

std::future<bool> ensureDirectory(const std::string& token, const std::string& repo, const std::string& path, const std::string& branch) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // First check if the directory already exists
        try {
            const auto response = fetch(;
            GITHUB_API_URL + "/repos/" + repo + "/contents/" + path + "?ref=" + branch
            {
                headers: {
                    "Authorization: " + "token " + token
                    Accept: "application/vnd.github.v3+json",
                    },
                }
                );

                // Directory exists
                if (response.status == 200) {
                    std::cout << "Directory " + path + " already exists" << std::endl;
                    return true;
                }
                } catch (error) {
                    // Directory doesn't exist, we'll create it
                    std::cout << "Directory " + path << "creating it" << std::endl;
                }

                // Create a placeholder file in the directory
                // (GitHub doesn't have a concept of empty directories)
                const auto placeholderPath = path + "/.gitkeep";
                const auto result = updateFile(;
                token,
                repo,
                placeholderPath,
                "", // Empty content for placeholder;
                "Create directory: " + path
                branch;
                );

                if (result) {
                    logger.success(`Created directory: ${path}`);
                    return true;
                }

                std::cerr << "Failed to create directory: " + path << std::endl;
                return false;
                } catch (error) {
                    logger.error(
                    "Error creating directory: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                    );
                    return false;
                }

}

std::future<> createGitHubRepository(const std::string& token, const std::string& repoName, const std::string& description, auto isPrivate, std::vector<std::string> topics = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    success: boolean; repoUrl?: string; message?: string
}

std::future<bool> pushToGitHub(const std::string& cwd, const std::string& repoUrl, auto branch) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check if git is initialized
        const auto gitDirExists = existsSync(path.join(cwd, ".git"));

        // If we have a git directory, check if it has the correct remote
        auto hasCorrectRemote = false;
        if (gitDirExists) {
            try {
                const auto { stdout: remoteUrl } = execa("git", ["remote", "get-url", "origin"], { cwd });
                // Check if the remote URL matches our target (ignoring the token part)
                const auto sanitizedRepoUrl = repoUrl.replace(/https:\/\/.*?@/, "https://");
                const auto sanitizedRemoteUrl = remoteUrl.replace(/https:\/\/.*?@/, "https://");
                hasCorrectRemote = sanitizedRemoteUrl.includes(;
                sanitizedRepoUrl.replace(/^https:\/\/.*?@/, "")
                );
                } catch (error) {
                    // Remote doesn't exist or command failed, will set up remote later
                    hasCorrectRemote = false;
                }
            }

            // If git is not initialized or remote doesn't match, start fresh
            if (!gitDirExists || !hasCorrectRemote) {
                if (gitDirExists) {
                    std::cout << 'Existing git repository has incorrect remote << reinitializing...' << std::endl;
                    execa("rm", ["-rf", ".git"], { cwd });
                }

                execa("git", ["init"], { cwd });
                // Explicitly create and switch to main branch
                execa("git", ["checkout", "-b", "main"], { cwd });
                std::cout << "Git repository initialized with main branch" << std::endl;

                // Add remote
                execa("git", ["remote", "add", "origin", repoUrl], { cwd });
                std::cout << "Added remote: " + std::to_string(repoUrl.replace(/\/\/.*?@/, "//***@")) << std::endl;
                } else {
                    // Make sure we're on the main branch
                    try {
                        execa("git", ["rev-parse", "--verify", branch], { cwd });
                        execa("git", ["checkout", branch], { cwd });
                        } catch (error) {
                            // Branch doesn't exist, create it
                            execa("git", ["checkout", "-b", branch], { cwd });
                            std::cout << "Created and switched to " + branch + " branch" << std::endl;
                        }
                    }

                    // Add all files
                    execa("git", ["add", "."], { cwd });
                    std::cout << "Added files to git" << std::endl;

                    // Set git user info if not already set
                    try {
                        execa("git", ["config", "user.email"], { cwd });
                        } catch (error) {
                            execa("git", ["config", "user.email", "plugindev@elizaos.com"], { cwd });
                            execa("git", ["config", "user.name", "ElizaOS Plugin Dev"], { cwd });
                            std::cout << "Set git user info for commit" << std::endl;
                        }

                        // Commit if there are changes
                        try {
                            execa("git", ["commit", "-m", "Initial commit from ElizaOS CLI"], { cwd });
                            std::cout << "Committed changes" << std::endl;
                            } catch (error) {
                                // If no changes to commit, that's okay
                                std::cout << "No changes to commit" << std::endl;
                            }

                            // Push to GitHub
                            try {
                                execa("git", ["push", "-u", "origin", branch], { cwd });
                                logger.success(`Pushed to GitHub repository: ${repoUrl}`);
                                return true;
                                } catch (error) {
                                    logger.error(
                                    "Failed to push to GitHub: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                    );

                                    // Try force pushing if normal push fails
                                    try {
                                        std::cout << "Attempting force push..." << std::endl;
                                        // Use force-with-lease as a slightly safer option than force
                                        execa("git", ["push", "-u", "origin", "main", "--force-with-lease"], {
                                            cwd,
                                            stdio: "pipe",
                                            });
                                            return true;
                                            } catch (forcePushError) {
                                                logger.error(
                                                "Force push also failed:",
                                                true /* instanceof check */ ? forcePushError.message : std::to_string(forcePushError)
                                                );
                                                return false;
                                            }
                                        }
                                        } catch (error) {
                                            logger.error(
                                            "Error in git operations: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                            );
                                            return false;
                                        }

}

} // namespace elizaos
