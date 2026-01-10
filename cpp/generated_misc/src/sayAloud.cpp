#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sam/src/actions/sayAloud.h"

string extractTextToSpeak(string messageText)
{
    shared text = messageText->toLowerCase()->trim();
    auto quotedPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("say ["']([^"']+)["'"))), (new RegExp(std::string("speak ["']([^"']+)["'"))), (new RegExp(std::string("read ["']([^"']+)["'"))), (new RegExp(std::string("announce ["']([^"']+)["'"))), (new RegExp(std::string("["']([^"']+)["'"))) };
    for (auto& pattern : quotedPatterns)
    {
        auto match = text->match(pattern);
        if (match) {
            return (*const_(match))[1];
        }
    }
    auto afterKeywordPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("(?:say|speak|read)\s+(?:aloud\s+)?(?:this\s+)?:?\s*(.+)"))), (new RegExp(std::string("(?:can you|please)\s+(?:say|speak|read)\s+(?:aloud\s+)?(.+)"))), (new RegExp(std::string("(?:i want to hear|let me hear)\s+(.+)"))), (new RegExp(std::string("(?:read this|say this|speak this)\s*:?\s*(.+)"))) };
    for (auto& pattern : afterKeywordPatterns)
    {
        auto match = text->match(pattern);
        if (match) {
            auto extractedText = (*const_(match))[1]->trim();
            extractedText = extractedText->replace((new RegExp(std::string("\s+out loud"))), string_empty)->replace((new RegExp(std::string("\s+aloud"))), string_empty)->replace((new RegExp(std::string("\s+please"))), string_empty)->trim();
            return extractedText;
        }
    }
    auto speechKeywords = array<string>{ std::string("say aloud"), std::string("speak"), std::string("use your voice"), std::string("talk to me") };
    if (speechKeywords->some([=](auto keyword) mutable
    {
        return text->includes(keyword);
    }
    )) {
        return std::string("Hello! I am speaking using my SAM voice synthesizer.");
    }
    return string_empty;
};


std::shared_ptr<SamTTSOptions> extractVoiceOptions(string messageText)
{
    auto text = messageText->toLowerCase();
    auto options = object{};
    if (OR((OR((text->includes(std::string("higher voice"))), (text->includes(std::string("high pitch"))))), (text->includes(std::string("squeaky"))))) {
        options->pitch = 100;
    } else if (OR((OR((text->includes(std::string("lower voice"))), (text->includes(std::string("low pitch"))))), (text->includes(std::string("deep voice"))))) {
        options->pitch = 30;
    }
    if (OR((OR((text->includes(std::string("faster"))), (text->includes(std::string("quickly"))))), (text->includes(std::string("speed up"))))) {
        options->speed = 120;
    } else if (OR((OR((text->includes(std::string("slower"))), (text->includes(std::string("slowly"))))), (text->includes(std::string("slow down"))))) {
        options->speed = 40;
    }
    if (OR((text->includes(std::string("robotic"))), (text->includes(std::string("robot voice"))))) {
        options->throat = 200;
        options->mouth = 50;
    } else if (OR((text->includes(std::string("smooth"))), (text->includes(std::string("natural"))))) {
        options->throat = 100;
        options->mouth = 150;
    }
    return options;
};


std::shared_ptr<Action> sayAloudAction = object{
    object::pair{std::string("name"), std::string("SAY_ALOUD")}, 
    object::pair{std::string("description"), std::string("Make the agent speak text aloud using SAM retro speech synthesizer")}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Can you say hello out loud?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll say hello using my SAM voice.")}, 
            object::pair{std::string("action"), std::string("SAY_ALOUD")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Please read this message aloud: Welcome to ElizaOS")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll read that message aloud for you now.")}, 
            object::pair{std::string("action"), std::string("SAY_ALOUD")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Speak in a higher voice")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll adjust my voice settings and speak in a higher pitch.")}, 
            object::pair{std::string("action"), std::string("SAY_ALOUD")}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        shared text = message->content->text->toLowerCase();
        auto speechTriggers = array<string>{ std::string("say aloud"), std::string("speak"), std::string("read aloud"), std::string("say out loud"), std::string("voice"), std::string("speak this"), std::string("say this"), std::string("read this"), std::string("announce"), std::string("proclaim"), std::string("tell everyone"), std::string("speak up"), std::string("use your voice"), std::string("talk to me"), std::string("higher voice"), std::string("lower voice"), std::string("change voice"), std::string("robotic voice"), std::string("retro voice") };
        auto hasSpeechTrigger = speechTriggers->some([=](auto trigger) mutable
        {
            return text->includes(trigger);
        }
        );
        auto hasVocalizationIntent = OR((OR((OR((OR((OR((text->includes(std::string("can you say"))), (text->includes(std::string("please say"))))), (text->includes(std::string("i want to hear"))))), (text->includes(std::string("let me hear"))))), (text->match((new RegExp(std::string("say ["'].*["'")))) != nullptr))), (text->match((new RegExp(std::string("speak ["'].*["'")))) != nullptr));
        return OR((hasSpeechTrigger), (hasVocalizationIntent));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        logger->info(std::string("[SAY_ALOUD] Processing speech request..."));
        auto samService = as<any>((as<object>(runtime))["getService"](std::string("SAM_TTS")));
        if (!samService) {
            logger->warn(std::string("[SAY_ALOUD] SAM TTS service not available"));
            callback(object{
                object::pair{std::string("text"), std::string("Sorry, I cannot speak aloud right now. The text-to-speech service is not available.")}, 
                object::pair{std::string("action"), std::string("SAY_ALOUD")}, 
                object::pair{std::string("error"), std::string("SAM TTS service not available")}
            });
            return std::shared_ptr<Promise<void>>();
        }
        auto textToSpeak = extractTextToSpeak(message->content->text);
        auto voiceOptions = extractVoiceOptions(message->content->text);
        logger->info(std::string("[SAY_ALOUD] Speaking: "") + textToSpeak + std::string("""));
        logger->info(std::string("[SAY_ALOUD] Voice options:"), voiceOptions);
        auto audioBuffer = std::async([=]() { samService->speakText(textToSpeak, voiceOptions); });
        logger->info(std::string("[SAY_ALOUD] ✅ Speech synthesis completed successfully"));
        callback(object{
            object::pair{std::string("text"), std::string("I spoke aloud using my SAM voice: "") + textToSpeak + std::string(""")}, 
            object::pair{std::string("action"), std::string("SAY_ALOUD")}, 
            object::pair{std::string("audioData"), Array->from(audioBuffer)}
        });
    }
    }
};

void Main(void)
{
}

MAIN
