#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_DIRECTORYTRAVERSAL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_DIRECTORYTRAVERSAL_H
#include "core.h"
#include "node:fs.h"
using fs = _default;
#include "node:path.h"
using path = _default;
#include "./Configuration.js.h"

class DirectoryTraversal;

class DirectoryTraversal : public object, public std::enable_shared_from_this<DirectoryTraversal> {
public:
    using std::enable_shared_from_this<DirectoryTraversal>::shared_from_this;
    std::shared_ptr<Configuration> config;

    array<string> prFiles = array<string>();

    static array<string> FORCED_EXCLUDED_DIRS;

    DirectoryTraversal(std::shared_ptr<Configuration> config_, array<string> prFiles_ = array<string>());
    virtual string getAbsolutePath(string filePath);
    virtual string getRelativePath(string filePath);
    virtual array<string> traverse();
    virtual boolean isExcluded(string absolutePath);
};

#endif
