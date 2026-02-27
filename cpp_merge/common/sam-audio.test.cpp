#include "sam-audio.test.h"

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
    describe(std:("SAM Audio Generation"), [=]() mutable
    {
        it(std:("should generate audio buffer from text"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            auto service = std::make_shared<SamTTSService>(runtime);
            auto audioBuffer = std::async([=]() { service->generateAudio(std:("Hello retro world"), object{
                object::pair{std:("speed"), 72}, 
                object::pair{std:("pitch"), 64}, 
                object::pair{std:("throat"), 128}, 
                object::pair{std:("mouth"), 128}
            }); });
            expect(audioBuffer)->toBeDefined();
            expect(audioBuffer)->toBeInstanceOf(Uint8Array);
            expect(audioBuffer->length)->toBeGreaterThan(0);
        }
        );
        it(std:("should handle different voice parameters"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            auto service = std::make_shared<SamTTSService>(runtime);
            auto testParameters = array<object>{ object{
                object::pair{std:("speed"), 50}, 
                object::pair{std:("pitch"), 40}, 
                object::pair{std:("throat"), 100}, 
                object::pair{std:("mouth"), 100}
            }, object{
                object::pair{std:("speed"), 72}, 
                object::pair{std:("pitch"), 64}, 
                object::pair{std:("throat"), 128}, 
                object::pair{std:("mouth"), 128}
            }, object{
                object::pair{std:("speed"), 100}, 
                object::pair{std:("pitch"), 80}, 
                object::pair{std:("throat"), 150}, 
                object::pair{std:("mouth"), 150}
            }, object{
                object::pair{std:("speed"), 150}, 
                object::pair{std:("pitch"), 120}, 
                object::pair{std:("throat"), 200}, 
                object::pair{std:("mouth"), 200}
            } };
            auto& __array1303_1673 = testParameters;
            for (auto __indx1303_1673 = 0_N; __indx1303_1673 < __array1303_1673->get_length(); __indx1303_1673++)
            {
                auto& params = const_(__array1303_1673)[__indx1303_1673];
                {
                    auto audioBuffer = std::async([=]() { service->generateAudio(std:("Test voice parameters"), params); });
                    expect(audioBuffer)->toBeDefined();
                    expect(audioBuffer->length)->toBeGreaterThan(0);
                }
            }
        }
        );
        it(std:("should create WAV buffer with proper header"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            auto service = std::make_shared<SamTTSService>(runtime);
            auto audioData = createMockAudioBuffer(1000);
            auto sampleRate = 22050;
            auto wavBuffer = service->createWAVBuffer(audioData, sampleRate);
            expect(wavBuffer)->toBeDefined();
            expect(wavBuffer)->toBeInstanceOf(Uint8Array);
            expect(wavBuffer->length)->toBeGreaterThan(audioData->length);
            auto headerView = std::make_shared<DataView>(wavBuffer->buffer, 0, 44);
            auto riffSignature = ((std::make_shared<TextDecoder>()))->decode(wavBuffer->subarray(0, 4));
            expect(riffSignature)->toBe(std:("RIFF"));
            auto waveSignature = ((std::make_shared<TextDecoder>()))->decode(wavBuffer->subarray(8, 12));
            expect(waveSignature)->toBe(std:("WAVE"));
            auto fmtSignature = ((std::make_shared<TextDecoder>()))->decode(wavBuffer->subarray(12, 16));
            expect(fmtSignature)->toBe(std:("fmt "));
            auto dataSignature = ((std::make_shared<TextDecoder>()))->decode(wavBuffer->subarray(36, 40));
            expect(dataSignature)->toBe(std:("data"));
            auto headerSampleRate = headerView->getUint32(24, true);
            expect(headerSampleRate)->toBe(sampleRate);
        }
        );
        it(std:("should handle empty text input"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            auto service = std::make_shared<SamTTSService>(runtime);
            auto audioBuffer = std::async([=]() { service->generateAudio(string_empty); });
            expect(audioBuffer)->toBeDefined();
            expect(audioBuffer)->toBeInstanceOf(Uint8Array);
        }
        );
        it(std:("should handle special characters and punctuation"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            auto service = std::make_shared<SamTTSService>(runtime);
            auto testTexts = array<string>{ std:("Hello, world!"), std:("What's happening?"), std:("System error: code 404"), std:("100% complete"), std:("User@domain.com"), std:("File path: /home/user/file.txt") };
            auto& __array3824_4110 = testTexts;
            for (auto __indx3824_4110 = 0_N; __indx3824_4110 < __array3824_4110->get_length(); __indx3824_4110++)
            {
                auto& text = const_(__array3824_4110)[__indx3824_4110];
                {
                    auto audioBuffer = std::async([=]() { service->generateAudio(text); });
                    expect(audioBuffer)->toBeDefined();
                    expect(audioBuffer->length)->toBeGreaterThanOrEqual(0);
                }
            }
        }
        );
        it(std:("should generate different audio for different texts"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            auto service = std::make_shared<SamTTSService>(runtime);
            auto text1 = std:("Short text");
            auto text2 = std:("This is a much longer piece of text that should generate more audio data");
            auto audio1 = std::async([=]() { service->generateAudio(text1); });
            auto audio2 = std::async([=]() { service->generateAudio(text2); });
            expect(audio1->length)->toBeLessThan(audio2->length);
        }
        );
        it(std:("should handle boundary voice parameter values"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            auto service = std::make_shared<SamTTSService>(runtime);
            auto boundaryTests = array<object>{ object{
                object::pair{std:("speed"), 20}, 
                object::pair{std:("pitch"), 0}, 
                object::pair{std:("throat"), 0}, 
                object::pair{std:("mouth"), 0}
            }, object{
                object::pair{std:("speed"), 200}, 
                object::pair{std:("pitch"), 255}, 
                object::pair{std:("throat"), 255}, 
                object::pair{std:("mouth"), 255}
            }, object{
                object::pair{std:("speed"), -1}, 
                object::pair{std:("pitch"), -1}, 
                object::pair{std:("throat"), -1}, 
                object::pair{std:("mouth"), -1}
            }, object{
                object::pair{std:("speed"), 300}, 
                object::pair{std:("pitch"), 300}, 
                object::pair{std:("throat"), 300}, 
                object::pair{std:("mouth"), 300}
            } };
            for (auto& params : boundaryTests)
            {
                auto audioBuffer = std::async([=]() { service->generateAudio(std:("Boundary test"), params); });
                expect(audioBuffer)->toBeDefined();
                expect(audioBuffer)->toBeInstanceOf(Uint8Array);
            }
        }
        );
    }
    );
    describe(std:("SAM Service Integration"), [=]() mutable
    {
        it(std:("should initialize service correctly"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            auto service = std::async([=]() { SamTTSService::start(as<object>(runtime)); });
            expect(service)->toBeDefined();
            expect(service)->toBeInstanceOf(SamTTSService);
            expect(service->get_capabilityDescription())->toContain(std:("SAM"));
            expect(service->get_capabilityDescription())->toContain(std:("text-to-speech"));
        }
        );
        it(std:("should have correct service type"), [=]() mutable
        {
            expect(SamTTSService::serviceType)->toBe(std:("SAM_TTS"));
        }
        );
        it(std:("should stop service cleanly"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            auto service = std::make_shared<SamTTSService>(runtime);
            std::async([=]() { service->stop(); });
        }
        );
        it(std:("should handle concurrent audio generation"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            shared service = std::make_shared<SamTTSService>(runtime);
            auto texts = array<string>{ std:("First audio generation"), std:("Second concurrent generation"), std:("Third simultaneous generation") };
            auto promises = texts->map([=](auto text) mutable
            {
                return service->generateAudio(text);
            }
            );
            auto results = std::async([=]() { Promise->all(promises); });
            results->forEach([=](auto result) mutable
            {
                expect(result)->toBeDefined();
                expect(result->length)->toBeGreaterThan(0);
            }
            );
            expect(const_(results)[0])->not->toEqual(const_(results)[1]);
            expect(const_(results)[1])->not->toEqual(const_(results)[2]);
        }
        );
        it(std:("should speak text and return audio buffer"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            shared sentAudioData = nullptr;
            shared mockHardwareBridge = object{
                object::pair{std:("sendAudioData"), [=](auto audioData) mutable
                {
                    sentAudioData = audioData;
                }
                }, 
                object::pair{std:("isConnected"), [=]() mutable
                {
                    return true;
                }
                }
            };
            runtime->getService = [=](auto serviceType) mutable
            {
                if (serviceType == std:("hardwareBridge")) {
                    return mockHardwareBridge;
                }
                return nullptr;
            };
            auto service = std::make_shared<SamTTSService>(runtime);
            auto audioBuffer = std::async([=]() { service->speakText(std:("Test speech output")); });
            expect(audioBuffer)->toBeDefined();
            expect(audioBuffer->length)->toBeGreaterThan(0);
            expect(sentAudioData)->toBeDefined();
            expect(sentAudioData)->not->toBeNull();
        }
        );
        it(std:("should handle hardware bridge unavailability"), [=]() mutable
        {
            auto runtime = createMockRuntime();
            runtime->getService = [=](auto _serviceType) mutable
            {
                return nullptr;
            };
            auto service = std::make_shared<SamTTSService>(runtime);
            auto audioBuffer = std::async([=]() { service->speakText(std:("Test without hardware bridge")); });
            expect(audioBuffer)->toBeDefined();
            expect(audioBuffer->length)->toBeGreaterThan(0);
        }
        );
    }
    );
}

MAIN
