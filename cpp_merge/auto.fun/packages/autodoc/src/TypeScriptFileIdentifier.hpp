#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPESCRIPTFILEIDENTIFIER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPESCRIPTFILEIDENTIFIER_H
#include "core.hpp"
#include "node:fs.h"
// Using alias removed (invalid transpilation)
#include "node:path.h"
// Using alias removed (invalid transpilation)

class TypeScriptFileIdentifier;

class TypeScriptFileIdentifier : public object, public std::enable_shared_from_this<TypeScriptFileIdentifier> {
public:
    using std::enable_shared_from_this<TypeScriptFileIdentifier>::shared_from_this;
    virtual boolean isTypeScriptFile(string file);
    virtual array<string> getTypeScriptFiles(string directory);
};

#endif
