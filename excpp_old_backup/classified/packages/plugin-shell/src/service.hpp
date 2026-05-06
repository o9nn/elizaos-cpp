#include "elizaos/core.hpp"
#include "types.hpp"
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

// The terminal service holds an active terminal session which the agent can use, updating the terminal history

 // Added for path normalization

struct ShellHistoryEntry {
    std::string command;
    std::string output;
    std::optional<std::string> error;
    number | null exitCode;
    double timestamp;
    std::string cwd;
};

// Interface for file operations
struct FileOperationEntry {
    double timestamp;
    string; // e.g., "read", "write", "delete", "create_dir", "edit", "move", "copy" operationType;
    string; // The full command command;
    string; // The primary file or directory path involved target;
    std::optional<string; // For commands like mv, cp> secondaryTarget;
    std::string cwd;
};

// Heuristic to identify file operation types and targets
// This is a simplified parser and might need to be more robust for complex commands

class ShellService extends Service {
  static override serviceType: ServiceTypeName = ShellServiceType.SHELL;
  override capabilityDescription =
    'Provides shell access to execute commands on the host system.';

  private history: ShellHistoryEntry[] = [];
  private fileOperationHistory: FileOperationEntry[] = []; // Added
  private maxHistoryLength = 100; // Store the last 100 commands
  private maxFileOperationHistoryLength = 10; // Store last 10 file operations

  private currentWorkingDirectory: string = process.cwd();

  constructor(runtime: IAgentRuntime) {
    super(runtime);
    logger.info('[ShellService] Initialized');
  }

    // No specific async startup actions needed for now

      // Handle 'cd' command separately to update CWD
          // Attempt to change directory
          // process.chdir will throw if path is invalid
          // To make it robust, we test if the path is valid by trying to execute a simple command in it.

    // Log file operation
      // Only log successful operations for simplicity

    // Emit event for progression tracking

  // New method to get file operation history

    // No specific cleanup needed for execSync

} // namespace elizaos
