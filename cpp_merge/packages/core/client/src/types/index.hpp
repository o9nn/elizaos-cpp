#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_TYPES_INDEX_H
#include "core.hpp"

class IAttachment;

class IAttachment : public object, public std::enable_shared_from_this<IAttachment> {
public:
    using std::enable_shared_from_this<IAttachment>::shared_from_this;
    string url;

    string contentType;

    string title;
};

#endif
