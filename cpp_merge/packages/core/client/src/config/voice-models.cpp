#include "voice-models.h"

Record<string, string> providerPluginMap = object{
    object::pair{std:("elevenlabs"), std:("@elizaos/plugin-elevenlabs")}, 
    object::pair{std:("local"), std:("@elizaos/plugin-local-ai")}, 
    object::pair{std:("openai"), std:("@elizaos/plugin-openai")}, 
    object::pair{std:("none"), string_empty}
};
array<std::shared_ptr<VoiceModel>> noVoiceModel = array<std::shared_ptr<VoiceModel>>{ object{
    object::pair{std:("value"), std:("none")}, 
    object::pair{std:("label"), std:("No Voice")}, 
    object::pair{std:("provider"), std:("none")}
} };
array<std::shared_ptr<VoiceModel>> localVoiceModels = array<std::shared_ptr<VoiceModel>>{ object{
    object::pair{std:("value"), std:("female_1")}, 
    object::pair{std:("label"), std:("Local Voice - Female 1")}, 
    object::pair{std:("provider"), std:("local")}, 
    object::pair{std:("gender"), std:("female")}
}, object{
    object::pair{std:("value"), std:("female_2")}, 
    object::pair{std:("label"), std:("Local Voice - Female 2")}, 
    object::pair{std:("provider"), std:("local")}, 
    object::pair{std:("gender"), std:("female")}
}, object{
    object::pair{std:("value"), std:("male_1")}, 
    object::pair{std:("label"), std:("Local Voice - Male 1")}, 
    object::pair{std:("provider"), std:("local")}, 
    object::pair{std:("gender"), std:("male")}
}, object{
    object::pair{std:("value"), std:("male_2")}, 
    object::pair{std:("label"), std:("Local Voice - Male 2")}, 
    object::pair{std:("provider"), std:("local")}, 
    object::pair{std:("gender"), std:("male")}
} };
array<std::shared_ptr<VoiceModel>> elevenLabsVoiceModels = array<std::shared_ptr<VoiceModel>>{ object{
    object::pair{std:("value"), std:("EXAVITQu4vr4xnSDxMaL")}, 
    object::pair{std:("label"), std:("ElevenLabs - Rachel (Default)")}, 
    object::pair{std:("provider"), std:("elevenlabs")}, 
    object::pair{std:("gender"), std:("female")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("professional") }}
}, object{
    object::pair{std:("value"), std:("21m00Tcm4TlvDq8ikWAM")}, 
    object::pair{std:("label"), std:("ElevenLabs - Adam")}, 
    object::pair{std:("provider"), std:("elevenlabs")}, 
    object::pair{std:("gender"), std:("male")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("professional") }}
}, object{
    object::pair{std:("value"), std:("AZnzlk1XvdvUeBnXmlld")}, 
    object::pair{std:("label"), std:("ElevenLabs - Domi")}, 
    object::pair{std:("provider"), std:("elevenlabs")}, 
    object::pair{std:("gender"), std:("female")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("friendly") }}
}, object{
    object::pair{std:("value"), std:("MF3mGyEYCl7XYWbV9V6O")}, 
    object::pair{std:("label"), std:("ElevenLabs - Elli")}, 
    object::pair{std:("provider"), std:("elevenlabs")}, 
    object::pair{std:("gender"), std:("female")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("friendly") }}
}, object{
    object::pair{std:("value"), std:("TxGEqnHWrfWFTfGW9XjX")}, 
    object::pair{std:("label"), std:("ElevenLabs - Josh")}, 
    object::pair{std:("provider"), std:("elevenlabs")}, 
    object::pair{std:("gender"), std:("male")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("professional") }}
} };
array<std::shared_ptr<VoiceModel>> openAIVoiceModels = array<std::shared_ptr<VoiceModel>>{ object{
    object::pair{std:("value"), std:("alloy")}, 
    object::pair{std:("label"), std:("OpenAI - Alloy")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("female")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("versatile") }}
}, object{
    object::pair{std:("value"), std:("echo")}, 
    object::pair{std:("label"), std:("OpenAI - Echo")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("male")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("professional") }}
}, object{
    object::pair{std:("value"), std:("fable")}, 
    object::pair{std:("label"), std:("OpenAI - Fable")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("male")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("narrative") }}
}, object{
    object::pair{std:("value"), std:("onyx")}, 
    object::pair{std:("label"), std:("OpenAI - Onyx")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("male")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("deep") }}
}, object{
    object::pair{std:("value"), std:("nova")}, 
    object::pair{std:("label"), std:("OpenAI - Nova")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("female")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("friendly") }}
}, object{
    object::pair{std:("value"), std:("shimmer")}, 
    object::pair{std:("label"), std:("OpenAI - Shimmer")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("female")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("bright") }}
}, object{
    object::pair{std:("value"), std:("ash")}, 
    object::pair{std:("label"), std:("OpenAI - Ash")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("male")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("calm") }}
}, object{
    object::pair{std:("value"), std:("coral")}, 
    object::pair{std:("label"), std:("OpenAI - Coral")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("female")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("warm") }}
}, object{
    object::pair{std:("value"), std:("sage")}, 
    object::pair{std:("label"), std:("OpenAI - Sage")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("female")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("wise") }}
}, object{
    object::pair{std:("value"), std:("ballad")}, 
    object::pair{std:("label"), std:("OpenAI - Ballad")}, 
    object::pair{std:("provider"), std:("openai")}, 
    object::pair{std:("gender"), std:("male")}, 
    object::pair{std:("language"), std:("en")}, 
    object::pair{std:("features"), array<string>{ std:("natural"), std:("melodic") }}
} };
std::function<array<std::shared_ptr<VoiceModel>>()> getAllVoiceModels = [=]() mutable
{
    return array<std::shared_ptr<VoiceModel>>{ noVoiceModel, localVoiceModels, elevenLabsVoiceModels, openAIVoiceModels };
};
std::function<array<std::shared_ptr<VoiceModel>>(any)> getVoiceModelsByProvider = [=](P0 provider) mutable
{
    static switch_type __switch4153_4412 = {
        { any(std:("local")), 1 },
        { any(std:("elevenlabs")), 2 },
        { any(std:("openai")), 3 },
        { any(std:("none")), 4 }
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
