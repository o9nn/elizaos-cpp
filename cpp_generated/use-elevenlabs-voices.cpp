#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/hooks/use-elevenlabs-voices.h"

any useElevenLabsVoices()
{
    auto [apiKey, setApiKey] = useState<any>(nullptr);
    useEffect([=]() mutable
    {
        auto storedKey = localStorage->getItem(std::string("ELEVENLABS_API_KEY"));
        setApiKey(storedKey);
    }
    , array<any>());
    return useQuery(object{
        object::pair{std::string("queryKey"), array<string>{ std::string("elevenlabs-voices"), apiKey }}, 
        object::pair{std::string("queryFn"), [=]() mutable
        {
            if (!apiKey) {
                return elevenLabsVoiceModels;
            }
            try
            {
                auto response = std::async([=]() { fetch(std::string("https://api.elevenlabs.io/v2/voices"), object{
                    object::pair{std::string("method"), std::string("GET")}, 
                    object::pair{std::string("headers"), object{
                        object::pair{std::string("xi-api-key"), apiKey}
                    }}
                }); });
                if (!response->ok) {
                    console->error(std::string("Failed to fetch ElevenLabs voices:"), response->statusText);
                    return elevenLabsVoiceModels;
                }
                auto data = std::async([=]() { response->json(); });
                auto apiVoices = data["voices"]["map"]([=](auto voice) mutable
                {
                    return (object{
                        object::pair{std::string("value"), voice->voice_id}, 
                        object::pair{std::string("label"), std::string("ElevenLabs - ") + voice->name + string_empty}, 
                        object::pair{std::string("provider"), std::string("elevenlabs")}, 
                        object::pair{std::string("gender"), (voice->labels->gender == std::string("female")) ? std::string("female") : std::string("male")}, 
                        object::pair{std::string("language"), std::string("en")}, 
                        object::pair{std::string("features"), array<any>{ OR((voice->category), (std::string("professional"))), OR((voice->labels->description), (std::string("natural"))) }}
                    });
                }
                );
                return apiVoices;
            }
            catch (const any& error)
            {
                console->error(std::string("Error fetching ElevenLabs voices:"), error);
                return elevenLabsVoiceModels;
            }
        }
        }, 
        object::pair{std::string("staleTime"), 60 * 60 * 1000}, 
        object::pair{std::string("refetchOnWindowFocus"), false}
    });
};


