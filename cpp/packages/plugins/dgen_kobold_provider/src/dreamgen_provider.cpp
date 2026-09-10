#include "dreamgen_provider.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace elizaos {
namespace {

constexpr std::size_t kMaxPromptBytes = 256U * 1024U;
constexpr std::size_t kMaxSystemPromptBytes = 128U * 1024U;
constexpr std::size_t kMaxCharacterNameBytes = 128U;
constexpr int kMaxTokens = 32768;

bool isBlank(const std::string& text) {
    return std::all_of(text.begin(), text.end(), [](unsigned char character) {
        return character == ' ' || character == '\t' ||
               character == '\r' || character == '\n';
    });
}

void validateInput(const std::string& prompt, const std::string& system_prompt,
                   const std::string& api_key, const std::string& model,
                   int max_tokens, double temperature,
                   const std::string& character_name) {
    if (prompt.empty() || isBlank(prompt) || prompt.size() > kMaxPromptBytes) {
        throw std::invalid_argument("DreamGen prompt is empty or exceeds the size limit");
    }
    if (system_prompt.size() > kMaxSystemPromptBytes ||
        character_name.size() > kMaxCharacterNameBytes) {
        throw std::invalid_argument("DreamGen message field exceeds the size limit");
    }
    if (api_key.empty()) throw std::invalid_argument("DreamGen API credential is required");
    if (api_key.size() > 8192U ||
        std::any_of(api_key.begin(), api_key.end(), [](unsigned char character) {
            return character <= 0x20U || character == 0x7FU;
        })) {
        throw std::invalid_argument("DreamGen API credential is invalid");
    }
    if (model != DreamGenProvider::MODEL_SMALL &&
        model != DreamGenProvider::MODEL_LARGE) {
        throw std::invalid_argument("DreamGen model is not supported");
    }
    if (max_tokens < 1 || max_tokens > kMaxTokens) {
        throw std::invalid_argument("DreamGen max_tokens is outside the allowed range");
    }
    if (!std::isfinite(temperature) || temperature < 0.0 || temperature > 2.0) {
        throw std::invalid_argument("DreamGen temperature is outside the allowed range");
    }
}

std::string parseCompletion(const nlohmann::json& response) {
    const auto choices = response.find("choices");
    if (choices == response.end() || !choices->is_array() || choices->empty()) {
        throw std::runtime_error("DreamGen response has no completion choices");
    }
    const auto& choice = choices->front();
    if (!choice.is_object()) throw std::runtime_error("DreamGen choice is malformed");
    const auto finish = choice.find("finish_reason");
    if (finish == choice.end() || !finish->is_string()) {
        throw std::runtime_error("DreamGen finish reason is malformed");
    }
    const std::string reason = finish->get<std::string>();
    if (reason == "content_filter") {
        throw std::runtime_error("DreamGen generation was blocked by content filtering");
    }
    if (reason != "stop" && reason != "length") {
        throw std::runtime_error("DreamGen finish reason is unsupported");
    }
    const auto message = choice.find("message");
    if (message == choice.end() || !message->is_object()) {
        throw std::runtime_error("DreamGen message is malformed");
    }
    const auto content = message->find("content");
    if (content == message->end() || !content->is_string()) {
        throw std::runtime_error("DreamGen content is malformed");
    }
    std::string text = content->get<std::string>();
    if (text.empty() || isBlank(text) || text.size() > 1024U * 1024U) {
        throw std::runtime_error("DreamGen generated text is empty or too large");
    }
    return text;
}

nlohmann::json messages(const std::string& prompt,
                        const std::string& system_prompt,
                        const std::string* name) {
    nlohmann::json result = nlohmann::json::array();
    if (!system_prompt.empty()) {
        result.push_back({{"role", "system"}, {"content", system_prompt}});
    }
    nlohmann::json user = {{"role", "user"}, {"content", prompt}};
    if (name != nullptr) user["name"] = *name;
    result.push_back(std::move(user));
    return result;
}

} // namespace

std::string DreamGenProvider::generate(
    const std::string& prompt, const std::string& system_prompt,
    const std::string& api_key, const std::string& model, int max_tokens,
    double temperature, const std::string& character_name,
    std::shared_ptr<const HttpTransport> transport,
    HttpRequestOptions request_options) {
    static_assert(API_ENDPOINT[0] == 'h' && API_ENDPOINT[4] == 's',
                  "DreamGen endpoint must use HTTPS");
    validateInput(prompt, system_prompt, api_key, model, max_tokens,
                  temperature, character_name);
    const std::string* name = character_name.empty() ? nullptr : &character_name;
    std::string request_model = model;
    if (name != nullptr) request_model += "/text";
    nlohmann::json body = {
        {"model", request_model}, {"messages", messages(prompt, system_prompt, name)},
        {"max_tokens", max_tokens}, {"temperature", temperature},
        {"frequency_penalty", 0.1}, {"presence_penalty", 0.1}};
    if (name != nullptr) {
        body["extra_body"] = {{"min_p", 0.05}, {"repetition_penalty", 1.02},
            {"dry", {{"multiplier", 0.8}, {"base", 1.75}, {"allowedLength", 2}}}};
    }
    const HttpClient client(std::move(transport), request_options);
    return parseCompletion(client.postJson(API_ENDPOINT, body, api_key));
}

std::string DreamGenProvider::generate_narration(
    const std::string& prompt, const std::string& system_prompt,
    const std::string& api_key, int max_tokens, double temperature,
    std::shared_ptr<const HttpTransport> transport,
    HttpRequestOptions request_options) {
    validateInput(prompt, system_prompt, api_key, MODEL_LARGE, max_tokens,
                  temperature, "");
    const std::string narrator_name;
    const nlohmann::json body = {
        {"model", std::string(MODEL_LARGE) + "/text"},
        {"messages", messages(prompt, system_prompt, &narrator_name)},
        {"max_tokens", max_tokens}, {"temperature", temperature},
        {"frequency_penalty", 0.1}, {"presence_penalty", 0.1}};
    const HttpClient client(std::move(transport), request_options);
    return parseCompletion(client.postJson(API_ENDPOINT, body, api_key));
}

} // namespace elizaos
