#include "koboldcpp_provider.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace elizaos {
namespace {

constexpr std::size_t kMaxPromptBytes = 256U * 1024U;
constexpr std::size_t kMaxSystemPromptBytes = 128U * 1024U;
constexpr int kMaxTokens = 32768;

bool isBlank(const std::string& text) {
    return std::all_of(text.begin(), text.end(), [](unsigned char character) {
        return character == ' ' || character == '\t' ||
               character == '\r' || character == '\n';
    });
}

void validateText(const std::string& prompt, const std::string& system = "") {
    if (prompt.empty() || isBlank(prompt) || prompt.size() > kMaxPromptBytes) {
        throw std::invalid_argument("KoboldCpp prompt is empty or exceeds the size limit");
    }
    if (system.size() > kMaxSystemPromptBytes) {
        throw std::invalid_argument("KoboldCpp system prompt exceeds the size limit");
    }
}

void validateSampling(int tokens, double temperature) {
    if (tokens < 1 || tokens > kMaxTokens) {
        throw std::invalid_argument("KoboldCpp token limit is outside the allowed range");
    }
    if (!std::isfinite(temperature) || temperature < 0.0 || temperature > 2.0) {
        throw std::invalid_argument("KoboldCpp temperature is outside the allowed range");
    }
}

bool parseDecimal(std::string_view text, unsigned int& value) {
    if (text.empty()) return false;
    const auto result = std::from_chars(text.data(), text.data() + text.size(), value, 10);
    return result.ec == std::errc{} && result.ptr == text.data() + text.size();
}

bool parseIpv4(std::string_view host, std::array<unsigned int, 4U>& octets) {
    std::size_t start = 0U;
    for (std::size_t index = 0U; index < octets.size(); ++index) {
        const std::size_t end = host.find('.', start);
        const bool last = index + 1U == octets.size();
        if ((last && end != std::string_view::npos) ||
            (!last && end == std::string_view::npos)) return false;
        const std::size_t component_end = last ? host.size() : end;
        if (!parseDecimal(host.substr(start, component_end - start), octets[index]) ||
            octets[index] > 255U) return false;
        start = component_end + 1U;
    }
    return true;
}

bool privateIpv4(const std::array<unsigned int, 4U>& value) {
    return value[0] == 10U || value[0] == 127U ||
           (value[0] == 172U && value[1] >= 16U && value[1] <= 31U) ||
           (value[0] == 192U && value[1] == 168U) ||
           (value[0] == 169U && value[1] == 254U);
}

std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return value;
}

bool validHostname(std::string_view host) {
    if (host.empty() || host.size() > 253U || host.front() == '.' || host.back() == '.') {
        return false;
    }
    std::size_t label = 0U;
    bool hyphen = false;
    for (const char raw_character : host) {
        const unsigned char character = static_cast<unsigned char>(raw_character);
        if (character == '.') {
            if (label == 0U || hyphen) return false;
            label = 0U;
            hyphen = false;
            continue;
        }
        if ((std::isalnum(character) == 0 && character != '-') ||
            (label == 0U && character == '-') || label >= 63U) return false;
        ++label;
        hyphen = character == '-';
    }
    return label != 0U && !hyphen;
}

bool validIpv6(std::string_view host) {
    return !host.empty() && host.find(':') != std::string_view::npos &&
           std::all_of(host.begin(), host.end(), [](unsigned char character) {
               return std::isxdigit(character) != 0 || character == ':' || character == '.';
           });
}

bool privateIpv6(std::string_view host) {
    if (host == "::1") return true;
    const std::size_t separator = host.find(':');
    if (separator == std::string_view::npos || separator == 0U || separator > 4U) return false;
    unsigned int first = 0U;
    const auto result = std::from_chars(host.data(), host.data() + separator, first, 16);
    if (result.ec != std::errc{} || result.ptr != host.data() + separator) return false;
    return (first >= 0xFC00U && first <= 0xFDFFU) ||
           (first >= 0xFE80U && first <= 0xFEBFU);
}

std::string endpointBase(const std::string& endpoint, KoboldEndpointPolicy policy) {
    if (endpoint.empty() || endpoint.size() > 2048U ||
        endpoint.find_first_of("?#@\\") != std::string::npos ||
        std::any_of(endpoint.begin(), endpoint.end(), [](unsigned char character) {
            return std::iscntrl(character) != 0 || std::isspace(character) != 0;
        })) throw std::invalid_argument("KoboldCpp endpoint is malformed");
    const std::size_t scheme_end = endpoint.find("://");
    if (scheme_end == std::string::npos) {
        throw std::invalid_argument("KoboldCpp endpoint must include an HTTP scheme");
    }
    const std::string scheme = lowercase(endpoint.substr(0U, scheme_end));
    if (scheme != "http" && scheme != "https") {
        throw std::invalid_argument("KoboldCpp endpoint scheme is not allowed");
    }
    const std::size_t authority_start = scheme_end + 3U;
    const std::size_t path_start = endpoint.find('/', authority_start);
    const std::string authority = endpoint.substr(
        authority_start, path_start == std::string::npos
                             ? std::string::npos : path_start - authority_start);
    if (authority.empty() ||
        (path_start != std::string::npos &&
         endpoint.find_first_not_of('/', path_start) != std::string::npos)) {
        throw std::invalid_argument("KoboldCpp endpoint must be an origin without a path");
    }
    std::string host;
    std::string port;
    bool ipv6 = false;
    if (authority.front() == '[') {
        const std::size_t close = authority.find(']');
        if (close == std::string::npos) {
            throw std::invalid_argument("KoboldCpp IPv6 endpoint is malformed");
        }
        host = authority.substr(1U, close - 1U);
        ipv6 = true;
        if (close + 1U < authority.size()) {
            if (authority[close + 1U] != ':') {
                throw std::invalid_argument("KoboldCpp endpoint port is malformed");
            }
            port = authority.substr(close + 2U);
        }
    } else {
        const std::size_t colon = authority.rfind(':');
        if (colon != std::string::npos) {
            if (authority.find(':') != colon) {
                throw std::invalid_argument("KoboldCpp IPv6 endpoint requires brackets");
            }
            host = authority.substr(0U, colon);
            port = authority.substr(colon + 1U);
        } else host = authority;
    }
    host = lowercase(host);
    if (host.empty()) throw std::invalid_argument("KoboldCpp endpoint host is missing");
    if (!port.empty()) {
        unsigned int parsed = 0U;
        if (!parseDecimal(port, parsed) || parsed == 0U || parsed > 65535U) {
            throw std::invalid_argument("KoboldCpp endpoint port is invalid");
        }
    } else if (authority.back() == ':') {
        throw std::invalid_argument("KoboldCpp endpoint port is missing");
    }
    bool local_or_private = false;
    if (ipv6) {
        if (!validIpv6(host)) throw std::invalid_argument("KoboldCpp IPv6 host is invalid");
        local_or_private = privateIpv6(host);
    } else {
        std::array<unsigned int, 4U> octets{};
        if (parseIpv4(host, octets)) local_or_private = privateIpv4(octets);
        else {
            if (!validHostname(host)) throw std::invalid_argument("KoboldCpp host is invalid");
            local_or_private = host == "localhost";
        }
    }
    if (!local_or_private && policy != KoboldEndpointPolicy::AllowRemote) {
        throw std::invalid_argument("KoboldCpp remote endpoint requires explicit AllowRemote");
    }
    std::string result = scheme + "://" + (ipv6 ? "[" + host + "]" : host);
    if (!port.empty()) result += ':' + port;
    return result;
}

std::string parseChat(const nlohmann::json& response) {
    const auto choices = response.find("choices");
    if (choices == response.end() || !choices->is_array() || choices->empty()) {
        throw std::runtime_error("KoboldCpp response has no completion choices");
    }
    const auto& choice = choices->front();
    if (!choice.is_object()) throw std::runtime_error("KoboldCpp choice is malformed");
    const auto finish = choice.find("finish_reason");
    if (finish == choice.end() || !finish->is_string()) {
        throw std::runtime_error("KoboldCpp finish reason is malformed");
    }
    const std::string reason = finish->get<std::string>();
    if (reason != "stop" && reason != "length") {
        throw std::runtime_error("KoboldCpp finish reason is unsupported");
    }
    const auto message = choice.find("message");
    if (message == choice.end() || !message->is_object()) {
        throw std::runtime_error("KoboldCpp message is malformed");
    }
    const auto content = message->find("content");
    if (content == message->end() || !content->is_string()) {
        throw std::runtime_error("KoboldCpp content is malformed");
    }
    std::string text = content->get<std::string>();
    if (text.empty() || isBlank(text) || text.size() > 1024U * 1024U) {
        throw std::runtime_error("KoboldCpp generated text is empty or too large");
    }
    return text;
}

std::string parseNative(const nlohmann::json& response) {
    const auto results = response.find("results");
    if (results == response.end() || !results->is_array() || results->empty()) {
        throw std::runtime_error("KoboldCpp response has no native results");
    }
    const auto& result = results->front();
    if (!result.is_object()) throw std::runtime_error("KoboldCpp native result is malformed");
    const auto text_value = result.find("text");
    if (text_value == result.end() || !text_value->is_string()) {
        throw std::runtime_error("KoboldCpp native text is malformed");
    }
    std::string text = text_value->get<std::string>();
    if (text.empty() || isBlank(text) || text.size() > 1024U * 1024U) {
        throw std::runtime_error("KoboldCpp generated text is empty or too large");
    }
    return text;
}

} // namespace

std::string KoboldCppProvider::generate(
    const std::string& prompt, const std::string& system_prompt,
    const std::string& endpoint, int max_tokens, double temperature,
    std::shared_ptr<const HttpTransport> transport, KoboldEndpointPolicy policy,
    HttpRequestOptions options) {
    validateText(prompt, system_prompt);
    validateSampling(max_tokens, temperature);
    const std::string base = endpointBase(endpoint, policy);
    nlohmann::json messages = nlohmann::json::array();
    if (!system_prompt.empty()) {
        messages.push_back({{"role", "system"}, {"content", system_prompt}});
    }
    messages.push_back({{"role", "user"}, {"content", prompt}});
    const nlohmann::json body = {{"messages", messages},
                                 {"max_tokens", max_tokens},
                                 {"temperature", temperature}};
    const HttpClient client(std::move(transport), options);
    return parseChat(client.postJson(base + "/v1/chat/completions", body));
}

std::string KoboldCppProvider::generate_native(
    const std::string& prompt, const std::string& endpoint, int max_length,
    double temperature, double rep_pen, int top_k, double top_p,
    std::shared_ptr<const HttpTransport> transport, KoboldEndpointPolicy policy,
    HttpRequestOptions options) {
    validateText(prompt);
    validateSampling(max_length, temperature);
    if (!std::isfinite(rep_pen) || rep_pen < 0.5 || rep_pen > 2.0) {
        throw std::invalid_argument("KoboldCpp repetition penalty is invalid");
    }
    if (top_k < 0 || top_k > 10000) {
        throw std::invalid_argument("KoboldCpp top_k is invalid");
    }
    if (!std::isfinite(top_p) || top_p < 0.0 || top_p > 1.0) {
        throw std::invalid_argument("KoboldCpp top_p is invalid");
    }
    const std::string base = endpointBase(endpoint, policy);
    const nlohmann::json body = {{"prompt", prompt}, {"max_length", max_length},
        {"temperature", temperature}, {"rep_pen", rep_pen}, {"top_k", top_k},
        {"top_p", top_p}};
    const HttpClient client(std::move(transport), options);
    return parseNative(client.postJson(base + "/api/v1/generate", body));
}

bool KoboldCppProvider::is_available(
    const std::string& endpoint, std::shared_ptr<const HttpTransport> transport,
    KoboldEndpointPolicy policy, HttpRequestOptions options) {
    const std::string base = endpointBase(endpoint, policy);
    const HttpClient client(std::move(transport), options);
    try {
        const nlohmann::json model = client.getJson(base + "/api/v1/model");
        return model.contains("result") && model["result"].is_string() &&
               !model["result"].get<std::string>().empty();
    } catch (...) {
        return false;
    }
}

} // namespace elizaos
