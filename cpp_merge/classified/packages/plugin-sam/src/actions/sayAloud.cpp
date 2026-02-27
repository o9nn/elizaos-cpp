#include "sayAloud.hpp"

string extractTextToSpeak(string messageText)
{
    shared text = messageText->toLowerCase()->trim();
    auto quotedPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("say ["']([^"']+)["'"))), (new RegExp(std:("speak ["']([^"']+)["'"))), (new RegExp(std:("read ["']([^"']+)["'"))), (new RegExp(std:("announce ["']([^"']+)["'"))), (new RegExp(std:("["']([^"']+)["'"))) };
    for (auto& pattern : quotedPatterns)
    {
        auto match = text->match(pattern);
        if (match) {
            return (*const_(match))[1];
        }
    }
    auto afterKeywordPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("(?:say|speak|read)\s+(?:aloud\s+)?(?:this\s+)?:?\s*(.+)"))), (new RegExp(std:("(?:can you|please)\s+(?:say|speak|read)\s+(?:aloud\s+)?(.+)"))), (new RegExp(std:("(?:i want to hear|let me hear)\s+(.+)"))), (new RegExp(std:("(?:read this|say this|speak this)\s*:?\s*(.+)"))) };
    for (auto& pattern : afterKeywordPatterns)
    {
        auto match = text->match(pattern);
        if (match) {
            auto extractedText = (*const_(match))[1]->trim();
            extractedText = extractedText->replace((new RegExp(std:("\s+out loud"))), string_empty)->replace((new RegExp(std:("\s+aloud"))), string_empty)->replace((new RegExp(std:("\s+please"))), string_empty)->trim();
            return extractedText;
        }
    }
    auto speechKeywords = array<string>{ std:("say aloud"), std:("speak"), std:("use your voice"), std:("talk to me") };
    if (speechKeywords->some([=](auto keyword) mutable
    {
        return text->includes(keyword);
    }
    )) {
        return std:("Hello! I am speaking using my SAM voice synthesizer.");
    }
    return string_empty;
};


std::shared_ptr<SamTTSOptions> extractVoiceOptions(string messageText)
{
    auto text = messageText->toLowerCase();
    auto options = object{};
    if (OR((OR((text->includes(std:("higher voice"))), (text->includes(std:("high pitch"))))), (text->includes(std:("squeaky"))))) {
        options->pitch = 100;
    } else if (OR((OR((text->includes(std:("lower voice"))), (text->includes(std:("low pitch"))))), (text->includes(std:("deep voice"))))) {
        options->pitch = 30;
    }
    if (OR((OR((text->includes(std:("faster"))), (text->includes(std:("quickly"))))), (text->includes(std:("speed up"))))) {
        options->speed = 120;
    } else if (OR((OR((text->includes(std:("slower"))), (text->includes(std:("slowly"))))), (text->includes(std:("slow down"))))) {
        options->speed = 40;
    }
    if (OR((text->includes(std:("robotic"))), (text->includes(std:("robot voice"))))) {
        options->throat = 200;
        options->mouth = 50;
    } else if (OR((text->includes(std:("smooth"))), (text->includes(std:("natural"))))) {
        options->throat = 100;
        options->mouth = 150;
    }
    return options;
};


std::shared_ptr<Action> sayAloudAction = object{
    object::pair{std:("name"), std:("SAY_ALOUD")}, 
    object::pair{std:("description"), std:("Make the agent speak text aloud using SAM retro speech synthesizer")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Can you say hello out loud?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll say hello using my SAM voice.")}, 
            object::pair{std:("action"), std:("SAY_ALOUD")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Please read this message aloud: Welcome to ElizaOS")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll read that message aloud for you now.")}, 
            object::pair{std:("action"), std:("SAY_ALOUD")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{user1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Speak in a higher voice")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll adjust my voice settings and speak in a higher pitch.")}, 
            object::pair{std:("action"), std:("SAY_ALOUD")}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        shared text = message->content->text->toLowerCase();
        auto speechTriggers = array<string>{ std:("say aloud"), std:("speak"), std:("read aloud"), std:("say out loud"), std:("voice"), std:("speak this"), std:("say this"), std:("read this"), std:("announce"), std:("proclaim"), std:("tell everyone"), std:("speak up"), std:("use your voice"), std:("talk to me"), std:("higher voice"), std:("lower voice"), std:("change voice"), std:("robotic voice"), std:("retro voice") };
        auto hasSpeechTrigger = speechTriggers->some([=](auto trigger) mutable
        {
            return text->includes(trigger);
        }
        );
        auto hasVocalizationIntent = OR((OR((OR((OR((OR((text->includes(std:("can you say"))), (text->includes(std:("please say"))))), (text->includes(std:("i want to hear"))))), (text->includes(std:("let me hear"))))), (text->match((new RegExp(std:("say ["'].*["'")))) != nullptr))), (text->match((new RegExp(std:("speak ["'].*["'")))) != nullptr));
        return OR((hasSpeechTrigger), (hasVocalizationIntent));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        logger->info(std:("[SAY_ALOUD] Processing speech request..."));
        auto samService = as<any>((as<object>(runtime))["getService"](std:("SAM_TTS")));
        if (!samService) {
            logger->warn(std:("[SAY_ALOUD] SAM TTS service not available"));
            callback(object{
                object::pair{std:("text"), std:("Sorry, I cannot speak aloud right now. The text-to-speech service is not available.")}, 
                object::pair{std:("action"), std:("SAY_ALOUD")}, 
                object::pair{std:("error"), std:("SAM TTS service not available")}
            });
            return std::shared_ptr<Promise<void>>();
        }
        auto textToSpeak = extractTextToSpeak(message->content->text);
        auto voiceOptions = extractVoiceOptions(message->content->text);
        logger->info(std:("[SAY_ALOUD] Speaking: "") + textToSpeak + std:("""));
        logger->info(std:("[SAY_ALOUD] Voice options:"), voiceOptions);
        auto audioBuffer = std::async([=]() { samService->speakText(textToSpeak, voiceOptions); });
        logger->info(std:("[SAY_ALOUD] ✅ Speech synthesis completed successfully"));
        callback(object{
            object::pair{std:("text"), std:("I spoke aloud using my SAM voice: "") + textToSpeak + std:(""")}, 
            object::pair{std:("action"), std:("SAY_ALOUD")}, 
            object::pair{std:("audioData"), Array->from(audioBuffer)}
        });
    }
    }
};

void Main(void)
{
}

MAIN
