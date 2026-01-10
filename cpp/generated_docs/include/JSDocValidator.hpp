#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_JSDOCVALIDATOR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_JSDOCVALIDATOR_H
#include "core.h"
#include "@typescript-eslint/parser.h"
#include "./AIService/AIService.js.h"

class JSDocValidator;

class JSDocValidator : public object, public std::enable_shared_from_this<JSDocValidator> {
public:
    using std::enable_shared_from_this<JSDocValidator>::shared_from_this;
    std::shared_ptr<AIService> aiService;

    std::shared_ptr<ParserOptions> parserOptions = object{
        object::pair{std::string("sourceType"), std::string("module")}, 
        object::pair{std::string("ecmaVersion"), 2020}, 
        object::pair{std::string("ecmaFeatures"), object{
            object::pair{std::string("jsx"), true}
        }}, 
        object::pair{std::string("range"), true}, 
        object::pair{std::string("loc"), true}, 
        object::pair{std::string("tokens"), true}, 
        object::pair{std::string("comment"), true}
    };

    JSDocValidator(std::shared_ptr<AIService> aiService_);
    virtual std::shared_ptr<Promise<string>> validateAndFixJSDoc(string fileName, string code, string originalComment);
    virtual boolean isValidTypeScript(string code);
    virtual string fixCommonJSDocIssues(string comment);
    virtual std::shared_ptr<Promise<string>> regenerateJSDoc(string code);
};

#endif
