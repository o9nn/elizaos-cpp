#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE_ELEVENLABS_VOICES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_HOOKS_USE_ELEVENLABS_VOICES_H
#include "core.hpp"
#include "react.hpp"
// External dependency removed
// External dependency removed
// External dependency removed

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
