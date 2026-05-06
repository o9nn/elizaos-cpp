#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE-ELEVENLABS-VOICES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE-ELEVENLABS-VOICES_H
#include "core.h"
#include "react.h"
#include "@tanstack/react-query.h"
#include "@/config/voice-models.h"
#include "@/config/voice-models.h"

class ElevenLabsVoice;

class ElevenLabsVoice : public object, public std::enable_shared_from_this<ElevenLabsVoice> {
public:
    using std::enable_shared_from_this<ElevenLabsVoice>::shared_from_this;
    string voice_id;

    string name;

    string category;

    object labels;

    string preview_url;
};

any useElevenLabsVoices();

#endif
