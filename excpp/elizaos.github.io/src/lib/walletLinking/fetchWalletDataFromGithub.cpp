#include "fetchWalletDataFromGithub.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<BatchWalletDataResponse> batchFetchWalletDataFromGithub(const std::vector<std::string>& usernames, GitHubClient githubClient) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (usernames.length == 0) {
        return {}
    }

    const auto requests = usernames.map((username) => ({;
        owner: username,
        repo: username,
        path: "README.md",
        }));

        try {
            const auto fileContents: BatchFileContentResult[] =;
            githubClient.batchFetchFileContents(requests);

            const BatchWalletDataResponse results = {};
            for (int i = 0; i < usernames.length; i++) {
                const auto username = usernames[i];
                const auto result = fileContents[i];

                if (result.content) {
                    const auto walletData = parseWalletLinkingDataFromReadme(result.content);
                    results[username] = {
                        walletData,
                        profileRepoExists: true,
                        };
                        } else {
                            results[username] = {
                                walletData: nullptr,
                                profileRepoExists: result.repoExists,
                                };
                            }
                        }
                        return results;
                        } catch (err) {
                            console.error(`Exception in batch fetching READMEs:`, {
                                error: std::to_string(err),
                                });
                                const BatchWalletDataResponse errorResponse = {};
                                for (const auto& username : usernames)
                                    errorResponse[username] = {
                                        walletData: nullptr,
                                        profileRepoExists: false,
                                        };
                                    }
                                    return errorResponse;
                                }

}

std::future<WalletDataResponse> fetchWalletDataFromGithub(const std::string& username, GitHubClient githubClient) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto results = batchFetchWalletDataFromGithub(;
    [username],
    githubClient,
    );
    return results[username] || { walletData: nullptr, profileRepoExists: false };

}

} // namespace elizaos
