#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_AUTODOC_SRC_TYPESCRIPTFILEIDENTIFIER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_AUTODOC_SRC_TYPESCRIPTFILEIDENTIFIER_H
#include "core.hpp"
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)

class TypeScriptFileIdentifier;

class TypeScriptFileIdentifier : public object, public std::enable_shared_from_this<TypeScriptFileIdentifier> {
public:
    using std::enable_shared_from_this<TypeScriptFileIdentifier>::shared_from_this;
    virtual boolean isTypeScriptFile(string file);
    virtual array<string> getTypeScriptFiles(string directory);
};

#endif
