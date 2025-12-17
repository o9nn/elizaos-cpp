#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".utils/log.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Problem statement classes for defining agent tasks
 * Converted from sweagent/agent/problem_statement.py
 */

// Constants for image processing

/**
 * Interface for problem statements
 * Any class that implements this interface can be used as a problem statement
 */
struct ProblemStatement {
    std::string id;
};

/**
 * Base class for built-in problem statements
 */

    ExtraFields getExtraFields();

/**
 * Empty problem statement
 */
class EmptyProblemStatement {
public:
    EmptyProblemStatement();
    std::string getProblemStatement();

private:
    std::string id_;
};

/**
 * Text-based problem statement
 */
class TextProblemStatement {
public:
    TextProblemStatement(std::optional<std::any> config);
    std::string getProblemStatement();
    ExtraFields getExtraFields();
    std::string toString();

private:
    std::string id_;
    std::string text_;
    ExtraFields extraFields_;
};

/**
 * File-based problem statement
 */
class FileProblemStatement {
public:
    FileProblemStatement(std::optional<std::any> config);
    std::string getProblemStatement();
    ExtraFields getExtraFields();

private:
    std::string id_;
    std::string filepath_;
    ExtraFields extraFields_;
};

/**
 * GitHub issue problem statement
 */
class GithubIssue {
public:
    GithubIssue(std::optional<std::any> config);
     parseGithubUrl(const std::string& url);
    void if(auto !match);
    std::future<std::string> getProblemStatementAsync();
    std::string getProblemStatement();
    ExtraFields getExtraFields();

private:
    std::string id_;
    std::string githubUrl_;
    ExtraFields extraFields_;
};

/**
 * SWE-Bench multimodal problem statement with image support
 */
class SWEBenchMultimodalProblemStatement {
public:
    SWEBenchMultimodalProblemStatement(std::optional<{
    text: string;
    issueImages: string[];
    disableImageProcessing: boolean;
    extraFields: Record<string> config, std::optional<string;
  }> any>;
    std::string getProblemStatementForEnv();
    std::string getProblemStatement();
    std::future<std::string> getProblemStatementAsync();
    std::optional<std::string> downloadAndConvertImageSync(const std::string& url);
    ExtraFields getExtraFields();
    std::string toString();

/**
 * Factory function to create problem statement from simplified input
 */
ProblemStatement problemStatementFromSimplifiedInput(const std::string& input, const std::variant<'text', 'text_file', 'github_issue', 'swe_bench_multimodal'>& type);

/**
 * Type for problem statement configurations
 */
using ProblemStatementConfig = std::variant<, TextProblemStatement, SWEBenchMultimodalProblemStatement, GithubIssue, EmptyProblemStatement, FileProblemStatement>;

} // namespace elizaos
