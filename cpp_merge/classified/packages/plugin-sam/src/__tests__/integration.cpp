#include "integration.test.h"

void Main(void)
{
    beforeAll([=]() mutable
    {
        setupLoggerSpies();
    }
    );
    afterAll([=]() mutable
    {
    }
    );
    describe(std:("Integration: SAY_ALOUD Action with SamTTSService"), [=]() mutable
    {
        shared<std::shared_ptr<MockRuntime>> mockRuntime;
        shared<object> mockHardwareBridge;
        beforeEach([=]() mutable
        {
            auto sendAudioDataSpy = [=]() mutable
            {
                return Promise->resolve();
            };
            mockHardwareBridge = object{
                object::pair{std:("sendAudioData"), sendAudioDataSpy}, 
                object::pair{std:("isConnected"), [=]() mutable
                {
                    return true;
                }
                }, 
                object::pair{std:("capabilityDescription"), std:("Mock hardware bridge for audio output")}
            };
            spyOn(mockHardwareBridge, std:("sendAudioData"));
            shared mockSamService = object{
                object::pair{std:("generateAudio"), [=](auto text, auto _options = undefined) mutable
                {
                    auto audioLength = text->get_length() * 100;
                    return ((std::make_shared<Uint8Array>(audioLength)))->fill(42);
                }
                }, 
                object::pair{std:("speakText"), [=](auto text, auto options = undefined) mutable
                {
                    auto audioBuffer = std::async([=]() { mockSamService["generateAudio"](text, options); });
                    std::async([=]() { mockHardwareBridge["sendAudioData"](audioBuffer); });
                    return audioBuffer;
                }
                }, 
                object::pair{std:("createWAVBuffer"), [=](auto audioData, auto _sampleRate = 22050) mutable
                {
                    auto wavHeader = std::make_shared<Uint8Array>(44);
                    auto wavData = std::make_shared<Uint8Array>(44 + audioData->length);
                    wavData->set(wavHeader, 0);
                    wavData->set(audioData, 44);
                    return wavData;
                }
                }, 
                object::pair{std:("stop"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }, 
                object::pair{std:("capabilityDescription"), std:("Mock SAM TTS service with audio generation")}
            };
            spyOn(mockSamService, std:("generateAudio"));
            spyOn(mockSamService, std:("speakText"));
            spyOn(mockSamService, std:("createWAVBuffer"));
            auto getServiceImpl = [=](auto serviceType) mutable
            {
                if (serviceType == std:("SAM_TTS")) {
                    return mockSamService;
                }
                if (serviceType == std:("hardwareBridge")) {
                    return mockHardwareBridge;
                }
                return nullptr;
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std:("getService"), getServiceImpl}
            });
            (as<any>(mockRuntime))["_mockSamService"] = mockSamService;
        }
        );
        it(std:("should handle SAY_ALOUD action with SamTTSService available"), [=]() mutable
        {
            auto sayAloudAction = samPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std:("SAY_ALOUD");
            }
            );
            expect(sayAloudAction)->toBeDefined();
            auto mockMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("say aloud: Hello from the retro future!")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto mockState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            shared callbackCalls = array<array<any>>();
            auto callbackFn = [=](Args... args_) mutable
            {
                array<any> args = array<any>{args_...};
                callbackCalls->push(args);
            };
            std::async([=]() { sayAloudAction->handler(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime)), mockMessage, mockState, object{}, as<std::shared_ptr<HandlerCallback>>(callbackFn), array<any>()); });
            expect(callbackCalls->get_length())->toBeGreaterThan(0);
            if (callbackCalls->get_length() > 0) {
                auto response = const_(const_(callbackCalls)[0])[0];
                expect(response->text)->toContain(std:("SAM voice"));
                expect(response->action)->toBe(std:("SAY_ALOUD"));
            }
            auto samService = (as<any>(mockRuntime))["_mockSamService"];
            expect(samService)->toBeDefined();
            expect(samService["speakText"])->toHaveBeenCalled();
            expect(mockHardwareBridge["sendAudioData"])->toHaveBeenCalled();
        }
        );
        it(std:("should extract text correctly from various message formats"), [=]() mutable
        {
            auto sayAloudAction = samPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std:("SAY_ALOUD");
            }
            );
            expect(sayAloudAction)->toBeDefined();
            auto testCases = array<object>{ object{
                object::pair{std:("input"), std:("say aloud: Welcome to 1985")}, 
                object::pair{std:("expectedText"), std:("aloud: welcome to 1985")}
            }, object{
                object::pair{std:("input"), std:("please speak this text: System online")}, 
                object::pair{std:("expectedText"), std:("text: system online")}
            }, object{
                object::pair{std:("input"), std:("speak up: Error detected")}, 
                object::pair{std:("expectedText"), std:("up: error detected")}
            }, object{
                object::pair{std:("input"), std:("can you say aloud the following: Hello world")}, 
                object::pair{std:("expectedText"), std:("the following: hello world")}
            } };
            auto samService = (as<any>(mockRuntime))["_mockSamService"];
            for (auto& testCase : testCases)
            {
                auto mockMessage = object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                    object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                    object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), testCase["input"]}, 
                        object::pair{std:("source"), std:("test")}
                    }}, 
                    object::pair{std:("createdAt"), Date->now()}
                };
                auto mockState = object{
                    object::pair{std:("values"), object{}}, 
                    object::pair{std:("data"), object{}}, 
                    object::pair{std:("text"), string_empty}
                };
                auto callbackFn = [=]() mutable
                {
                };
                samService["speakText"]["mockClear"]();
                std::async([=]() { sayAloudAction->handler(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime)), mockMessage, mockState, object{}, as<std::shared_ptr<HandlerCallback>>(callbackFn), array<any>()); });
                expect(samService["speakText"])->toHaveBeenCalledWith(testCase["expectedText"], expect->any(Object));
            }
        }
        );
        it(std:("should handle voice parameters from message content"), [=]() mutable
        {
            auto sayAloudAction = samPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std:("SAY_ALOUD");
            }
            );
            expect(sayAloudAction)->toBeDefined();
            auto mockMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("say aloud with robotic voice: Fast robot voice")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto mockState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto callbackFn = [=]() mutable
            {
            };
            auto samService = (as<any>(mockRuntime))["_mockSamService"];
            std::async([=]() { sayAloudAction->handler(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime)), mockMessage, mockState, object{}, as<std::shared_ptr<HandlerCallback>>(callbackFn), array<any>()); });
            expect(samService["speakText"])->toHaveBeenCalled();
            auto callArgs = const_(samService["speakText"]["mock"]["calls"])[0];
            expect(const_(callArgs)[1])->toMatchObject(object{
                object::pair{std:("throat"), 200}, 
                object::pair{std:("mouth"), 50}
            });
        }
        );
        it(std:("should handle service unavailability gracefully"), [=]() mutable
        {
            auto runtimeWithoutSam = createMockRuntime(object{
                object::pair{std:("getService"), [=](auto serviceType) mutable
                {
                    if (serviceType == std:("SAM_TTS")) {
                        return nullptr;
                    }
                    return nullptr;
                }
                }
            });
            auto sayAloudAction = samPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std:("SAY_ALOUD");
            }
            );
            expect(sayAloudAction)->toBeDefined();
            auto mockMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("say aloud: This should fail gracefully")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto mockState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            shared callbackCalls = array<array<any>>();
            auto callbackFn = [=](Args... args_) mutable
            {
                array<any> args = array<any>{args_...};
                callbackCalls->push(args);
            };
            std::async([=]() { sayAloudAction->handler(as<std::shared_ptr<IAgentRuntime>>(as<any>(runtimeWithoutSam)), mockMessage, mockState, object{}, as<std::shared_ptr<HandlerCallback>>(callbackFn), array<any>()); });
            expect(callbackCalls->get_length())->toBeGreaterThan(0);
            if (callbackCalls->get_length() > 0) {
                auto response = const_(const_(callbackCalls)[0])[0];
                expect(response->text)->toContain(std:("not available"));
            }
        }
        );
    }
    );
    describe(std:("Integration: Plugin initialization and service registration"), [=]() mutable
    {
        it(std:("should initialize the plugin and register the service"), [=]() mutable
        {
            auto mockRuntime = createMockRuntime();
            shared registerServiceCalls = array<object>();
            mockRuntime->registerService = [=](auto type, auto service) mutable
            {
                registerServiceCalls->push(object{
                    object::pair{std:("type"), std:("type")}, 
                    object::pair{std:("service"), std:("service")}
                });
            };
            if (samPlugin->init) {
                std::async([=]() { samPlugin->init(object{}, as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))); });
                if (samPlugin->services) {
                    auto SamTTSServiceClass = const_(samPlugin->services)[0];
                    auto _serviceInstance = std::async([=]() { SamTTSServiceClass->start(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))); });
                    mockRuntime->registerService(SamTTSServiceClass);
                }
                expect(registerServiceCalls->get_length())->toBeGreaterThan(0);
            }
        }
        );
        it(std:("should register all plugin components correctly"), [=]() mutable
        {
            auto mockRuntime = createMockRuntime();
            shared registeredActions = array<any>();
            shared registeredServices = array<any>();
            mockRuntime->registerAction = [=](auto action) mutable
            {
                registeredActions->push(action);
            };
            mockRuntime->registerService = [=](auto service) mutable
            {
                registeredServices->push(service);
            };
            if (samPlugin->init) {
                std::async([=]() { samPlugin->init(object{}, as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))); });
            }
            if (samPlugin->actions) {
                for (auto& action : samPlugin->actions)
                {
                    mockRuntime->registerAction(action);
                }
            }
            if (samPlugin->services) {
                for (auto& ServiceClass : samPlugin->services)
                {
                    auto serviceInstance = std::async([=]() { ServiceClass->start(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))); });
                    mockRuntime->registerService(serviceInstance);
                }
            }
            expect(registeredActions)->toHaveLength(1);
            expect(const_(registeredActions)[0]->name)->toBe(std:("SAY_ALOUD"));
            expect(registeredServices)->toHaveLength(1);
            expect(const_(registeredServices)[0]->constructor->name)->toBe(std:("SamTTSService"));
        }
        );
    }
    );
}

MAIN
