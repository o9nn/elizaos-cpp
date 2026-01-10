#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/audio-capture-stream.h"

StreamingAudioCaptureService::StreamingAudioCaptureService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<StreamingAudioConfig> config) : EventEmitter() {
    this->runtime = runtime;
    this->config = utils::assign(object{
        object::pair{std::string("sampleRate"), 16000}, 
        object::pair{std::string("channels"), 1}, 
        object::pair{std::string("vadThreshold"), 0.01}, 
        object::pair{std::string("silenceTimeout"), 1500}, 
        object::pair{std::string("responseDelay"), 7777}, 
        object::pair{std::string("chunkSize"), 4096}
    }, config);
}

std::shared_ptr<Promise<void>> StreamingAudioCaptureService::initialize()
{
    if (!this->config->enabled) {
        logger->info(std::string("[StreamingAudio] Audio capture disabled"));
        return std::shared_ptr<Promise<void>>();
    }
    try
    {
        logger->info(std::string("[StreamingAudio] Initializing streaming audio capture..."));
        std::async([=]() { this->startContinuousCapture(); });
        logger->info(std::string("[StreamingAudio] Streaming audio capture initialized"));
    }
    catch (const any& error)
    {
        logger->error(std::string("[StreamingAudio] Failed to initialize:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<void>> StreamingAudioCaptureService::startContinuousCapture()
{
    auto platform = process->platform;
    string command;
    array<string> args;
    if (platform == std::string("darwin")) {
        command = std::string("sox");
        args = array<string>{ std::string("-d"), std::string("-r"), this->config->sampleRate->toString(), std::string("-c"), this->config->channels->toString(), std::string("-b"), std::string("16"), std::string("-e"), std::string("signed"), std::string("-t"), std::string("raw"), std::string("-") };
    } else if (platform == std::string("linux")) {
        command = std::string("arecord");
        args = array<string>{ std::string("-D"), OR((this->config->device), (std::string("default"))), std::string("-f"), std::string("S16_LE"), std::string("-r"), this->config->sampleRate->toString(), std::string("-c"), this->config->channels->toString(), std::string("-t"), std::string("raw"), std::string("-") };
    } else if (platform == std::string("win32")) {
        command = std::string("ffmpeg");
        args = array<string>{ std::string("-f"), std::string("dshow"), std::string("-i"), std::string("audio="") + (OR((this->config->device), (std::string("Microphone")))) + std::string("""), std::string("-acodec"), std::string("pcm_s16le"), std::string("-ar"), this->config->sampleRate->toString(), std::string("-ac"), this->config->channels->toString(), std::string("-f"), std::string("s16le"), std::string("pipe:1") };
    } else {
        throw any(std::make_shared<Error>(std::string("Unsupported platform: ") + platform + string_empty));
    }
    this->captureProcess = spawn(command, args);
    this->isCapturing = true;
    this->captureProcess->stdout->on(std::string("data"), [=](auto chunk) mutable
    {
        this->processAudioChunk(chunk);
    }
    );
    this->captureProcess->stderr->on(std::string("data"), [=](auto data) mutable
    {
        logger->debug(std::string("[StreamingAudio] Capture stderr:"), data["toString"]());
    }
    );
    this->captureProcess->on(std::string("error"), [=](auto error) mutable
    {
        logger->error(std::string("[StreamingAudio] Capture process error:"), error);
        this->isCapturing = false;
    }
    );
    this->captureProcess->on(std::string("exit"), [=](auto code) mutable
    {
        logger->info(std::string("[StreamingAudio] Capture process exited with code:"), code);
        this->isCapturing = false;
    }
    );
    return std::shared_ptr<Promise<void>>();
}

void StreamingAudioCaptureService::processAudioChunk(std::shared_ptr<Buffer> chunk)
{
    auto energy = this->calculateEnergy(chunk);
    shared timestamp = Date->now();
    auto audioChunk = object{
        object::pair{std::string("data"), chunk}, 
        object::pair{std::string("timestamp"), std::string("timestamp")}, 
        object::pair{std::string("energy"), std::string("energy")}
    };
    if (energy > this->config->vadThreshold) {
        if (!this->isSpeaking) {
            this->isSpeaking = true;
            this->lastSpeechTime = timestamp;
            logger->debug(std::string("[StreamingAudio] Speech detected, starting recording"));
            this->emit(std::string("speechStart"));
            if (this->responseTimer) {
                clearTimeout(this->responseTimer);
                this->responseTimer = nullptr;
                logger->debug(std::string("[StreamingAudio] Cancelled pending response due to new speech"));
            }
        }
        this->audioBuffer->push(audioChunk);
        this->lastSpeechTime = timestamp;
        if (this->silenceTimer) {
            clearTimeout(this->silenceTimer);
        }
        if (!this->transcriptionInProgress) {
            this->startStreamingTranscription();
        }
    } else if (this->isSpeaking) {
        this->audioBuffer->push(audioChunk);
        if (!this->silenceTimer) {
            this->silenceTimer = setTimeout([=]() mutable
            {
                this->endSpeech();
            }
            , this->config->silenceTimeout);
        }
    }
    shared cutoffTime = timestamp - 30000;
    this->audioBuffer = this->audioBuffer->filter([=](auto c) mutable
    {
        return c->timestamp > cutoffTime;
    }
    );
}

double StreamingAudioCaptureService::calculateEnergy(std::shared_ptr<Buffer> chunk)
{
    auto sum = 0;
    auto samples = chunk->length / 2;
    for (auto i = 0; i < chunk->length; i += 2)
    {
        auto sample = chunk->readInt16LE(i);
        sum += sample * sample;
    }
    auto rms = Math->sqrt(sum / samples);
    return rms / 32768;
}

std::shared_ptr<Promise<void>> StreamingAudioCaptureService::startStreamingTranscription()
{
    if (this->transcriptionInProgress) {
        return std::shared_ptr<Promise<void>>();
    }
    this->transcriptionInProgress = true;
    logger->debug(std::string("[StreamingAudio] Starting streaming transcription"));
    try
    {
        auto audioData = this->getRecentAudioData();
        if (audioData->length == 0) {
            this->transcriptionInProgress = false;
            return std::shared_ptr<Promise<void>>();
        }
        auto result = std::async([=]() { this->transcribeAudio(audioData); });
        if (AND((result), (result->trim()))) {
            this->currentTranscription = result;
            logger->info(std::string("[StreamingAudio] Partial transcription: "") + result + std::string("""));
            this->emit(std::string("transcription"), object{
                object::pair{std::string("text"), result}, 
                object::pair{std::string("isFinal"), false}
            });
        }
    }
    catch (const any& error)
    {
        logger->error(std::string("[StreamingAudio] Transcription error:"), error);
    }
    this->transcriptionInProgress = false;
    if (this->isSpeaking) {
        setTimeout([=]() mutable
        {
            return this->startStreamingTranscription();
        }
        , 500);
    }
}

void StreamingAudioCaptureService::endSpeech()
{
    if (!this->isSpeaking) {
        return;
    }
    this->isSpeaking = false;
    this->silenceTimer = nullptr;
    logger->debug(std::string("[StreamingAudio] Speech ended"));
    this->emit(std::string("speechEnd"));
    this->processFinalTranscription();
}

std::shared_ptr<Promise<void>> StreamingAudioCaptureService::processFinalTranscription()
{
    auto audioData = this->getRecentAudioData();
    if (audioData->length == 0) {
        return std::shared_ptr<Promise<void>>();
    }
    {
        utils::finally __finally8542_8645([&]() mutable
        {
            this->audioBuffer = array<any>();
            this->currentTranscription = string_empty;
        });
        try
        {
            shared finalText = std::async([=]() { this->transcribeAudio(audioData); });
            if (AND((finalText), (finalText->trim()))) {
                this->currentTranscription = finalText;
                logger->info(std::string("[StreamingAudio] Final transcription: "") + finalText + std::string("""));
                this->emit(std::string("transcription"), object{
                    object::pair{std::string("text"), finalText}, 
                    object::pair{std::string("isFinal"), true}
                });
                this->responseTimer = setTimeout([=]() mutable
                {
                    this->generateResponse(finalText);
                }
                , this->config->responseDelay);
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("[StreamingAudio] Final transcription error:"), error);
        }
    }
}

std::shared_ptr<Buffer> StreamingAudioCaptureService::getRecentAudioData()
{
    if (this->audioBuffer->get_length() == 0) {
        return Buffer::alloc(0);
    }
    shared startTime = const_(this->audioBuffer)[0]->timestamp;
    auto relevantChunks = this->audioBuffer->filter([=](auto c) mutable
    {
        return c->timestamp >= startTime;
    }
    );
    auto totalLength = relevantChunks->reduce([=](auto sum, auto c) mutable
    {
        return sum + c->data->length;
    }
    , 0);
    auto combined = Buffer::alloc(totalLength);
    auto offset = 0;
    auto& __array9126_9251 = relevantChunks;
    for (auto __indx9126_9251 = 0_N; __indx9126_9251 < __array9126_9251->get_length(); __indx9126_9251++)
    {
        auto& chunk = const_(__array9126_9251)[__indx9126_9251];
        {
            chunk->data->copy(combined, offset);
            offset += chunk->data->length;
        }
    }
    return combined;
}

std::shared_ptr<Promise<any>> StreamingAudioCaptureService::transcribeAudio(std::shared_ptr<Buffer> audioData)
{
    try
    {
        auto wavBuffer = this->rawToWav(audioData);
        auto result = std::async([=]() { this->runtime->useModel(ModelType->TRANSCRIPTION, object{
            object::pair{std::string("audio"), wavBuffer}, 
            object::pair{std::string("language"), std::string("en")}, 
            object::pair{std::string("stream"), true}
        }); });
        return as<string>(result);
    }
    catch (const any& error)
    {
        logger->error(std::string("[StreamingAudio] Transcription failed:"), error);
        return nullptr;
    }
}

std::shared_ptr<Buffer> StreamingAudioCaptureService::rawToWav(std::shared_ptr<Buffer> rawData)
{
    auto sampleRate = this->config->sampleRate;
    auto channels = this->config->channels;
    auto bitsPerSample = 16;
    auto byteRate = sampleRate * channels * (bitsPerSample / 8);
    auto blockAlign = channels * (bitsPerSample / 8);
    auto dataSize = rawData->length;
    auto fileSize = 36 + dataSize;
    auto header = Buffer::alloc(44);
    header->write(std::string("RIFF"), 0);
    header->writeUInt32LE(fileSize, 4);
    header->write(std::string("WAVE"), 8);
    header->write(std::string("fmt "), 12);
    header->writeUInt32LE(16, 16);
    header->writeUInt16LE(1, 20);
    header->writeUInt16LE(channels, 22);
    header->writeUInt32LE(sampleRate, 24);
    header->writeUInt32LE(byteRate, 28);
    header->writeUInt16LE(blockAlign, 32);
    header->writeUInt16LE(bitsPerSample, 34);
    header->write(std::string("data"), 36);
    header->writeUInt32LE(dataSize, 40);
    return Buffer::concat(array<std::shared_ptr<Buffer>>{ header, rawData });
}

std::shared_ptr<Promise<void>> StreamingAudioCaptureService::generateResponse(string transcription)
{
    this->responseTimer = nullptr;
    try
    {
        std::async([=]() { this->createAudioMemory(transcription); });
        this->emit(std::string("utteranceComplete"), transcription);
    }
    catch (const any& error)
    {
        logger->error(std::string("[StreamingAudio] Response generation error:"), error);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> StreamingAudioCaptureService::createAudioMemory(string transcription)
{
    try
    {
        auto _memory = object{
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("[Audio] ") + transcription + string_empty}, 
                object::pair{std::string("type"), std::string("audio_transcription")}, 
                object::pair{std::string("source"), std::string("microphone_streaming")}, 
                object::pair{std::string("timestamp"), Date->now()}
            }}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("isAudioTranscription"), true}, 
                object::pair{std::string("streaming"), true}
            }}
        };
        logger->info(std::string("[StreamingAudio] Audio transcription stored in context"));
    }
    catch (const any& error)
    {
        logger->error(std::string("[StreamingAudio] Failed to create audio memory:"), error);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> StreamingAudioCaptureService::stop()
{
    logger->info(std::string("[StreamingAudio] Stopping audio capture..."));
    if (this->captureProcess) {
        this->captureProcess->kill();
        this->captureProcess = nullptr;
    }
    if (this->silenceTimer) {
        clearTimeout(this->silenceTimer);
        this->silenceTimer = nullptr;
    }
    if (this->responseTimer) {
        clearTimeout(this->responseTimer);
        this->responseTimer = nullptr;
    }
    this->isCapturing = false;
    this->isSpeaking = false;
    this->audioBuffer = array<any>();
    logger->info(std::string("[StreamingAudio] Audio capture stopped"));
    return std::shared_ptr<Promise<void>>();
}

boolean StreamingAudioCaptureService::isActive()
{
    return this->isCapturing;
}

string StreamingAudioCaptureService::getCurrentTranscription()
{
    return this->currentTranscription;
}

boolean StreamingAudioCaptureService::isSpeechActive()
{
    return this->isSpeaking;
}

StreamingAudioCaptureService::StreamingAudioCaptureService(std::shared_ptr<EventEmitterOptions> options) : EventEmitter(options) {
}

