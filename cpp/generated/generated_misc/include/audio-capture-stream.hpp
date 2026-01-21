#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_AUDIO-CAPTURE-STREAM_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_AUDIO-CAPTURE-STREAM_H
#include "core.h"
#include "@elizaos/core.h"
#include "child_process.h"
#include "events.h"

class StreamingAudioConfig;
class AudioChunk;
class StreamingAudioCaptureService;

class StreamingAudioConfig : public object, public std::enable_shared_from_this<StreamingAudioConfig> {
public:
    using std::enable_shared_from_this<StreamingAudioConfig>::shared_from_this;
    boolean enabled;

    std::string device;

    double sampleRate;

    double channels;

    double vadThreshold;

    double silenceTimeout;

    double responseDelay;

    double chunkSize;
};

class AudioChunk : public object, public std::enable_shared_from_this<AudioChunk> {
public:
    using std::enable_shared_from_this<AudioChunk>::shared_from_this;
    std::shared_ptr<Buffer> data;

    double timestamp;

    double energy;
};

class StreamingAudioCaptureService : public EventEmitter, public std::enable_shared_from_this<StreamingAudioCaptureService> {
public:
    using std::enable_shared_from_this<StreamingAudioCaptureService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    std::shared_ptr<StreamingAudioConfig> config;

    std::any captureProcess = nullptr;

    boolean isCapturing = false;

    array<std::shared_ptr<AudioChunk>> audioBuffer = array<std::shared_ptr<AudioChunk>>();

    boolean isSpeaking = false;

    double lastSpeechTime = 0;

    std::any silenceTimer = nullptr;

    boolean transcriptionInProgress = false;

    std::string currentTranscription = string_empty;

    std::any responseTimer = nullptr;

    StreamingAudioCaptureService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<StreamingAudioConfig> config);
    virtual std::shared_ptr<Promise<void>> initialize();
    virtual std::shared_ptr<Promise<void>> startContinuousCapture();
    virtual void processAudioChunk(std::shared_ptr<Buffer> chunk);
    virtual double calculateEnergy(std::shared_ptr<Buffer> chunk);
    virtual std::shared_ptr<Promise<void>> startStreamingTranscription();
    virtual void endSpeech();
    virtual std::shared_ptr<Promise<void>> processFinalTranscription();
    virtual std::shared_ptr<Buffer> getRecentAudioData();
    virtual std::shared_ptr<Promise<any>> transcribeAudio(std::shared_ptr<Buffer> audioData);
    virtual std::shared_ptr<Buffer> rawToWav(std::shared_ptr<Buffer> rawData);
    virtual std::shared_ptr<Promise<void>> generateResponse(std::string transcription);
    virtual std::shared_ptr<Promise<void>> createAudioMemory(std::string transcription);
    virtual std::shared_ptr<Promise<void>> stop();
    virtual boolean isActive();
    virtual std::string getCurrentTranscription();
    virtual boolean isSpeechActive();
    StreamingAudioCaptureService(std::shared_ptr<EventEmitterOptions> options = undefined);
};

#endif
