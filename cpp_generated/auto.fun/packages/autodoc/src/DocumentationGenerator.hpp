#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_DOCUMENTATIONGENERATOR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_DOCUMENTATIONGENERATOR_H
#include "core.h"
#include "node:fs.h"
using fs = _default;
#include "node:path.h"
using path = _default;
#include "@typescript-eslint/types.h"
#include "./AIService/AIService.js.h"
#include "./Configuration.js.h"
#include "./DirectoryTraversal.js.h"
#include "./GitManager.js.h"
#include "./JSDocValidator.js.h"
#include "./JsDocAnalyzer.js.h"
#include "./JsDocGenerator.js.h"
#include "./PluginDocumentationGenerator.js.h"
#include "./TypeScriptParser.js.h"
#include "./types/index.js.h"

class DocumentationGenerator;

class DocumentationGenerator : public object, public std::enable_shared_from_this<DocumentationGenerator> {
public:
    using std::enable_shared_from_this<DocumentationGenerator>::shared_from_this;
    std::shared_ptr<DirectoryTraversal> directoryTraversal;

    std::shared_ptr<TypeScriptParser> typeScriptParser;

    std::shared_ptr<JsDocAnalyzer> jsDocAnalyzer;

    std::shared_ptr<JsDocGenerator> jsDocGenerator;

    std::shared_ptr<GitManager> gitManager;

    std::shared_ptr<Configuration> configuration;

    std::shared_ptr<AIService> aiService;

    array<std::shared_ptr<ASTQueueItem>> missingJsDocQueue = array<std::shared_ptr<ASTQueueItem>>();

    array<std::shared_ptr<ASTQueueItem>> existingJsDocQueue = array<std::shared_ptr<ASTQueueItem>>();

    boolean hasChanges = false;

    std::shared_ptr<Map<string, string>> fileContents = std::make_shared<Map>();

    string branchName = string_empty;

    std::shared_ptr<Map<string, double>> fileOffsets = std::make_shared<Map>();

    array<string> typeScriptFiles = array<string>();

    std::shared_ptr<JSDocValidator> jsDocValidator;

    DocumentationGenerator(std::shared_ptr<DirectoryTraversal> directoryTraversal_, std::shared_ptr<TypeScriptParser> typeScriptParser_, std::shared_ptr<JsDocAnalyzer> jsDocAnalyzer_, std::shared_ptr<JsDocGenerator> jsDocGenerator_, std::shared_ptr<GitManager> gitManager_, std::shared_ptr<Configuration> configuration_, std::shared_ptr<AIService> aiService_);
    virtual std::shared_ptr<Promise<object>> generate(double pullNumber = undefined);
    virtual void processNode(std::shared_ptr<TSESTree::Node> node, string filePath, std::shared_ptr<TSESTree::Program> ast);
    virtual std::shared_ptr<Promise<void>> updateFileWithJSDoc(string filePath, string jsDoc, double insertLine);
    virtual string getNodeCode(string filePath, std::shared_ptr<TSESTree::Node> node);
    virtual std::shared_ptr<Promise<string>> getFileContent(string contentsUrl);
    virtual std::shared_ptr<Promise<object>> generatePRContent(double pullNumber = undefined);
    virtual string generateDefaultPRBody();
    virtual std::shared_ptr<Promise<object>> analyzeCodebase();
};

#endif
