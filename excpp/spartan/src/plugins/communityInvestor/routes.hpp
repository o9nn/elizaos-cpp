#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "service.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Helper to send success response
void sendSuccess(const std::any& res, const std::any& data, auto status);

// Helper to send error response
void sendError(const std::any& res, double status, const std::string& code, const std::string& message, std::optional<std::string> details);

std::future<void> getLeaderboardDataHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

std::future<void> communityInvestorPanelHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);

std::future<void> communityInvestorAssetsHandler(const std::any& req, const std::any& res, IAgentRuntime runtime);


} // namespace elizaos
