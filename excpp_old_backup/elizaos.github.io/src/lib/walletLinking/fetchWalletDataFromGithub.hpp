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



struct WalletDataResponse {
    WalletLinkingData | null walletData;
    bool profileRepoExists;
};

struct BatchWalletDataResponse {
};

std::future<BatchWalletDataResponse> batchFetchWalletDataFromGithub(const std::vector<std::string>& usernames, GitHubClient githubClient);

std::future<WalletDataResponse> fetchWalletDataFromGithub(const std::string& username, GitHubClient githubClient);

} // namespace elizaos
