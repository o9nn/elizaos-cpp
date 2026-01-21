#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_PLUGIN-HYPERFY_MANAGERS_EMOTE-MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_PLUGIN-HYPERFY_MANAGERS_EMOTE-MANAGER_H
#include "core.h"
#include "fs/promises.h"
using fs = _default;
#include "path.h"
using path = _default;
#include "../constants.js.h"
#include "../hyperfy/src/core/extras/playerEmotes.js.h"
#include "../utils.h"
#include "@elizaos/core.h"
#include "../service.js.h"

class EmoteManager;

class EmoteManager : public object, public std::enable_shared_from_this<EmoteManager> {
public:
    using std::enable_shared_from_this<EmoteManager>::shared_from_this;
    std::shared_ptr<Map<std::string, string>> emoteHashMap;

    std::any currentEmoteTimeout;

    std::any movementCheckInterval = nullptr;

    std::shared_ptr<IAgentRuntime> runtime;

    EmoteManager(std::any runtime);
    virtual void uploadEmotes();
    virtual void playEmote(std::string name);
    virtual void clearEmote(std::any player);
    virtual void clearTimers();
    virtual std::any getService();
};

#endif
