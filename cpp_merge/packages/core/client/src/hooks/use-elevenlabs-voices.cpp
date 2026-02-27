#include "use-elevenlabs-voices.h"

any useElevenLabsVoices()
{
    auto [apiKey, setApiKey] = useState<any>(nullptr);
    useEffect([=]() mutable
    {
        auto storedKey = localStorage->getItem(std:("ELEVENLABS_API_KEY"));
        setApiKey(storedKey);
    }
    , array<any>());
    return useQuery(object{
        object::pair{std:("queryKey"), array<string>{ std:("elevenlabs-voices"), apiKey }}, 
        object::pair{std:("queryFn"), [=]() mutable
        {
            if (!apiKey) {
                return elevenLabsVoiceModels;
            }
            try
            {
                auto response = std::async([=]() { fetch(std:("https://api.elevenlabs.io/v2/voices"), object{
                    object::pair{std:("method"), std:("GET")}, 
                    object::pair{std:("headers"), object{
                        object::pair{std:("xi-api-key"), apiKey}
                    }}
                }); });
                if (!response->ok) {
                    console->error(std:("Failed to fetch ElevenLabs voices:"), response->statusText);
                    return elevenLabsVoiceModels;
                }
                auto data = std::async([=]() { response->json(); });
                auto apiVoices = data["voices"]["map"]([=](auto voice) mutable
                {
                    return (object{
                        object::pair{std:("value"), voice->voice_id}, 
                        object::pair{std:("label"), std:("ElevenLabs - ") + voice->name + string_empty}, 
                        object::pair{std:("provider"), std:("elevenlabs")}, 
                        object::pair{std:("gender"), (voice->labels->gender == std:("female")) ? std:("female") : std:("male")}, 
                        object::pair{std:("language"), std:("en")}, 
                        object::pair{std:("features"), array<any>{ OR((voice->category), (std:("professional"))), OR((voice->labels->description), (std:("natural"))) }}
                    });
                }
                );
                return apiVoices;
            }
            catch (const any& error)
            {
                console->error(std:("Error fetching ElevenLabs voices:"), error);
                return elevenLabsVoiceModels;
            }
        }
        }, 
        object::pair{std:("staleTime"), 60 * 60 * 1000}, 
        object::pair{std:("refetchOnWindowFocus"), false}
    });
};


