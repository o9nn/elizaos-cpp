#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_PLUGINDOCUMENTATIONGENERATOR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_PLUGINDOCUMENTATIONGENERATOR_H
#include "core.h"
#include "node:fs.h"
using fs = _default;
#include "node:path.h"
using path = _default;
#include "./AIService/AIService.js.h"
#include "./AIService/generators/FullDocumentationGenerator.js.h"
#include "./Configuration.js.h"
#include "./GitManager.js.h"
#include "./types/index.js.h"

class PluginDocumentationGenerator;

class PluginDocumentationGenerator : public object, public std::enable_shared_from_this<PluginDocumentationGenerator> {
public:
    using std::enable_shared_from_this<PluginDocumentationGenerator>::shared_from_this;
    std::shared_ptr<AIService> aiService;

    std::shared_ptr<GitManager> gitManager;

    std::shared_ptr<Configuration> configuration;

    std::shared_ptr<FullDocumentationGenerator> fullDocumentationGenerator;

    PluginDocumentationGenerator(std::shared_ptr<AIService> aiService_, std::shared_ptr<GitManager> gitManager_, std::shared_ptr<Configuration> configuration_);
    virtual std::shared_ptr<Promise<void>> generate(array<std::shared_ptr<ASTQueueItem>> existingDocs, string branchName = undefined, array<std::shared_ptr<TodoItem>> todoItems = array<std::shared_ptr<TodoItem>>(), array<std::shared_ptr<EnvUsage>> envUsages = array<std::shared_ptr<EnvUsage>>());
    virtual string generateMarkdownContent(std::shared_ptr<PluginDocumentation> docs, any packageJson);
};

#endif
