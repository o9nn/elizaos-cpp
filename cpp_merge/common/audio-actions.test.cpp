#include "audio-actions.test.h"

void Main(void)
{
    describe(std:("Audio Actions"), [=]() mutable
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
                object::pair{std:("recordAndTranscribe"), [=]() mutable
                {
                    recordAndTranscribeCalled = true;
                    return Promise->resolve(transcribeResult);
                }
                }
            };
            shared startCalled = false;
            shared stopCalled = false;
            mockStreamingAudioCapture = object{
                object::pair{std:("isCapturing"), false}, 
                object::pair{std:("start"), [=]() mutable
                {
                    startCalled = true;
                    mockStreamingAudioCapture["isCapturing"] = true;
                    return Promise->resolve();
                }
                }, 
                object::pair{std:("stop"), [=]() mutable
                {
                    stopCalled = true;
                    mockStreamingAudioCapture["isCapturing"] = false;
                    return Promise->resolve();
                }
                }, 
                , 
            };
            mockVisionService = object{
                object::pair{std:("isActive"), [=]() mutable
                {
                    return true;
                }
                }, 
                object::pair{std:("audioCapture"), mockAudioCapture}, 
                object::pair{std:("streamingAudioCapture"), mockStreamingAudioCapture}
            };
            mockSettings = object{
                object::pair{std:("ENABLE_MICROPHONE"), std:("true")}, 
                object::pair{std:("VISION_MICROPHONE_ENABLED"), std:("true")}, 
                object::pair{std:("USE_STREAMING_AUDIO"), std:("true")}
            };
            mockRuntime = as<any>(object{
                object::pair{std:("getSetting"), [=](auto key) mutable
                {
                    return OR((const_(mockSettings)[key]), (undefined));
                }
                }, 
                object::pair{std:("getService"), [=](auto name) mutable
                {
                    if (name == std:("VISION")) {
                        return mockVisionService;
                    }
                    return nullptr;
                }
                }
            });
        }
        );
        describe(std:("transcribeAudioAction"), [=]() mutable
        {
            it(std:("should validate when microphone is enabled"), [=]() mutable
            {
                auto isValid = std::async([=]() { transcribeAudioAction->validate(mockRuntime, as<std::shared_ptr<Memory>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std:("should not validate when microphone is disabled"), [=]() mutable
            {
                mockSettings[std:("ENABLE_MICROPHONE")] = std:("false");
                mockSettings[std:("VISION_MICROPHONE_ENABLED")] = std:("false");
                auto isValid = std::async([=]() { transcribeAudioAction->validate(mockRuntime, as<std::shared_ptr<Memory>>(object{})); });
                expect(isValid)->toBe(false);
            }
            );
            it(std:("should successfully transcribe audio"), [=]() mutable
            {
                auto testTranscription = std:("Hello, this is a test transcription");
                transcribeResult = testTranscription;
                auto result = std::async([=]() { transcribeAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(object{
                    object::pair{std:("transcription"), testTranscription}, 
                    object::pair{std:("timestamp"), expect->any(String)}
                });
                expect(recordAndTranscribeCalled)->toBe(true);
            }
            );
            it(std:("should handle transcription failure"), [=]() mutable
            {
                transcribeResult = nullptr;
                auto result = std::async([=]() { transcribeAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(false);
                expect(result->error)->toBe(std:("No speech detected or transcription failed"));
            }
            );
            it(std:("should handle missing audio capture service"), [=]() mutable
            {
                mockVisionService["audioCapture"] = nullptr;
                mockVisionService["streamingAudioCapture"] = nullptr;
                auto result = std::async([=]() { transcribeAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(false);
                expect(result->error)->toBe(std:("Audio capture not initialized. Please enable microphone in settings."));
            }
            );
        }
        );
        describe(std:("toggleStreamingAudioAction"), [=]() mutable
        {
            it(std:("should validate when microphone and streaming are enabled"), [=]() mutable
            {
                auto isValid = std::async([=]() { toggleStreamingAudioAction->validate(mockRuntime, as<std::shared_ptr<Memory>>(object{})); });
                expect(isValid)->toBe(true);
            }
            );
            it(std:("should not validate when streaming is disabled"), [=]() mutable
            {
                mockSettings[std:("USE_STREAMING_AUDIO")] = std:("false");
                auto isValid = std::async([=]() { toggleStreamingAudioAction->validate(mockRuntime, as<std::shared_ptr<Memory>>(object{})); });
                expect(isValid)->toBe(false);
            }
            );
            it(std:("should start streaming when not capturing"), [=]() mutable
            {
                mockStreamingAudioCapture["isCapturing"] = false;
                auto result = std::async([=]() { toggleStreamingAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(object{
                    object::pair{std:("streaming"), true}, 
                    object::pair{std:("message"), std:("Streaming audio capture started")}
                });
                expect(mockStreamingAudioCapture["startCalled"])->toBe(true);
            }
            );
            it(std:("should stop streaming when capturing"), [=]() mutable
            {
                mockStreamingAudioCapture["isCapturing"] = true;
                auto result = std::async([=]() { toggleStreamingAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(true);
                expect(result->data)->toEqual(object{
                    object::pair{std:("streaming"), false}, 
                    object::pair{std:("message"), std:("Streaming audio capture stopped")}
                });
                expect(mockStreamingAudioCapture["stopCalled"])->toBe(true);
            }
            );
            it(std:("should handle missing streaming audio service"), [=]() mutable
            {
                mockVisionService["streamingAudioCapture"] = nullptr;
                auto result = std::async([=]() { toggleStreamingAudioAction->handler(mockRuntime, as<std::shared_ptr<Memory>>(object{}), undefined, undefined, undefined); });
                expect(result)->toBeDefined();
                expect(result->success)->toBe(false);
                expect(result->error)->toBe(std:("Streaming audio not initialized. Enable USE_STREAMING_AUDIO in settings."));
            }
            );
        }
        );
    }
    );
}

MAIN
