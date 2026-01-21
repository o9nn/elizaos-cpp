#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_AISERVICE_UTILS_DOCUMENTORGANIZER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_AUTODOC_SRC_AISERVICE_UTILS_DOCUMENTORGANIZER_H
#include "core.h"
#include "../../types.h"
#include "../types.h"

class DocumentOrganizer;

class DocumentOrganizer : public object, public std::enable_shared_from_this<DocumentOrganizer> {
public:
    using std::enable_shared_from_this<DocumentOrganizer>::shared_from_this;
    virtual std::shared_ptr<OrganizedDocs> organizeDocumentation(array<std::shared_ptr<ASTQueueItem>> docs);
    virtual array<std::shared_ptr<FileDocsGroup>> groupDocsByFile(std::shared_ptr<OrganizedDocs> docs);
};

#endif
