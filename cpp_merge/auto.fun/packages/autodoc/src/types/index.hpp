#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPES_INDEX_H
#include "core.hpp"
// External dependency removed

class ASTQueueItem;
class Repository;
class FullModeFileChange;
class PrModeFileChange;
class TodoSection;
class TodoItem;
class EnvUsage;
class PluginDocumentation;
class ActionMetadata;
class ActionBounds;

class ASTQueueItem : public object, public std::enable_shared_from_this<ASTQueueItem> {
public:
    using std::enable_shared_from_this<ASTQueueItem>::shared_from_this;
    string name;

    string filePath;

    double startLine;

    double endLine;

    string nodeType;

    string code;

    string className;

    string methodName;

    string jsDoc;
};

class Repository : public object, public std::enable_shared_from_this<Repository> {
public:
    using std::enable_shared_from_this<Repository>::shared_from_this;
    string owner;

    string name;

    double pullNumber;
};

class FullModeFileChange : public object, public std::enable_shared_from_this<FullModeFileChange> {
public:
    using std::enable_shared_from_this<FullModeFileChange>::shared_from_this;
    string filename;

    string status;
};

class PrModeFileChange : public FullModeFileChange, public std::enable_shared_from_this<PrModeFileChange> {
public:
    using std::enable_shared_from_this<PrModeFileChange>::shared_from_this;
    double additions;

    double deletions;

    double changes;

    string contents_url;
};

class TodoSection : public object, public std::enable_shared_from_this<TodoSection> {
public:
    using std::enable_shared_from_this<TodoSection>::shared_from_this;
    string todos;

    double todoCount;
};

class TodoItem : public object, public std::enable_shared_from_this<TodoItem> {
public:
    using std::enable_shared_from_this<TodoItem>::shared_from_this;
    string comment;

    string code;

    string fullContext;

    std::shared_ptr<TSESTree::Node> node;

    object location;

    object contextLocation;
};

class EnvUsage : public object, public std::enable_shared_from_this<EnvUsage> {
public:
    using std::enable_shared_from_this<EnvUsage>::shared_from_this;
    string code;

    string context;

    string fullContext;

    std::shared_ptr<TSESTree::Node> node;

    object location;

    object contextLocation;
};

class PluginDocumentation : public object, public std::enable_shared_from_this<PluginDocumentation> {
public:
    using std::enable_shared_from_this<PluginDocumentation>::shared_from_this;
    string overview;

    string installation;

    string configuration;

    string usage;

    string apiReference;

    string troubleshooting;

    string todos;

    string actionsDocumentation;

    string providersDocumentation;

    string evaluatorsDocumentation;

    string faq;
};

class ActionMetadata : public object, public std::enable_shared_from_this<ActionMetadata> {
public:
    using std::enable_shared_from_this<ActionMetadata>::shared_from_this;
    string name;

    array<string> similes;

    string validate;

    string handler;

    array<string> examples;

    string description;
};

class ActionBounds : public object, public std::enable_shared_from_this<ActionBounds> {
public:
    using std::enable_shared_from_this<ActionBounds>::shared_from_this;
    double startLine;

    double endLine;
};

#endif
