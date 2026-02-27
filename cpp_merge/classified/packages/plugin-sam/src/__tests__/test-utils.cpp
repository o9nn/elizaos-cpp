#include "test-utils.h"
#include <string>

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides)
{
    return as<std::shared_ptr<IAgentRuntime>>(utils::assign(object{
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001"))}, 
        object::pair{std::string("getSetting"), mock([=](auto key) mutable
        {
            auto settings = object{
                object::pair{std::string("SAM_SPEED"), 72}, 
                object::pair{std::string("SAM_PITCH"), 64}, 
                object::pair{std::string("SAM_THROAT"), 128}, 
                object::pair{std::string("SAM_MOUTH"), 128}
            };
            return OR((const_(settings)[key]), (nullptr));
        }
        )}, 
        object::pair{std::string("getService"), mock([=]() mutable
        {
            return nullptr;
        }
        )}, 
        object::pair{std::string("hasService"), mock([=]() mutable
        {
            return false;
        }
        )}, 
        object::pair{std::string("registerService"), mock()}, 
        object::pair{std::string("initialize"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("stop"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("evaluate"), mock([=]() mutable
        {
            return Promise->resolve(nullptr);
        }
        )}, 
        object::pair{std::string("processActions"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("useModel"), as<any>(mock([=]() mutable
        {
            return Promise->resolve(std::string("test response"));
        }
        ))}, 
        object::pair{std::string("ensureConnection"), mock([=]() mutable
        {
            return Promise->resolve();
        }
        )}, 
        object::pair{std::string("composeState"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<State>>(object{
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), string_empty}
            }));
        }
        )}, 
        object::pair{std::string("createMemory"), mock([=]() mutable
        {
            return Promise->resolve(as<std::shared_ptr<UUID>>(std::string("test-memory-id")));
        }
        )}, 
        object::pair{std::string("actions"), array<any>()}, 
        object::pair{std::string("providers"), array<any>()}, 
        object::pair{std::string("evaluators"), array<any>()}, 
        object::pair{std::string("services"), std::make_shared<Map>()}, 
        object::pair{std::string("db"), nullptr}, 
        object::pair{std::string("plugins"), array<any>()}, 
        object::pair{std::string("routes"), array<any>()}, 
        object::pair{std::string("logger"), std::string("logger")}, 
        object::pair{std::string("character"), object{
            object::pair{std::string("name"), std::string("Test SAM Agent")}, 
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001"))}, 
            object::pair{std::string("username"), std::string("test-sam-agent")}, 
            object::pair{std::string("bio"), array<string>{ std::string("Retro voice synthesis agent") }}, 
            object::pair{std::string("settings"), object{}}, 
            object::pair{std::string("system"), std::string("You are a helpful assistant with retro voice synthesis.")}, 
            object::pair{std::string("plugins"), array<string>{ std::string("@elizaos/plugin-sam") }}
        }}
    }, overrides));
};


void setupLoggerSpies()
{
    spyOn(logger, std::string("info"));
    spyOn(logger, std::string("error"));
    spyOn(logger, std::string("warn"));
    spyOn(logger, std::string("debug"));
};


std::shared_ptr<Memory> createMockMemory(Partial<std::shared_ptr<Memory>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000002"))}, 
        object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000003"))}, 
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000001"))}, 
        object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000004"))}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("test message")}
        }}, 
        object::pair{std::string("createdAt"), Date->now()}
    }, overrides);
};


std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides)
{
    return utils::assign(object{
        object::pair{std::string("data"), object{}}, 
        object::pair{std::string("values"), object{}}, 
        object::pair{std::string("text"), string_empty}
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
    wavBuffer->set(((std::make_shared<TextEncoder>()))->encode(std::string("RIFF")), 0);
    view->setUint32(4, 36 + audioData->length, true);
    wavBuffer->set(((std::make_shared<TextEncoder>()))->encode(std::string("WAVE")), 8);
    wavBuffer->set(((std::make_shared<TextEncoder>()))->encode(std::string("fmt ")), 12);
    view->setUint32(16, 16, true);
    view->setUint16(20, 1, true);
    view->setUint16(22, 1, true);
    view->setUint32(24, sampleRate, true);
    view->setUint32(28, sampleRate, true);
    view->setUint16(32, 1, true);
    view->setUint16(34, 8, true);
    wavBuffer->set(((std::make_shared<TextEncoder>()))->encode(std::string("data")), 36);
    view->setUint32(40, audioData->length, true);
    wavBuffer->set(audioData, wavHeaderSize);
    return wavBuffer;
};


