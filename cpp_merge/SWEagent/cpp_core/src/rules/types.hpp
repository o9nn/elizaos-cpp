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
    std: name;
    std::optional<std:> description;
    std::optional<std::vector<std::string>> globs;
    bool alwaysApply;
    std::variant<std:, RuleContent> content;
};

/**
 * Structured rule content
 */
struct RuleContent {
    std::optional<std:> title;
    std::optional<std::vector<std::string>> guidelines;
    std::optional<std:> overview;
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
    std: path;
    std: description;
    std: type;
};

struct ClassInfo {
    std: name;
    std: path;
    std: description;
};

struct ExecutionEnvironment {
    std: type;
    std: description;
    std::optional<ClassInfo> interfaceClass;
    std::optional<std:> interfaceProject;
};

struct ToolsInfo {
    std: location;
    std: organization;
    std: deployment;
};

struct Inspector {
    std: name;
    std: path;
    std: type;
    std: description;
};

/**
 * Coding guidelines configuration
 */
struct CodingGuidelines {
    std: language;
    std: version;
    std::vector<CodingRule> rules;
};

struct CodingRule {
    std: id;
    std: category;
    std: rule;
    std::optional<std:> enforcement;
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
