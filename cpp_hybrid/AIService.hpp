#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_AISERVICE_AISERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_AISERVICE_AISERVICE_H
#include "core.h"
#include "@langchain/openai.h"
#include "dotenv.h"
using dotenv = _default;
#include "../Configuration.js.h"
#include "../TypeScriptParser.js.h"
#include "./utils/CodeFormatter.js.h"
#include "./utils/DocumentOrganizer.js.h"

class AIService;

class AIService : public object, public std::enable_shared_from_this<AIService> {
public:
    using std::enable_shared_from_this<AIService>::shared_from_this;
    std::shared_ptr<Configuration> configuration;

    std::shared_ptr<ChatOpenAI> chatModel;

    std::shared_ptr<CodeFormatter> codeFormatter;

    std::shared_ptr<ChatOpenAI> chatModelFAQ;

    AIService(std::shared_ptr<Configuration> configuration_);
    virtual std::shared_ptr<Promise<string>> generateComment(string prompt, boolean isFAQ = false);
    virtual void handleAPIError(std::shared_ptr<Error> error);
};

#endif
