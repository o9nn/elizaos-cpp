#include "use-elevenlabs-voices.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useElevenLabsVoices() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [apiKey, setApiKey] = useState<string | nullptr>(nullptr);

    // Load API key from localStorage or another source
    useEffect(() => {
        const auto storedKey = localStorage.getItem('ELEVENLABS_API_KEY');
        setApiKey(storedKey);
        }, []);

        return useQuery({;
            queryKey: ['elevenlabs-voices', apiKey],
            queryFn: async () => {
                // If no API key is available, use the hardcoded models
                if (!apiKey) {
                    return elevenLabsVoiceModels;
                }

                try {
                    const auto response = fetch('https://api.elevenlabs.io/v2/voices', {;
                        method: 'GET',
                        headers: {
                            'xi-api-key': apiKey,
                            },
                            });

                            if (!response.ok) {
                                std::cerr << 'Failed to fetch ElevenLabs voices:' << response.statusText << std::endl;
                                return elevenLabsVoiceModels;
                            }

                            const auto data = response.json();

                            // Transform the API response to match our VoiceModel format
                            const std::vector<VoiceModel> apiVoices = data.voices.map((voice: ElevenLabsVoice) => ({;
                                value: voice.voice_id,
                                "ElevenLabs - " + std::to_string(voice.name)
                                provider: 'elevenlabs',
                                gender: voice.labels.gender == 'female' ? 'female' : 'male',
                                language: 'en',
                                features: [voice.category || 'professional', voice.labels.description || 'natural'],
                                }));

                                return apiVoices;
                                } catch (error) {
                                    std::cerr << 'Error fetching ElevenLabs voices:' << error << std::endl;
                                    return elevenLabsVoiceModels;
                                }
                                },
                                // Refresh the data every hour
                                staleTime: 60 * 60 * 1000,
                                // Don't refetch on window focus
                                refetchOnWindowFocus: false,
                                });

}

} // namespace elizaos
