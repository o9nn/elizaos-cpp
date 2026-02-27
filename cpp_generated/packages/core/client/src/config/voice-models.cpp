#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/config/voice-models.h"

Record<string, string> providerPluginMap = object{
    object::pair{std::string("elevenlabs"), std::string("@elizaos/plugin-elevenlabs")}, 
    object::pair{std::string("local"), std::string("@elizaos/plugin-local-ai")}, 
    object::pair{std::string("openai"), std::string("@elizaos/plugin-openai")}, 
    object::pair{std::string("none"), string_empty}
};
array<std::shared_ptr<VoiceModel>> noVoiceModel = array<std::shared_ptr<VoiceModel>>{ object{
    object::pair{std::string("value"), std::string("none")}, 
    object::pair{std::string("label"), std::string("No Voice")}, 
    object::pair{std::string("provider"), std::string("none")}
} };
array<std::shared_ptr<VoiceModel>> localVoiceModels = array<std::shared_ptr<VoiceModel>>{ object{
    object::pair{std::string("value"), std::string("female_1")}, 
    object::pair{std::string("label"), std::string("Local Voice - Female 1")}, 
    object::pair{std::string("provider"), std::string("local")}, 
    object::pair{std::string("gender"), std::string("female")}
}, object{
    object::pair{std::string("value"), std::string("female_2")}, 
    object::pair{std::string("label"), std::string("Local Voice - Female 2")}, 
    object::pair{std::string("provider"), std::string("local")}, 
    object::pair{std::string("gender"), std::string("female")}
}, object{
    object::pair{std::string("value"), std::string("male_1")}, 
    object::pair{std::string("label"), std::string("Local Voice - Male 1")}, 
    object::pair{std::string("provider"), std::string("local")}, 
    object::pair{std::string("gender"), std::string("male")}
}, object{
    object::pair{std::string("value"), std::string("male_2")}, 
    object::pair{std::string("label"), std::string("Local Voice - Male 2")}, 
    object::pair{std::string("provider"), std::string("local")}, 
    object::pair{std::string("gender"), std::string("male")}
} };
array<std::shared_ptr<VoiceModel>> elevenLabsVoiceModels = array<std::shared_ptr<VoiceModel>>{ object{
    object::pair{std::string("value"), std::string("EXAVITQu4vr4xnSDxMaL")}, 
    object::pair{std::string("label"), std::string("ElevenLabs - Rachel (Default)")}, 
    object::pair{std::string("provider"), std::string("elevenlabs")}, 
    object::pair{std::string("gender"), std::string("female")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("professional") }}
}, object{
    object::pair{std::string("value"), std::string("21m00Tcm4TlvDq8ikWAM")}, 
    object::pair{std::string("label"), std::string("ElevenLabs - Adam")}, 
    object::pair{std::string("provider"), std::string("elevenlabs")}, 
    object::pair{std::string("gender"), std::string("male")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("professional") }}
}, object{
    object::pair{std::string("value"), std::string("AZnzlk1XvdvUeBnXmlld")}, 
    object::pair{std::string("label"), std::string("ElevenLabs - Domi")}, 
    object::pair{std::string("provider"), std::string("elevenlabs")}, 
    object::pair{std::string("gender"), std::string("female")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("friendly") }}
}, object{
    object::pair{std::string("value"), std::string("MF3mGyEYCl7XYWbV9V6O")}, 
    object::pair{std::string("label"), std::string("ElevenLabs - Elli")}, 
    object::pair{std::string("provider"), std::string("elevenlabs")}, 
    object::pair{std::string("gender"), std::string("female")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("friendly") }}
}, object{
    object::pair{std::string("value"), std::string("TxGEqnHWrfWFTfGW9XjX")}, 
    object::pair{std::string("label"), std::string("ElevenLabs - Josh")}, 
    object::pair{std::string("provider"), std::string("elevenlabs")}, 
    object::pair{std::string("gender"), std::string("male")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("professional") }}
} };
array<std::shared_ptr<VoiceModel>> openAIVoiceModels = array<std::shared_ptr<VoiceModel>>{ object{
    object::pair{std::string("value"), std::string("alloy")}, 
    object::pair{std::string("label"), std::string("OpenAI - Alloy")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("female")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("versatile") }}
}, object{
    object::pair{std::string("value"), std::string("echo")}, 
    object::pair{std::string("label"), std::string("OpenAI - Echo")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("male")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("professional") }}
}, object{
    object::pair{std::string("value"), std::string("fable")}, 
    object::pair{std::string("label"), std::string("OpenAI - Fable")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("male")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("narrative") }}
}, object{
    object::pair{std::string("value"), std::string("onyx")}, 
    object::pair{std::string("label"), std::string("OpenAI - Onyx")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("male")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("deep") }}
}, object{
    object::pair{std::string("value"), std::string("nova")}, 
    object::pair{std::string("label"), std::string("OpenAI - Nova")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("female")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("friendly") }}
}, object{
    object::pair{std::string("value"), std::string("shimmer")}, 
    object::pair{std::string("label"), std::string("OpenAI - Shimmer")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("female")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("bright") }}
}, object{
    object::pair{std::string("value"), std::string("ash")}, 
    object::pair{std::string("label"), std::string("OpenAI - Ash")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("male")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("calm") }}
}, object{
    object::pair{std::string("value"), std::string("coral")}, 
    object::pair{std::string("label"), std::string("OpenAI - Coral")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("female")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("warm") }}
}, object{
    object::pair{std::string("value"), std::string("sage")}, 
    object::pair{std::string("label"), std::string("OpenAI - Sage")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("female")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("wise") }}
}, object{
    object::pair{std::string("value"), std::string("ballad")}, 
    object::pair{std::string("label"), std::string("OpenAI - Ballad")}, 
    object::pair{std::string("provider"), std::string("openai")}, 
    object::pair{std::string("gender"), std::string("male")}, 
    object::pair{std::string("language"), std::string("en")}, 
    object::pair{std::string("features"), array<string>{ std::string("natural"), std::string("melodic") }}
} };
std::function<array<std::shared_ptr<VoiceModel>>()> getAllVoiceModels = [=]() mutable
{
    return array<std::shared_ptr<VoiceModel>>{ noVoiceModel, localVoiceModels, elevenLabsVoiceModels, openAIVoiceModels };
};
std::function<array<std::shared_ptr<VoiceModel>>(any)> getVoiceModelsByProvider = [=](P0 provider) mutable
{
    static switch_type __switch4153_4412 = {
        { any(std::string("local")), 1 },
        { any(std::string("elevenlabs")), 2 },
        { any(std::string("openai")), 3 },
        { any(std::string("none")), 4 }
    };
    switch (__switch4153_4412[provider])
    {
    case 1:
        return localVoiceModels;
    case 2:
        return elevenLabsVoiceModels;
    case 3:
        return openAIVoiceModels;
    case 4:
        return noVoiceModel;
    default:
        return array<any>();
    }
};
std::function<std::shared_ptr<VoiceModel>(string)> getVoiceModelByValue = [=](auto value) mutable
{
    return getAllVoiceModels()->find([=](auto model) mutable
    {
        return model->value == value;
    }
    );
};
std::function<string(string)> getRequiredPluginForProvider = [=](auto provider) mutable
{
    return const_(providerPluginMap)[provider];
};
std::function<array<string>()> getAllRequiredPlugins = [=]() mutable
{
    return Object->values(providerPluginMap)->filter(Boolean);
};

void Main(void)
{
}

MAIN
