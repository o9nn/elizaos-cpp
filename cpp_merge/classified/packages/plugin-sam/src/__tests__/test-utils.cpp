#include "test-utils.h"

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    return as<std::shared_ptr<IAgentRuntime>>(utils::assign(object{
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001"))}, 
        object::pair{std:("getSetting"), mock([=](auto key) mutable
        {
            auto settings = object{
                object::pair{std:("SAM_SPEED"), 72}, 
                object::pair{std:("SAM_PITCH"), 64}, 
                object::pair{std:("SAM_THROAT"), 128}, 
                object::pair{std:("SAM_MOUTH"), 128}
            };
            return OR((const_(settings)[key]), (nullptr));
        }
        )}, 
        object::pair{std:("getService"), mock([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std:("hasService"), mock([=]() mutable
        {
            return false;
        }
        )}, 
        object::pair{std:("registerService"), mock()}, 
        object::pair{std:("initialize"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("stop"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("evaluate"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std:("processActions"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("useModel"), as<any>(mock([=]() mutable
        {
            return Promise->resolve(std:("test response"));
        }
        ))}, 
        object::pair{std:("ensureConnection"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std:("composeState"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<State>>(object{
                object::pair{std:("data"), object{}}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), string_empty}
            }));
        }
        )}, 
        object::pair{std:("createMemory"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std:("test-memory-id")));
        }
        )}, 
        object::pair{std:("actions"), array<any>()}, 
        object::pair{std:("providers"), array<any>()}, 
        object::pair{std:("evaluators"), array<any>()}, 
        object::pair{std:("services"), std::make_shared<Map>()}, 
        object::pair{std:("db"), nullptr}, 
        object::pair{std:("plugins"), array<any>()}, 
        object::pair{std:("routes"), array<any>()}, 
        object::pair{std:("logger"), std:("logger")}, 
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("Test SAM Agent")}, 
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001"))}, 
            object::pair{std:("username"), std:("test-sam-agent")}, 
            object::pair{std:("bio"), array<string>{ std:("Retro voice synthesis agent") }}, 
            object::pair{std:("settings"), object{}}, 
            object::pair{std:("system"), std:("You are a helpful assistant with retro voice synthesis.")}, 
            object::pair{std:("plugins"), array<string>{ std:("@elizaos/plugin-sam") }}
        }}
    }, overrides));
};


void setupLoggerSpies()
{
    spyOn(logger, std:("info"));
    spyOn(logger, std:("error"));
    spyOn(logger, std:("warn"));
    spyOn(logger, std:("debug"));
};


std::shared_ptr<Memory> createMockMemory(Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000002"))}, 
        object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000003"))}, 
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000001"))}, 
        object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000004"))}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("test message")}
        }}, 
        object::pair{std:("createdAt"), Date->now()}
    }, overrides);
};


std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        object::pair{std:("data"), object{}}, 
        object::pair{std:("values"), object{}}, 
        object::pair{std:("text"), string_empty}
    }, overrides);
};


std::shared_ptr<Uint8Array> createMockAudioBuffer(double length)
{
    auto buffer = std::make_shared<Uint8Array>(length);
    for (auto i = 0; i < length; i++)
    {
        (*buffer)[i] = Math->floor(Math->random() * 256);
    }
    return buffer;
};


std::shared_ptr<Uint8Array> createMockWAVBuffer(std::shared_ptr<Uint8Array> audioData, double sampleRate)
{
    auto wavHeaderSize = 44;
    auto wavBuffer = std::make_shared<Uint8Array>(wavHeaderSize + audioData->length);
    auto view = std::make_shared<DataView>(wavBuffer->buffer, 0, wavHeaderSize);
    wavBuffer->set(((std::make_shared<TextEncoder>()))->encode(std:("RIFF")), 0);
    view->setUint32(4, 36 + audioData->length, true);
    wavBuffer->set(((std::make_shared<TextEncoder>()))->encode(std:("WAVE")), 8);
    wavBuffer->set(((std::make_shared<TextEncoder>()))->encode(std:("fmt ")), 12);
    view->setUint32(16, 16, true);
    view->setUint16(20, 1, true);
    view->setUint16(22, 1, true);
    view->setUint32(24, sampleRate, true);
    view->setUint32(28, sampleRate, true);
    view->setUint16(32, 1, true);
    view->setUint16(34, 8, true);
    wavBuffer->set(((std::make_shared<TextEncoder>()))->encode(std:("data")), 36);
    view->setUint32(40, audioData->length, true);
    wavBuffer->set(audioData, wavHeaderSize);
    return wavBuffer;
};


