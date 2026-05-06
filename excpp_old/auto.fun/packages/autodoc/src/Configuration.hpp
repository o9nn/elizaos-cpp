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

// Configuration.ts

/**
 * Gets the repository root path by going up two levels from the current file
 * This assumes the code is in src/ directory of the package
 */

struct ConfigurationData {
    std::string absolute;
    std::string relative;
    std::vector<std::string> excludedDirectories;
    Repository repository;
    std::string commitMessage;
    std::string pullRequestTitle;
    std::string pullRequestDescription;
    std::vector<std::string> pullRequestLabels;
    std::vector<std::string> pullRequestReviewers;
    std::vector<std::string> excludedFiles;
    bool generateJsDoc;
    bool generateReadme;
};

/**
 * Represents a configuration object that holds various settings for a project.
 * Handles both absolute and relative paths for different operations.
 */
class Configuration implements Omit<ConfigurationData, "rootDirectory"> {
  private _rootDirectory!: ConfigurationData["rootDirectory"];
  private readonly repoRoot: string;
  private _branch = "develop";
  private _generateJsDoc = true;
  private _generateReadme = false;

  public excludedDirectories: string[] = [];
  public repository: Repository = {
    owner: "elizaOS",
    name: "eliza",
    pullNumber: undefined,
  };

    // First try to get from environment variables

    // Handle other inputs


} // namespace elizaos
