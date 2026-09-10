#pragma once

#include <memory>
#include <string>

#include "http_client.hpp"

namespace elizaos {

/** Stateless facade. Credentials are per-call and never retained. */
class DreamGenProvider {
public:
    static constexpr const char* PROVIDER_NAME = "dreamgen";
    static constexpr const char* API_ENDPOINT =
        "https://dreamgen.com/api/openai/v1/chat/completions";
    static constexpr const char* MODEL_SMALL = "lucid-v1-medium";
    static constexpr const char* MODEL_LARGE = "lucid-v1-extra-large";

    static std::string generate(
        const std::string& prompt,
        const std::string& system_prompt,
        const std::string& api_key,
        const std::string& model = MODEL_LARGE,
        int max_tokens = 500,
        double temperature = 0.8,
        const std::string& character_name = "",
        std::shared_ptr<const HttpTransport> transport = {},
        HttpRequestOptions request_options = {});

    static std::string generate_narration(
        const std::string& prompt,
        const std::string& system_prompt,
        const std::string& api_key,
        int max_tokens = 500,
        double temperature = 1.0,
        std::shared_ptr<const HttpTransport> transport = {},
        HttpRequestOptions request_options = {});
};

} // namespace elizaos
