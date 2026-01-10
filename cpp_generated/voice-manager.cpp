#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/managers/voice-manager.h"

VoiceManager::VoiceManager(std::shared_ptr<IAgentRuntime> runtime) {
    this->runtime = runtime;
}

void VoiceManager::start()
{
    auto service = this->getService();
    auto world = service->getWorld();
    world->livekit->on(std::string("audio"), [=](auto data) mutable
    {
        auto isLoudEnough = [=](auto pcmBuffer, auto threshold) mutable
        {
            auto sum = 0;
            auto sampleCount = Math->floor(pcmBuffer->length / 2);
            for (auto i = 0; i < pcmBuffer->length; i += 2)
            {
                auto sample = pcmBuffer->readInt16LE(i);
                sum += Math->abs(sample);
            }
            auto avgAmplitude = sum / sampleCount;
            return avgAmplitude > threshold;
        };

        auto playerId = data["participant"];
        if (!this->userStates->has(playerId)) {
            this->userStates->set(playerId, object{
                object::pair{std::string("buffers"), array<any>()}, 
                object::pair{std::string("totalLength"), 0}, 
                object::pair{std::string("lastActive"), Date->now()}, 
                object::pair{std::string("transcriptionText"), string_empty}
            });
        }
        auto pcmBuffer = data["buffer"];
        if (isLoudEnough(pcmBuffer)) {
            this->handleUserBuffer(playerId, pcmBuffer);
        }
    }
    );
}

void VoiceManager::handleUserBuffer(any playerId, any buffer)
{
    auto state = this->userStates->get(playerId);
    try
    {
        state["buffers"]->push(buffer);
        state["totalLength"] += buffer["length"];
        state["lastActive"] = Date->now();
        this->debouncedProcessTranscription(playerId);
    }
    catch (const any& error)
    {
        console->error(std::string("Error processing buffer for user ") + playerId + std::string(":"), error);
    }
}

void VoiceManager::debouncedProcessTranscription(std::shared_ptr<UUID> playerId)
{
    auto DEBOUNCE_TRANSCRIPTION_THRESHOLD = 1500;
    if (this->processingVoice) {
        auto state = this->userStates->get(playerId);
        state["buffers"]->get_length() = 0;
        state["totalLength"] = 0;
        return std::shared_ptr<Promise<void>>();
    }
    if (this->transcriptionTimeout) {
        clearTimeout(this->transcriptionTimeout);
    }
    this->transcriptionTimeout = as<std::shared_ptr<NodeJS::Timeout>>(as<any>(setTimeout([=]() mutable
    {
        std::async([=]() { agentActivityLock->run([=]() mutable
        {
            this->processingVoice = true;
            {
                utils::finally __finally3077_3129([&]() mutable
                {
                    this->processingVoice = false;
                });
                try
                {
                    std::async([=]() { this->processTranscription(playerId); });
                    this->userStates->forEach([=](auto state, auto _) mutable
                    {
                        state["buffers"]->get_length() = 0;
                        state["totalLength"] = 0;
                        state["transcriptionText"] = string_empty;
                    }
                    );
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        ); });
    }
    , DEBOUNCE_TRANSCRIPTION_THRESHOLD)));
}

void VoiceManager::processTranscription(std::shared_ptr<UUID> playerId)
{
    auto state = this->userStates->get(playerId);
    if (OR((!state), (state["buffers"]->get_length() == 0))) return std::shared_ptr<Promise<void>>();
    try
    {
        auto inputBuffer = Buffer::concat(state["buffers"], state["totalLength"]);
        state["buffers"]->get_length() = 0;
        state["totalLength"] = 0;
        auto wavHeader = getWavHeader(inputBuffer->length, 48000);
        auto wavBuffer = Buffer::concat(array<any>{ wavHeader, inputBuffer });
        logger->debug(std::string("Starting transcription..."));
        auto transcriptionText = std::async([=]() { this->runtime->useModel(ModelType->TRANSCRIPTION, wavBuffer); });
        console->log(std::string("[VOICE MANAGER] Transcrtion: "), transcriptionText);
        auto isValidTranscription = [=](auto text) mutable
        {
            if (OR((!text), (text->includes(std::string("[BLANK_AUDIO]"))))) return false;
            return true;
        };

        if (AND((transcriptionText), (isValidTranscription(transcriptionText)))) {
            state["transcriptionText"] += transcriptionText;
        }
        if (state["transcriptionText"]->get_length()) {
            auto finalText = state["transcriptionText"];
            state["transcriptionText"] = string_empty;
            std::async([=]() { this->handleMessage(finalText, playerId); });
        }
    }
    catch (const any& error)
    {
        console->error(std::string("Error transcribing audio for user ") + playerId + std::string(":"), error);
    }
}

any VoiceManager::handleMessage(string message, std::shared_ptr<UUID> playerId)
{
    try
    {
        if (OR((OR((!message), (message->trim() == string_empty))), (message->get_length() < 3))) {
            return object{
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("actions"), array<string>{ std::string("IGNORE") }}
            };
        }
        shared service = this->getService();
        auto world = service->getWorld();
        auto playerInfo = world->entities->getPlayer(playerId);
        auto userName = playerInfo->data->name;
        shared name = userName;
        auto _currentWorldId = service->currentWorldId;
        auto channelId = _currentWorldId;
        shared roomId = createUniqueUuid(this->runtime, OR((_currentWorldId), (std::string("hyperfy-unknown-world"))));
        shared entityId = as<std::shared_ptr<UUID>>(createUniqueUuid(this->runtime, playerId));
        shared type = ChannelType->WORLD;
        std::async([=]() { this->runtime->ensureConnection(object{
            object::pair{std::string("entityId"), std::string("entityId")}, 
            object::pair{std::string("roomId"), std::string("roomId")}, 
            object::pair{std::string("userName"), std::string("userName")}, 
            object::pair{std::string("name"), std::string("name")}, 
            object::pair{std::string("source"), std::string("hyperfy")}, 
            object::pair{std::string("channelId"), std::string("channelId")}, 
            object::pair{std::string("serverId"), std::string("hyperfy")}, 
            object::pair{std::string("type"), ChannelType->WORLD}, 
            object::pair{std::string("worldId"), _currentWorldId}, 
            object::pair{std::string("userId"), playerId}
        }); });
        shared memory = object{
            object::pair{std::string("id"), createUniqueUuid(this->runtime, string_empty + channelId + std::string("-voice-message-") + Date->now() + string_empty)}, 
            object::pair{std::string("agentId"), this->runtime->agentId}, 
            object::pair{std::string("entityId"), entityId}, 
            object::pair{std::string("roomId"), std::string("roomId")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), message}, 
                object::pair{std::string("source"), std::string("hyperfy")}, 
                object::pair{std::string("name"), name}, 
                object::pair{std::string("userName"), userName}, 
                object::pair{std::string("isVoiceMessage"), true}, 
                object::pair{std::string("channelType"), type}
            }}, 
            object::pair{std::string("createdAt"), Date->now()}
        };
        auto callback = [=](auto content, auto _files = array<any>()) mutable
        {
            console->info(std::string("[Hyperfy Voice Chat Callback] Received response: ") + JSON->stringify(content) + string_empty);
            try
            {
                auto responseMemory = object{
                    object::pair{std::string("id"), createUniqueUuid(this->runtime, string_empty + memory->id + std::string("-voice-response-") + Date->now() + string_empty)}, 
                    object::pair{std::string("entityId"), this->runtime->agentId}, 
                    object::pair{std::string("agentId"), this->runtime->agentId}, 
                    object::pair{std::string("content"), utils::assign(object{
                        , 
                        object::pair{std::string("name"), this->runtime->character->name}, 
                        object::pair{std::string("inReplyTo"), memory->id}, 
                        object::pair{std::string("isVoiceMessage"), true}, 
                        object::pair{std::string("channelType"), type}
                    }, content)}, 
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("createdAt"), Date->now()}
                };
                std::async([=]() { this->runtime->createMemory(responseMemory, std::string("messages")); });
                if (responseMemory->content->text->trim()) {
                    auto responseStream = std::async([=]() { this->runtime->useModel(ModelType->TEXT_TO_SPEECH, content->text); });
                    if (responseStream) {
                        auto audioBuffer = std::async([=]() { convertToAudioBuffer(responseStream); });
                        auto emoteManager = service->getEmoteManager();
                        auto emote = OR((as<string>(content->emote)), (std::string("TALK")));
                        emoteManager->playEmote(emote);
                        std::async([=]() { this->playAudio(audioBuffer); });
                    }
                }
                return array<any>{ responseMemory };
            }
            catch (const any& error)
            {
                console->error(std::string("Error in voice message callback:"), error);
                return array<any>();
            }
        };
        agentActivityLock->enter();
        this->runtime->emitEvent(array<hyperfyEventType>{ hyperfyEventType::VOICE_MESSAGE_RECEIVED }, object{
            object::pair{std::string("runtime"), this->runtime}, 
            object::pair{std::string("message"), memory}, 
            object::pair{std::string("callback"), std::string("callback")}, 
            object::pair{std::string("onComplete"), [=]() mutable
            {
                agentActivityLock->exit();
            }
            }
        });
    }
    catch (const any& error)
    {
        console->error(std::string("Error processing voice message:"), error);
    }
}

void VoiceManager::playAudio(any audioBuffer)
{
    if (this->processingVoice) {
        logger->info(std::string("[VOICE MANAER] Current voice is processing....."));
        return std::shared_ptr<Promise<void>>();
    }
    auto service = this->getService();
    auto world = service->getWorld();
    this->processingVoice = true;
    {
        utils::finally __finally8346_8390([&]() mutable
        {
            this->processingVoice = false;
        });
        try
        {
            std::async([=]() { world->livekit->publishAudioStream(audioBuffer); });
        }
        catch (const any& error)
        {
            logger->error(error);
        }
    }
}

any VoiceManager::getService()
{
    return this->runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
}

