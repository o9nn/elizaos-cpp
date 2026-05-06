#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "http_client.hpp"

namespace elizaos {

/**
 * KoboldCpp model provider for ElizaOS.
 *
 * Provides access to locally-running KoboldCpp instances for GGUF model inference.
 * Supports both the KoboldCpp native API and the OpenAI-compatible API endpoint.
 *
 * Configuration:
 *   character.settings.secrets.KOBOLDCPP_ENDPOINT = "http://localhost:5001"
 *
 * Supported APIs:
 *   - /v1/chat/completions (OpenAI-compatible)
 *   - /v1/completions (OpenAI-compatible text completion)
 *   - /api/v1/generate (KoboldCpp native)
 *
 * Features:
 *   - Local inference with GGUF models (llama.cpp backend)
 *   - GPU acceleration via Vulkan, CUDA, CLBlast
 *   - No API key required (local server)
 *   - Low latency, full privacy
 */
class KoboldCppProvider {
public:
    static constexpr const char* PROVIDER_NAME = "koboldcpp";
    static constexpr const char* DEFAULT_ENDPOINT = "http://localhost:5001";

    /**
     * Generate text using the KoboldCpp OpenAI-compatible chat API.
     *
     * @param prompt The input prompt
     * @param system_prompt Optional system prompt for context
     * @param endpoint The KoboldCpp server endpoint URL
     * @param max_tokens Maximum tokens to generate
     * @param temperature Sampling temperature (0.0-2.0)
     * @return Generated text string
     */
    static std::string generate(
        const std::string& prompt,
        const std::string& system_prompt,
        const std::string& endpoint = DEFAULT_ENDPOINT,
        int max_tokens = 500,
        double temperature = 0.7
    );

    /**
     * Generate text using the KoboldCpp native API.
     * This gives access to KoboldCpp-specific parameters like rep_pen, top_k, etc.
     *
     * @param prompt The full prompt text (including any formatting)
     * @param endpoint The KoboldCpp server endpoint URL
     * @param max_length Maximum tokens to generate
     * @param temperature Sampling temperature
     * @param rep_pen Repetition penalty
     * @param top_k Top-K sampling parameter
     * @param top_p Top-P (nucleus) sampling parameter
     * @return Generated text string
     */
    static std::string generate_native(
        const std::string& prompt,
        const std::string& endpoint = DEFAULT_ENDPOINT,
        int max_length = 500,
        double temperature = 0.7,
        double rep_pen = 1.1,
        int top_k = 40,
        double top_p = 0.9
    );

    /**
     * Check if a KoboldCpp server is running and responsive.
     *
     * @param endpoint The KoboldCpp server endpoint URL
     * @return true if the server is reachable
     */
    static bool is_available(const std::string& endpoint = DEFAULT_ENDPOINT);
};

} // namespace elizaos
