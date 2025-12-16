#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Message {
    std::string id;
    std::string content;
    bool isFromAgent;
    std::string timestamp;
};

struct AgentChatProps {
    RebelRanking agent;
};

// Helper function to generate agent responses based on agent name
std::string getAgentResponse(const std::string& agentName, const std::string& userMessage);

} // namespace elizaos
