#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
import type { VoiceModel } from '@/config/voice-models';

// TODO: Move this to a shared config file, or the 11labs plugin once plugin categories are implemented

struct ElevenLabsVoice {
    std::string voice_id;
    std::string name;
    std::string category;
    std::optional<{> labels;
    std::optional<std::string> accent;
    std::optional<std::string> age;
    std::optional<std::string> description;
    std::optional<std::string> gender;
    std::optional<std::string> use_case;
    std::optional<std::string> preview_url;
};


, []);

  return useQuery({
    queryKey: ['elevenlabs-voices', apiKey],
    queryFn: async () => {
      // If no API key is available, use the hardcoded models
      if (!apiKey) {
        return elevenLabsVoiceModels;
      }

      try {
        const response = await fetch('https://api.elevenlabs.io/v2/voices', {
          method: 'GET',
          headers: {
            'xi-api-key': apiKey,
          },
        });

        if (!response.ok) {
          console.error('Failed to fetch ElevenLabs voices:', response.statusText);
          return elevenLabsVoiceModels;
        }

        const data = await response.json();

        // Transform the API response to match our VoiceModel format
        const apiVoices: VoiceModel[] = data.voices.map((voice: ElevenLabsVoice) => ({
          value: voice.voice_id,
          label: `ElevenLabs - ${voice.name}`,
          provider: 'elevenlabs',
          gender: voice.labels?.gender === 'female' ? 'female' : 'male',
          language: 'en',
          features: [voice.category || 'professional', voice.labels?.description || 'natural'],
        }));

        return apiVoices;
      } catch (error) {
        console.error('Error fetching ElevenLabs voices:', error);
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
