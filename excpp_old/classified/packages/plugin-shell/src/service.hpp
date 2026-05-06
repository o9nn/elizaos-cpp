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
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// The terminal service holds an active terminal session which the agent can use, updating the terminal history

 // Added for path normalization

struct ShellHistoryEntry {
    std::string command;
    std::string output;
    std::optional<std::string> error;
    std::optional<double> exitCode;
    double timestamp;
    std::string cwd;
};

// Interface for file operations
struct FileOperationEntry {
    double timestamp;
    std::string operationType;
    std::string command;
    std::string target;
    std::optional<std::string> secondaryTarget;
    std::string cwd;
};

// Heuristic to identify file operation types and targets
// This is a simplified parser and might need to be more robust for complex commands

class ShellService {
public:
    ShellService(IAgentRuntime runtime);
    std::future<ShellService> start(IAgentRuntime runtime);
    Promise< executeCommand(const std::string& command);
    void catch(const std::any& e);
    void catch(const std::any& e);
    void if(auto this.history.length > this.maxHistoryLength);
    std::vector<ShellHistoryEntry> getHistory(auto count = 10);
    std::vector<FileOperationEntry> getFileOperationHistory(auto count = 10);
    std::string getCurrentWorkingDirectory();
    void clearHistory();
    std::future<void> stop();
};


} // namespace elizaos
