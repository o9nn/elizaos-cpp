#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * TypeScript types rules and configuration
 */

/**
 * Rule definition structure matching Cursor IDE rules format
 */
struct Rule {
    std::string name;
    std::optional<std::string> description;
    std::optional<std::vector<std::string>> globs;
    bool alwaysApply;
    std::variant<std::string, RuleContent> content;
};

/**
 * Structured rule content
 */
struct RuleContent {
    std::optional<std::string> title;
    std::optional<std::vector<std::string>> guidelines;
    std::optional<std::string> overview;
    std::optional<ProjectStructure> projectStructure;
};

/**
 * Project structure information
 */
struct ProjectStructure {
    std::vector<EntryPoint> mainEntryPoints;
    ClassInfo mainClass;
    ExecutionEnvironment executionEnvironment;
    ToolsInfo tools;
    std::vector<Inspector> inspectors;
};

struct EntryPoint {
    std::string path;
    std::string description;
    std::string type;
};

struct ClassInfo {
    std::string name;
    std::string path;
    std::string description;
};

struct ExecutionEnvironment {
    std::string type;
    std::string description;
    std::optional<ClassInfo> interfaceClass;
    std::optional<std::string> interfaceProject;
};

struct ToolsInfo {
    std::string location;
    std::string organization;
    std::string deployment;
};

struct Inspector {
    std::string name;
    std::string path;
    std::string type;
    std::string description;
};

/**
 * Coding guidelines configuration
 */
struct CodingGuidelines {
    std::string language;
    std::string version;
    std::vector<CodingRule> rules;
};

struct CodingRule {
    std::string id;
    std::string category;
    std::string rule;
    std::optional<std::string> enforcement;
    std::optional<std::vector<std::string>> alternatives;
};

/**
 * Complete rules configuration
 */
struct RulesConfig {
    CodingGuidelines general;
    ProjectStructure projectOverview;
    std::optional<std::vector<Rule>> cursorRules;
};


} // namespace elizaos
