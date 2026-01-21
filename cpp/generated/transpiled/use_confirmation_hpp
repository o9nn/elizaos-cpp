#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE-CONFIRMATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE-CONFIRMATION_H
#include "core.h"
#include "react.h"
#include "@/components/confirmation-dialog.h"
using ConfirmationDialog = _default;

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
