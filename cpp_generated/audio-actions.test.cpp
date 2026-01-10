#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/__tests__/audio-actions.test.h"

void Main(void)
{
    describe(std::string("Audio Actions"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<any> mockVisionService;
        shared<any> mockAudioCapture;
        shared<any> mockStreamingAudioCapture;
        shared<Record<string, string>> mockSettings;
        shared transcribeResult = nullptr;
        shared recordAndTranscribeCalled = false;
        beforeEach([=]() mutable
        {
            transcribeResult = nullptr;
            recordAndTranscribeCalled = false;
            mockAudioCapture = object{
                object::pair{std::string("recordAndTranscribe"), [=]() mutable
                {
                    recordAndTranscribeCalled = true;
                    return Promise->resolve(transcribeResult);
                }
                }
            };
            shared startCalled = false;
            shared stopCalled = false;
            mockStreamingAudioCapture = object{
                object::pair{std::string("isCapturing"), false}, 
                object::pair{std::string("start"), [=]() mutable
                {
                    startCalled = true;
                    mockStreamingAudioCapture["isCapturing"] = true;
                    return Promise->resolve();
                }
                }, 
                object::pair{std::string("stop"), [=]() mutable
                {
                    stopCalled = true;
                    mockStreamingAudioCapture["isCapturing"] = false;
                    return Promise->resolve();
                }
                }, 
                , 
            };
            mockVisionService = object{
                object::pair{std::string("isActive"), [=]() mutable
                {
                    return true;
                }
                }, 
                object::pair{std::string("audioCapture"), mockAudioCapture}, 
                object::pair{std::string("streamingAudioCapture"), mockStreamingAudioCapture}
            };
            mockSettings = object{
                object::pair{std::string("ENABLE_MICROPHONE"), std::string("true")}, 
                object::pair{std::string("VISION_MICROPHONE_ENABLED"), std::string("true")}, 
                object::pair{std::string("USE_STREAMING_AUDIO"), std::string("true")}
            };
            mockRuntime = as<any>(object{
                object::pair{std::string("getSetting"), [=](auto key) mutable
                {
                    return OR((const_(mockSettings)[key]), (undefined));
                }
                }, 
                object::pair{std::string("getService"), [=](auto name) mutable
                {
                    if (name == std::string("VISION")) {
                        return mockVisionService;
                    }
                    return nullptr;
                }
                }
            });
        }
        );
        describe(std::string("transcribeAudioAction"), [=]() mutable
        {
            it(std::string("should validate when microphone is enabled"), [=]() mutable
            {
                auto isValid = std::async([=]() { transcribeAudioAction->validate(mockRuntime, as<std::shared_ptr<Memory>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std::string("should not validate when microphone is disabled"), [=]() mutable
            {
                mockSettings[std::string("ENABLE_MICROPHONE")] = std::string("false");
                mockSettings[std::string("VISION_MICROPHONE_ENABLED")] = std::string("false");
                auto isValid = std::async([=]() { transcribeAudioAction->validate(mockRuntime, as<std::shared_ptr<Memory>>(object{})); });
                expect(isValid)->toBe(false);
            }
            );
            it(std::string("should successfully transcribe audio"), [=]() mutable
            {
                auto testTranscription = std::string("Hello, this is a test transcription");
                transcribeResult = testTranscription;
                auto result = std::async([=]() { transcribeAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(object{
                    object::pair{std::string("transcription"), testTranscription}, 
                    object::pair{std::string("timestamp"), expect->any(String)}
                });
                expect(recordAndTranscribeCalled)->toBe(true);
            }
            );
            it(std::string("should handle transcription failure"), [=]() mutable
            {
                transcribeResult = nullptr;
                auto result = std::async([=]() { transcribeAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(false);
                expect(result->error)->toBe(std::string("No speech detected or transcription failed"));
            }
            );
            it(std::string("should handle missing audio capture service"), [=]() mutable
            {
                mockVisionService["audioCapture"] = nullptr;
                mockVisionService["streamingAudioCapture"] = nullptr;
                auto result = std::async([=]() { transcribeAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(false);
                expect(result->error)->toBe(std::string("Audio capture not initialized. Please enable microphone in settings."));
            }
            );
        }
        );
        describe(std::string("toggleStreamingAudioAction"), [=]() mutable
        {
            it(std::string("should validate when microphone and streaming are enabled"), [=]() mutable
            {
                auto isValid = std::async([=]() { toggleStreamingAudioAction->validate(mockRuntime, as<std::shared_ptr<Memory>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std::string("should not validate when streaming is disabled"), [=]() mutable
            {
                mockSettings[std::string("USE_STREAMING_AUDIO")] = std::string("false");
                auto isValid = std::async([=]() { toggleStreamingAudioAction->validate(mockRuntime, as<std::shared_ptr<Memory>>(object{})); });
                expect(isValid)->toBe(false);
            }
            );
            it(std::string("should start streaming when not capturing"), [=]() mutable
            {
                mockStreamingAudioCapture["isCapturing"] = false;
                auto result = std::async([=]() { toggleStreamingAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(object{
                    object::pair{std::string("streaming"), true}, 
                    object::pair{std::string("message"), std::string("Streaming audio capture started")}
                });
                expect(mockStreamingAudioCapture["startCalled"])->toBe(true);
            }
            );
            it(std::string("should stop streaming when capturing"), [=]() mutable
            {
                mockStreamingAudioCapture["isCapturing"] = true;
                auto result = std::async([=]() { toggleStreamingAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(object{
                    object::pair{std::string("streaming"), false}, 
                    object::pair{std::string("message"), std::string("Streaming audio capture stopped")}
                });
                expect(mockStreamingAudioCapture["stopCalled"])->toBe(true);
            }
            );
            it(std::string("should handle missing streaming audio service"), [=]() mutable
            {
                mockVisionService["streamingAudioCapture"] = nullptr;
                auto result = std::async([=]() { toggleStreamingAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(false);
                expect(result->error)->toBe(std::string("Streaming audio not initialized. Enable USE_STREAMING_AUDIO in settings."));
            }
            );
        }
        );
    }
    );
}

MAIN
