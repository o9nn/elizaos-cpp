#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/action-audio.h"

std::shared_ptr<Action> transcribeAudioAction = object{
    object::pair{std::string("name"), std::string("TRANSCRIBE_AUDIO")}, 
    object::pair{std::string("similes"), array<string>{ std::string("LISTEN"), std::string("HEAR_AUDIO"), std::string("CAPTURE_AUDIO"), std::string("RECORD_AUDIO") }}, 
    object::pair{std::string("description"), std::string("Captures audio from the microphone and transcribes it to text. Useful for voice commands, dictation, or understanding spoken content.")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto microphoneEnabled = OR((runtime->getSetting(std::string("ENABLE_MICROPHONE")) == std::string("true")), (runtime->getSetting(std::string("VISION_MICROPHONE_ENABLED")) == std::string("true")));
        if (!microphoneEnabled) {
            logger->debug(std::string("[transcribeAudioAction] Microphone disabled in settings."));
            return false;
        }
        auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std::string("VISION"));
        return AND((!!visionService), (visionService->isActive()));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        try
        {
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std::string("VISION"));
            if (!visionService) {
                logger->error(std::string("[transcribeAudioAction] VisionService not found"));
                return object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("Vision service not available")}
                };
            }
            auto audioCapture = (as<any>(visionService))["audioCapture"];
            auto streamingAudioCapture = (as<any>(visionService))["streamingAudioCapture"];
            if (AND((!audioCapture), (!streamingAudioCapture))) {
                logger->error(std::string("[transcribeAudioAction] No audio capture service available"));
                return object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("Audio capture not initialized. Please enable microphone in settings.")}
                };
            }
            logger->info(std::string("[transcribeAudioAction] Starting audio transcription..."));
            if (audioCapture) {
                auto transcription = std::async([=]() { audioCapture["recordAndTranscribe"](); });
                if (transcription) {
                    logger->info(std::string("[transcribeAudioAction] Transcribed: "") + transcription + std::string("""));
                    if (callback) {
                        std::async([=]() { callback(object{
                            object::pair{std::string("text"), std::string("I heard you say: "") + transcription + std::string(""")}, 
                            object::pair{std::string("source"), std::string("transcribeAudioAction")}
                        }); });
                    }
                    return object{
                        object::pair{std::string("success"), true}, 
                        object::pair{std::string("data"), object{
                            object::pair{std::string("transcription"), std::string("transcription")}, 
                            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}
                        }}
                    };
                } else {
                    return object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("No speech detected or transcription failed")}
                    };
                }
            }
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("Audio capture service not properly initialized")}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("[transcribeAudioAction] Error:"), error);
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error occurred"))}
            };
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("elizaos")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Listen to what I have to say")}, 
            object::pair{std::string("action"), std::string("TRANSCRIBE_AUDIO")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("elizaos")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Can you hear me?")}, 
            object::pair{std::string("action"), std::string("LISTEN")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("elizaos")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Record audio and tell me what you hear")}, 
            object::pair{std::string("action"), std::string("CAPTURE_AUDIO")}
        }}
    } } })}
};
std::shared_ptr<Action> toggleStreamingAudioAction = object{
    object::pair{std::string("name"), std::string("TOGGLE_STREAMING_AUDIO")}, 
    object::pair{std::string("similes"), array<string>{ std::string("START_LISTENING"), std::string("STOP_LISTENING"), std::string("CONTINUOUS_LISTENING") }}, 
    object::pair{std::string("description"), std::string("Toggles continuous audio streaming with voice activity detection. When enabled, the agent will continuously listen and transcribe speech.")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto microphoneEnabled = OR((runtime->getSetting(std::string("ENABLE_MICROPHONE")) == std::string("true")), (runtime->getSetting(std::string("VISION_MICROPHONE_ENABLED")) == std::string("true")));
        auto streamingEnabled = runtime->getSetting(std::string("USE_STREAMING_AUDIO")) == std::string("true");
        return AND((microphoneEnabled), (streamingEnabled));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        try
        {
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std::string("VISION"));
            if (!visionService) {
                return object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("Vision service not available")}
                };
            }
            auto streamingAudioCapture = (as<any>(visionService))["streamingAudioCapture"];
            if (!streamingAudioCapture) {
                return object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), std::string("Streaming audio not initialized. Enable USE_STREAMING_AUDIO in settings.")}
                };
            }
            auto isCapturing = streamingAudioCapture["isCapturing"];
            if (isCapturing) {
                std::async([=]() { streamingAudioCapture["stop"](); });
                logger->info(std::string("[toggleStreamingAudioAction] Stopped streaming audio capture"));
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I have stopped listening continuously.")}, 
                        object::pair{std::string("source"), std::string("toggleStreamingAudioAction")}
                    }); });
                }
                return object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("streaming"), false}, 
                        object::pair{std::string("message"), std::string("Streaming audio capture stopped")}
                    }}
                };
            } else {
                std::async([=]() { streamingAudioCapture["start"](); });
                logger->info(std::string("[toggleStreamingAudioAction] Started streaming audio capture"));
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), std::string("I am now listening continuously. I will transcribe everything I hear.")}, 
                        object::pair{std::string("source"), std::string("toggleStreamingAudioAction")}
                    }); });
                }
                return object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("streaming"), true}, 
                        object::pair{std::string("message"), std::string("Streaming audio capture started")}
                    }}
                };
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("[toggleStreamingAudioAction] Error:"), error);
            return object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error occurred"))}
            };
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("elizaos")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Start listening continuously")}, 
            object::pair{std::string("action"), std::string("START_LISTENING")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("elizaos")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Stop continuous listening")}, 
            object::pair{std::string("action"), std::string("STOP_LISTENING")}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
