#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include ".utils/errors.hpp"
#include ".utils/transactions.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Utility function to sanitize errors by converting BigInt values to strings
std::any sanitizeError(const std::any& error);

// Helper function to detect platform from URL
std::string detectPlatform(const std::string& url);

class ClankerService {
public:
    ClankerService(IAgentRuntime runtime);
    Promise< getOrCreateWallet(const std::string& accountName);
    std::future<void> initialize(IAgentRuntime runtime);
    std::future<ClankerService> start(IAgentRuntime runtime);
    std::future<DeployResult> deployToken(TokenDeployParams params, const std::string& accountName);
    std::future<void> stop();

private:
    CdpTransactionManager transactionManager_;
};


} // namespace elizaos
