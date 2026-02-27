#include "action-audio.h"

std::shared_ptr<Action> transcribeAudioAction = object{
    object::pair{std:("name"), std:("TRANSCRIBE_AUDIO")}, 
    object::pair{std:("similes"), array<string>{ std:("LISTEN"), std:("HEAR_AUDIO"), std:("CAPTURE_AUDIO"), std:("RECORD_AUDIO") }}, 
    object::pair{std:("description"), std:("Captures audio from the microphone and transcribes it to text. Useful for voice commands, dictation, or understanding spoken content.")}, 
    object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto microphoneEnabled = OR((runtime->getSetting(std:("ENABLE_MICROPHONE")) == std:("true")), (runtime->getSetting(std:("VISION_MICROPHONE_ENABLED")) == std:("true")));
        if (!microphoneEnabled) {
            logger->debug(std:("[transcribeAudioAction] Microphone disabled in settings."));
            return false;
        }
        auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std:("VISION"));
        return AND((!!visionService), (visionService->isActive()));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        try
        {
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std:("VISION"));
            if (!visionService) {
                logger->error(std:("[transcribeAudioAction] VisionService not found"));
                return object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("Vision service not available")}
                };
            }
            auto audioCapture = (as<any>(visionService))["audioCapture"];
            auto streamingAudioCapture = (as<any>(visionService))["streamingAudioCapture"];
            if (AND((!audioCapture), (!streamingAudioCapture))) {
                logger->error(std:("[transcribeAudioAction] No audio capture service available"));
                return object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("Audio capture not initialized. Please enable microphone in settings.")}
                };
            }
            logger->info(std:("[transcribeAudioAction] Starting audio transcription..."));
            if (audioCapture) {
                auto transcription = std::async([=]() { audioCapture["recordAndTranscribe"](); });
                if (transcription) {
                    logger->info(std:("[transcribeAudioAction] Transcribed: "") + transcription + std:("""));
                    if (callback) {
                        std::async([=]() { callback(object{
                            object::pair{std:("text"), std:("I heard you say: "") + transcription + std:(""")}, 
                            object::pair{std:("source"), std:("transcribeAudioAction")}
                        }); });
                    }
                    return object{
                        object::pair{std:("success"), true}, 
                        object::pair{std:("data"), object{
                            object::pair{std:("transcription"), std:("transcription")}, 
                            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
                        }}
                    };
                } else {
                    return object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("error"), std:("No speech detected or transcription failed")}
                    };
                }
            }
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Audio capture service not properly initialized")}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("[transcribeAudioAction] Error:"), error);
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error occurred"))}
            };
        }
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("elizaos")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Listen to what I have to say")}, 
            object::pair{std:("action"), std:("TRANSCRIBE_AUDIO")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("elizaos")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Can you hear me?")}, 
            object::pair{std:("action"), std:("LISTEN")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("elizaos")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Record audio and tell me what you hear")}, 
            object::pair{std:("action"), std:("CAPTURE_AUDIO")}
        }}
    } } })}
};
std::shared_ptr<Action> toggleStreamingAudioAction = object{
    object::pair{std:("name"), std:("TOGGLE_STREAMING_AUDIO")}, 
    object::pair{std:("similes"), array<string>{ std:("START_LISTENING"), std:("STOP_LISTENING"), std:("CONTINUOUS_LISTENING") }}, 
    object::pair{std:("description"), std:("Toggles continuous audio streaming with voice activity detection. When enabled, the agent will continuously listen and transcribe speech.")}, 
    object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto microphoneEnabled = OR((runtime->getSetting(std:("ENABLE_MICROPHONE")) == std:("true")), (runtime->getSetting(std:("VISION_MICROPHONE_ENABLED")) == std:("true")));
        auto streamingEnabled = runtime->getSetting(std:("USE_STREAMING_AUDIO")) == std:("true");
        return AND((microphoneEnabled), (streamingEnabled));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        try
        {
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std:("VISION"));
            if (!visionService) {
                return object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("Vision service not available")}
                };
            }
            auto streamingAudioCapture = (as<any>(visionService))["streamingAudioCapture"];
            if (!streamingAudioCapture) {
                return object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("error"), std:("Streaming audio not initialized. Enable USE_STREAMING_AUDIO in settings.")}
                };
            }
            auto isCapturing = streamingAudioCapture["isCapturing"];
            if (isCapturing) {
                std::async([=]() { streamingAudioCapture["stop"](); });
                logger->info(std:("[toggleStreamingAudioAction] Stopped streaming audio capture"));
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I have stopped listening continuously.")}, 
                        object::pair{std:("source"), std:("toggleStreamingAudioAction")}
                    }); });
                }
                return object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("streaming"), false}, 
                        object::pair{std:("message"), std:("Streaming audio capture stopped")}
                    }}
                };
            } else {
                std::async([=]() { streamingAudioCapture["start"](); });
                logger->info(std:("[toggleStreamingAudioAction] Started streaming audio capture"));
                if (callback) {
                    std::async([=]() { callback(object{
                        object::pair{std:("text"), std:("I am now listening continuously. I will transcribe everything I hear.")}, 
                        object::pair{std:("source"), std:("toggleStreamingAudioAction")}
                    }); });
                }
                return object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("streaming"), true}, 
                        object::pair{std:("message"), std:("Streaming audio capture started")}
                    }}
                };
            }
        }
        catch (const any& error)
        {
            logger->error(std:("[toggleStreamingAudioAction] Error:"), error);
            return object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error occurred"))}
            };
        }
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("elizaos")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Start listening continuously")}, 
            object::pair{std:("action"), std:("START_LISTENING")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("elizaos")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Stop continuous listening")}, 
            object::pair{std:("action"), std:("STOP_LISTENING")}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
