#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPESCRIPTPARSER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPESCRIPTPARSER_H
#include "core.h"
#include "node:fs.h"
using fs = _default;
#include "@typescript-eslint/parser.h"
#include "./types.h"

class TypeScriptParser;

class TypeScriptParser : public object, public std::enable_shared_from_this<TypeScriptParser> {
public:
    using std::enable_shared_from_this<TypeScriptParser>::shared_from_this;
    virtual std::any parse(std::string file);
    virtual object extractExports(std::string file);
    virtual std::any findActionBounds(std::any ast);
    virtual std::string extractActionCode(std::string filePath, std::shared_ptr<ActionBounds> bounds);
    virtual void handleParseError(std::shared_ptr<Error> error);
};

#endif
