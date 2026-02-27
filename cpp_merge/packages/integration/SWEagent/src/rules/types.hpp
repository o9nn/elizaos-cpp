#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RULES_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RULES_TYPES_H
#include "core.hpp"

class Rule;
class RuleContent;
class ProjectStructure;
class EntryPoint;
class ClassInfo;
class ExecutionEnvironment;
class ToolsInfo;
class Inspector;
class CodingGuidelines;
class CodingRule;
class RulesConfig;

class Rule : public object, public std::enable_shared_from_this<Rule> {
public:
    using std::enable_shared_from_this<Rule>::shared_from_this;
    string name;

    string description;

    array<string> globs;

    boolean alwaysApply;

    any content;
};

class RuleContent : public object, public std::enable_shared_from_this<RuleContent> {
public:
    using std::enable_shared_from_this<RuleContent>::shared_from_this;
    string title;

    array<string> guidelines;

    string overview;

    std::shared_ptr<ProjectStructure> projectStructure;
};

class ProjectStructure : public object, public std::enable_shared_from_this<ProjectStructure> {
public:
    using std::enable_shared_from_this<ProjectStructure>::shared_from_this;
    array<std::shared_ptr<EntryPoint>> mainEntryPoints;

    std::shared_ptr<ClassInfo> mainClass;

    std::shared_ptr<ExecutionEnvironment> executionEnvironment;

    std::shared_ptr<ToolsInfo> tools;

    array<std::shared_ptr<Inspector>> inspectors;
};

class EntryPoint : public object, public std::enable_shared_from_this<EntryPoint> {
public:
    using std::enable_shared_from_this<EntryPoint>::shared_from_this;
    string path;

    string description;

    any type;
};

class ClassInfo : public object, public std::enable_shared_from_this<ClassInfo> {
public:
    using std::enable_shared_from_this<ClassInfo>::shared_from_this;
    string name;

    string path;

    string description;
};

class ExecutionEnvironment : public object, public std::enable_shared_from_this<ExecutionEnvironment> {
public:
    using std::enable_shared_from_this<ExecutionEnvironment>::shared_from_this;
    string type;

    string description;

    std::shared_ptr<ClassInfo> interfaceClass;

    string interfaceProject;
};

class ToolsInfo : public object, public std::enable_shared_from_this<ToolsInfo> {
public:
    using std::enable_shared_from_this<ToolsInfo>::shared_from_this;
    string location;

    string organization;

    string deployment;
};

class Inspector : public object, public std::enable_shared_from_this<Inspector> {
public:
    using std::enable_shared_from_this<Inspector>::shared_from_this;
    string name;

    string path;

    any type;

    string description;
};

class CodingGuidelines : public object, public std::enable_shared_from_this<CodingGuidelines> {
public:
    using std::enable_shared_from_this<CodingGuidelines>::shared_from_this;
    any language;

    string version;

    array<std::shared_ptr<CodingRule>> rules;
};

class CodingRule : public object, public std::enable_shared_from_this<CodingRule> {
public:
    using std::enable_shared_from_this<CodingRule>::shared_from_this;
    string id;

    any category;

    string rule;

    any enforcement;

    array<string> alternatives;
};

class RulesConfig : public object, public std::enable_shared_from_this<RulesConfig> {
public:
    using std::enable_shared_from_this<RulesConfig>::shared_from_this;
    std::shared_ptr<CodingGuidelines> general;

    std::shared_ptr<ProjectStructure> projectOverview;

    array<std::shared_ptr<Rule>> cursorRules;
};

#endif
