#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "http_client.hpp"

namespace elizaos {

/**
 * DreamGen model provider for ElizaOS.
 *
 * Provides access to DreamGen's lucid-v1 models via their OpenAI-compatible API.
 * Supports the extended 'text' role for character-based creative writing and
 * role-playing, as well as standard assistant-mode completions.
 *
 * Configuration:
 *   character.settings.secrets.DREAMGEN_API_KEY = "your_api_key"
 *
 * Models:
 *   - lucid-v1-medium (small/fast)
 *   - lucid-v1-extra-large (large/quality)
 *
 * API Endpoint: https://dreamgen.com/api/openai/v1/chat/completions
 */
class DreamGenProvider {
public:
    static constexpr const char* PROVIDER_NAME = "dreamgen";
    static constexpr const char* API_ENDPOINT = "https://dreamgen.com/api/openai/v1/chat/completions";
    static constexpr const char* MODEL_SMALL = "lucid-v1-medium";
    static constexpr const char* MODEL_LARGE = "lucid-v1-extra-large";

    /**
     * Generate text using the DreamGen API.
     *
     * @param prompt The input prompt
     * @param system_prompt Optional system prompt for context
     * @param api_key The DreamGen API key
     * @param model The model to use (defaults to lucid-v1-extra-large)
     * @param max_tokens Maximum tokens to generate
     * @param temperature Sampling temperature (0.0-2.0)
     * @param character_name Optional character name for text-role generation
     * @return Generated text string
     */
    static std::string generate(
        const std::string& prompt,
        const std::string& system_prompt,
        const std::string& api_key,
        const std::string& model = MODEL_LARGE,
        int max_tokens = 500,
        double temperature = 0.8,
        const std::string& character_name = ""
    );

    /**
     * Generate text in narrator mode (third-person prose).
     */
    static std::string generate_narration(
        const std::string& prompt,
        const std::string& system_prompt,
        const std::string& api_key,
        int max_tokens = 500,
        double temperature = 1.0
    );
};

} // namespace elizaos
