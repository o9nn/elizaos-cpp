#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_SRC_HOOKS_USETAURICHAT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_SRC_HOOKS_USETAURICHAT_H
#include "core.h"
#include "react.h"
#include "../services/TauriService.h"

class UseTauriChatReturn;

class UseTauriChatReturn : public object, public std::enable_shared_from_this<UseTauriChatReturn> {
public:
    using std::enable_shared_from_this<UseTauriChatReturn>::shared_from_this;
    boolean isConnected;

    std::function<std::shared_ptr<Promise<void>>(string)> sendMessage;

    array<std::shared_ptr<TauriMessage>> messages;

    any error;

    boolean isLoading;
};

std::shared_ptr<UseTauriChatReturn> useTauriChat();

#endif
