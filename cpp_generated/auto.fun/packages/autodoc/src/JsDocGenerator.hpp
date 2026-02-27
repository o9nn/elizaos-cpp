#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_JSDOCGENERATOR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_JSDOCGENERATOR_H
#include "core.h"
#include "./AIService/AIService.js.h"
#include "./types/index.js.h"

class JsDocGenerator;

class JsDocGenerator : public object, public std::enable_shared_from_this<JsDocGenerator> {
public:
    using std::enable_shared_from_this<JsDocGenerator>::shared_from_this;
    std::shared_ptr<AIService> aiService;

    JsDocGenerator(std::shared_ptr<AIService> aiService_);
    virtual std::shared_ptr<Promise<string>> generateComment(std::shared_ptr<ASTQueueItem> queueItem);
    virtual std::shared_ptr<Promise<string>> generateClassComment(std::shared_ptr<ASTQueueItem> queueItem);
    virtual string buildPrompt(std::shared_ptr<ASTQueueItem> queueItem);
    virtual string buildClassPrompt(std::shared_ptr<ASTQueueItem> queueItem);
};

#endif
