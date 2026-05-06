#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_TYPES_INDEX_H
#include "core.h"

class IAttachment;

class IAttachment : public object, public std::enable_shared_from_this<IAttachment> {
public:
    using std::enable_shared_from_this<IAttachment>::shared_from_this;
    string url;

    string contentType;

    string title;
};

#endif
