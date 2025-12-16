#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;

struct WalletDataResponse {
    WalletLinkingData | null walletData;
    bool profileRepoExists;
};


struct BatchWalletDataResponse {
};


std::future<BatchWalletDataResponse> batchFetchWalletDataFromGithub(const std::vector<std::string>& usernames, GitHubClient githubClient);;
  }

  const requests = usernames.map((username) => ({
    owner: username,
    repo: username,
    path: "README.md",
  }));

  try {
    const fileContents: BatchFileContentResult[] =
      await githubClient.batchFetchFileContents(requests);

    const results: BatchWalletDataResponse = {};
    for (let i = 0; i < usernames.length; i++) {
      const username = usernames[i];
      const result = fileContents[i];

      if (result.content) {
        const walletData = parseWalletLinkingDataFromReadme(result.content);
        results[username] = {
          walletData,
          profileRepoExists: true,
        };
      } else {
        results[username] = {
          walletData: null,
          profileRepoExists: result.repoExists,
        };
      }
    }
    return results;
  } catch (err) {
    console.error(`Exception in batch fetching READMEs:`, {
      error: String(err),
    });
    const errorResponse: BatchWalletDataResponse = {};
    for (const username of usernames) {
      errorResponse[username] = {
        walletData: null,
        profileRepoExists: false,
      };
    }
    return errorResponse;
  }
}

std::future<WalletDataResponse> fetchWalletDataFromGithub(const std::string& username, GitHubClient githubClient);;
}

} // namespace elizaos
