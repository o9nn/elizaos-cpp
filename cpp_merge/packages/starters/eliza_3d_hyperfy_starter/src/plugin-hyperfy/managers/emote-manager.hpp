#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_MANAGERS_EMOTE_MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_MANAGERS_EMOTE_MANAGER_H
#include "core.hpp"
#include "fs/promises.h"
// Using alias removed (invalid transpilation)
#include "path.hpp"
// Using alias removed (invalid transpilation)
#include "../constants.js.h"
#include "../hyperfy/src/core/extras/playerEmotes.js.h"
#include "../utils.h"
// External dependency removed
#include "../service.js.h"

class EmoteManager;

class EmoteManager : public object, public std::enable_shared_from_this<EmoteManager> {
public:
    using std::enable_shared_from_this<EmoteManager>::shared_from_this;
    std::shared_ptr<Map<string, string>> emoteHashMap;

    any currentEmoteTimeout;

    any movementCheckInterval = nullptr;

    std::shared_ptr<IAgentRuntime> runtime;

    EmoteManager(any runtime);
    virtual void uploadEmotes();
    virtual void playEmote(string name);
    virtual void clearEmote(any player);
    virtual void clearTimers();
    virtual any getService();
};

#endif
