#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE_CONFIRMATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE_CONFIRMATION_H
#include "core.hpp"
#include "react.hpp"
// External dependency removed
// Using alias removed (invalid transpilation)

class ConfirmationOptions;

class ConfirmationOptions : public object, public std::enable_shared_from_this<ConfirmationOptions> {
public:
    using std::enable_shared_from_this<ConfirmationOptions>::shared_from_this;
    string title;

    string description;

    string confirmText;

    string cancelText;

    any variant;
};

any useConfirmation();

#endif
