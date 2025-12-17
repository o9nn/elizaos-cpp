#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Constants

// Enums
enum ActionType {
}

// Types
using AgentLog = {

using ActionCardProps = {

using AgentActionViewerProps = {

// Helper functions
std::string getModelUsageType(const std::string& modelType);

void formatDate(double timestamp);

void getModelIcon(auto modelType = '');

void formatTokenUsage(const std::any& usage);

void truncateText(const std::string& text, auto maxLength = 100);

void copyToClipboard(const std::string& text);

void groupActionsByDate(const std::vector<AgentLog>& actions);

// Components
void ActionCard(auto { action, ActionCardProps onDelete });

void AgentActionViewer(auto { agentId, AgentActionViewerProps roomId });

} // namespace elizaos
