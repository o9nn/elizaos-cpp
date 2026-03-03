#include "koboldcpp_provider.hpp"

namespace elizaos {

std::string KoboldCppProvider::generate(
    const std::string& prompt,
    const std::string& system_prompt,
    const std::string& endpoint,
    int max_tokens,
    double temperature
) {
    json messages = json::array();

    // System prompt
    if (!system_prompt.empty()) {
        messages.push_back({{"role", "system"}, {"content", system_prompt}});
    }

    // User prompt
    messages.push_back({{"role", "user"}, {"content", prompt}});

    json body = {
        {"messages", messages},
        {"max_tokens", max_tokens},
        {"temperature", temperature}
    };

    // KoboldCpp doesn't require an API key - it's a local server
    std::string url = endpoint + "/v1/chat/completions";
    json response = HttpClient::post(url, body);

    if (response.contains("choices") && !response["choices"].empty()) {
        return response["choices"][0]["message"]["content"].get<std::string>();
    }

    throw std::runtime_error("KoboldCpp returned empty response from " + url);
}

std::string KoboldCppProvider::generate_native(
    const std::string& prompt,
    const std::string& endpoint,
    int max_length,
    double temperature,
    double rep_pen,
    int top_k,
    double top_p
) {
    json body = {
        {"prompt", prompt},
        {"max_length", max_length},
        {"temperature", temperature},
        {"rep_pen", rep_pen},
        {"top_k", top_k},
        {"top_p", top_p}
    };

    std::string url = endpoint + "/api/v1/generate";
    json response = HttpClient::post(url, body);

    if (response.contains("results") && !response["results"].empty()) {
        return response["results"][0]["text"].get<std::string>();
    }

    throw std::runtime_error("KoboldCpp native API returned empty response from " + url);
}

bool KoboldCppProvider::is_available(const std::string& endpoint) {
    try {
        // Use the model info endpoint as a health check
        json empty_body = json::object();
        std::string url = endpoint + "/api/v1/model";
        // This would need a GET request, but we'll use a simple approach:
        // try to reach the server and see if it responds
        HttpClient::post(url, empty_body);
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace elizaos
