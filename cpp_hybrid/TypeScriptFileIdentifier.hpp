#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPESCRIPTFILEIDENTIFIER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_TYPESCRIPTFILEIDENTIFIER_H
#include "core.h"
#include "node:fs.h"
using fs = _default;
#include "node:path.h"
using path = _default;

class TypeScriptFileIdentifier;

class TypeScriptFileIdentifier : public object, public std::enable_shared_from_this<TypeScriptFileIdentifier> {
public:
    using std::enable_shared_from_this<TypeScriptFileIdentifier>::shared_from_this;
    virtual boolean isTypeScriptFile(string file);
    virtual array<string> getTypeScriptFiles(string directory);
};

#endif
