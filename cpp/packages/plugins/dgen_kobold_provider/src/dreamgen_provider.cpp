#include "dreamgen_provider.hpp"

namespace elizaos {

std::string DreamGenProvider::generate(
    const std::string& prompt,
    const std::string& system_prompt,
    const std::string& api_key,
    const std::string& model,
    int max_tokens,
    double temperature,
    const std::string& character_name
) {
    if (api_key.empty()) {
        throw std::runtime_error("DreamGen API key is required");
    }

    json messages = json::array();

    // System prompt
    if (!system_prompt.empty()) {
        messages.push_back({{"role", "system"}, {"content", system_prompt}});
    }

    // User prompt - if character_name is set, use the name field to trigger text role
    json user_msg = {{"role", "user"}, {"content", prompt}};
    if (!character_name.empty()) {
        user_msg["name"] = character_name;
    }
    messages.push_back(user_msg);

    // Build the model string with text-role suffix for creative writing
    std::string model_str = model;
    if (!character_name.empty()) {
        model_str += "/text";
    }

    json body = {
        {"model", model_str},
        {"messages", messages},
        {"max_tokens", max_tokens},
        {"temperature", temperature},
        {"frequency_penalty", 0.1},
        {"presence_penalty", 0.1}
    };

    // Add DRY sampler for creative writing quality
    if (!character_name.empty()) {
        body["extra_body"] = {
            {"min_p", 0.05},
            {"repetition_penalty", 1.02},
            {"dry", {
                {"multiplier", 0.8},
                {"base", 1.75},
                {"allowedLength", 2}
            }}
        };
    }

    json response = HttpClient::post(API_ENDPOINT, body, api_key);

    if (response.contains("choices") && !response["choices"].empty()) {
        return response["choices"][0]["message"]["content"].get<std::string>();
    }

    throw std::runtime_error("DreamGen API returned empty response");
}

std::string DreamGenProvider::generate_narration(
    const std::string& prompt,
    const std::string& system_prompt,
    const std::string& api_key,
    int max_tokens,
    double temperature
) {
    json messages = json::array();

    if (!system_prompt.empty()) {
        messages.push_back({{"role", "system"}, {"content", system_prompt}});
    }

    // Empty name = narrator mode in DreamGen
    messages.push_back({{"role", "user"}, {"name", ""}, {"content", prompt}});

    json body = {
        {"model", std::string(MODEL_LARGE) + "/text"},
        {"messages", messages},
        {"max_tokens", max_tokens},
        {"temperature", temperature},
        {"frequency_penalty", 0.1},
        {"presence_penalty", 0.1}
    };

    json response = HttpClient::post(API_ENDPOINT, body, api_key);

    if (response.contains("choices") && !response["choices"].empty()) {
        return response["choices"][0]["message"]["content"].get<std::string>();
    }

    throw std::runtime_error("DreamGen API returned empty response");
}

} // namespace elizaos
