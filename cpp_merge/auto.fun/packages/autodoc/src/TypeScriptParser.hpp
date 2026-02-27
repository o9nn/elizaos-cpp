#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPESCRIPTPARSER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPESCRIPTPARSER_H
#include "core.hpp"
#include "node:fs.h"
// Using alias removed (invalid transpilation)
// External dependency removed
#include "./types.h"

class TypeScriptParser;

class TypeScriptParser : public object, public std::enable_shared_from_this<TypeScriptParser> {
public:
    using std::enable_shared_from_this<TypeScriptParser>::shared_from_this;
    virtual any parse(string file);
    virtual object extractExports(string file);
    virtual any findActionBounds(any ast);
    virtual string extractActionCode(string filePath, std::shared_ptr<ActionBounds> bounds);
    virtual void handleParseError(std::shared_ptr<Error> error);
};

#endif
