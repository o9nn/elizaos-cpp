#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_AUTODOC_SRC_AISERVICE_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_AUTODOC_SRC_AISERVICE_TYPES_INDEX_H
#include "core.hpp"
#include "../../types.h"

class FileDocsGroup;
class OrganizedDocs;

class FileDocsGroup : public object, public std::enable_shared_from_this<FileDocsGroup> {
public:
    using std::enable_shared_from_this<FileDocsGroup>::shared_from_this;
    string filePath;

    array<std::shared_ptr<ASTQueueItem>> classes;

    array<std::shared_ptr<ASTQueueItem>> methods;

    array<std::shared_ptr<ASTQueueItem>> interfaces;

    array<std::shared_ptr<ASTQueueItem>> types;

    array<std::shared_ptr<ASTQueueItem>> functions;
};

class OrganizedDocs : public object, public std::enable_shared_from_this<OrganizedDocs> {
public:
    using std::enable_shared_from_this<OrganizedDocs>::shared_from_this;
    array<std::shared_ptr<ASTQueueItem>> classes;

    array<std::shared_ptr<ASTQueueItem>> methods;

    array<std::shared_ptr<ASTQueueItem>> interfaces;

    array<std::shared_ptr<ASTQueueItem>> types;

    array<std::shared_ptr<ASTQueueItem>> functions;

    array<std::shared_ptr<ASTQueueItem>> variables;
};

#endif
