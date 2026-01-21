#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_AISERVICE_UTILS_CODEFORMATTER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_AISERVICE_UTILS_CODEFORMATTER_H
#include "core.h"
#include "../types.h"

class CodeFormatter;

class CodeFormatter : public object, public std::enable_shared_from_this<CodeFormatter> {
public:
    using std::enable_shared_from_this<CodeFormatter>::shared_from_this;
    virtual string ensureTypeScriptExtension(string filePath);
    virtual string formatApiComponents(std::shared_ptr<FileDocsGroup> fileGroup);
    virtual string formatComponents(std::shared_ptr<FileDocsGroup> fileGroup);
    virtual string formatFilePath(string filePath);
    virtual string formatJSDoc(string jsDoc, string _code = undefined);
    virtual string truncateCodeBlock(string code, double maxLength = 8000);
};

#endif
