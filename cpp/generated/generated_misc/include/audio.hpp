#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_TYPES_AUDIO_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_TYPES_AUDIO_H
#include "core.h"
#include "@elizaos/core.h"

class SpeechConversationParams;
class SpeechGenerateParams;
class AudioSynthesizeParams;
class TranscribeParams;
class SpeechResponse;
class TranscriptionResponse;

class SpeechConversationParams : public object, public std::enable_shared_from_this<SpeechConversationParams> {
public:
    using std::enable_shared_from_this<SpeechConversationParams>::shared_from_this;
    std::any audio;

    std::any format;

    std::string language;

    Record<std::string, any> metadata;
};

class SpeechGenerateParams : public object, public std::enable_shared_from_this<SpeechGenerateParams> {
public:
    using std::enable_shared_from_this<SpeechGenerateParams>::shared_from_this;
    std::string text;

    std::string voice;

    std::string language;

    double speed;

    double pitch;
};

class AudioSynthesizeParams : public object, public std::enable_shared_from_this<AudioSynthesizeParams> {
public:
    using std::enable_shared_from_this<AudioSynthesizeParams>::shared_from_this;
    std::shared_ptr<UUID> messageId;

    std::string voice;

    std::any format;
};

class TranscribeParams : public object, public std::enable_shared_from_this<TranscribeParams> {
public:
    using std::enable_shared_from_this<TranscribeParams>::shared_from_this;
    std::any audio;

    std::any format;

    std::string language;
};

class SpeechResponse : public object, public std::enable_shared_from_this<SpeechResponse> {
public:
    using std::enable_shared_from_this<SpeechResponse>::shared_from_this;
    std::string text;

    std::string audio;

    double duration;

    Record<std::string, any> metadata;
};

class TranscriptionResponse : public object, public std::enable_shared_from_this<TranscriptionResponse> {
public:
    using std::enable_shared_from_this<TranscriptionResponse>::shared_from_this;
    std::string text;

    std::string language;

    double confidence;

    array<object> words;
};

#endif
