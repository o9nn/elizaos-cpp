#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_MANAGERS_VOICE_MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_MANAGERS_VOICE_MANAGER_H
#include "core.hpp"
// External dependency removed
#include "../service.h"
#include "../utils.h"
#include "./guards.h"
#include "../events.h"

typedef object LiveKitAudioData;

class VoiceManager;

class VoiceManager : public object, public std::enable_shared_from_this<VoiceManager> {
public:
    using std::enable_shared_from_this<VoiceManager>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    std::shared_ptr<Map<string, object>> userStates = std::make_shared<Map>();

    boolean processingVoice = false;

    any transcriptionTimeout = nullptr;

    VoiceManager(std::shared_ptr<IAgentRuntime> runtime);
    virtual void start();
    virtual void handleUserBuffer(any playerId, any buffer);
    virtual void debouncedProcessTranscription(std::shared_ptr<UUID> playerId);
    virtual void processTranscription(std::shared_ptr<UUID> playerId);
    virtual any handleMessage(string message, std::shared_ptr<UUID> playerId);
    virtual void playAudio(any audioBuffer);
    virtual any getService();
};

#endif
