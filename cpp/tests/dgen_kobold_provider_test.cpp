#include <gtest/gtest.h>

#include "dreamgen_provider.hpp"
#include "koboldcpp_provider.hpp"
#include "plugin.hpp"

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace {

using elizaos::HttpRequest;
using elizaos::HttpResponse;
using elizaos::HttpTransport;

class FakeTransport final : public HttpTransport {
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;

    explicit FakeTransport(Handler handler) : handler_(std::move(handler)) {}

    HttpResponse execute(const HttpRequest& request) const override {
        calls_.fetch_add(1U, std::memory_order_relaxed);
        return handler_(request);
    }

    std::size_t calls() const noexcept {
        return calls_.load(std::memory_order_relaxed);
    }

private:
    const Handler handler_;
    mutable std::atomic<std::size_t> calls_{0U};
};

HttpResponse jsonResponse(const nlohmann::json& body) {
    return {200L, "application/json; charset=utf-8", body.dump()};
}

HttpResponse chatResponse(const std::string& text,
                          const std::string& finish_reason = "stop") {
    return jsonResponse({{"choices", nlohmann::json::array({
        {{"finish_reason", finish_reason},
         {"message", {{"role", "assistant"}, {"content", text}}}}
    })}});
}

HttpResponse nativeResponse(const std::string& text) {
    return jsonResponse({{"results", nlohmann::json::array({{{"text", text}}})}});
}

std::string headerValue(const HttpRequest& request, const std::string& name) {
    for (const auto& header : request.headers) {
        if (header.first == name) return header.second;
    }
    return "";
}

template <typename Callable>
std::string exceptionText(Callable&& callable) {
    try {
        callable();
    } catch (const std::exception& error) {
        return error.what();
    }
    return "";
}

TEST(DgenKoboldProvider, DreamGenExactRequestHeadersAndHttpsEndpoint) {
    const std::string secret = "dg-secret-value";
    auto transport = std::make_shared<FakeTransport>(
        [&](const HttpRequest& request) {
            EXPECT_EQ(request.method, elizaos::HttpMethod::Post);
            EXPECT_EQ(request.url, elizaos::DreamGenProvider::API_ENDPOINT);
            EXPECT_EQ(request.url.rfind("https://", 0U), 0U);
            EXPECT_EQ(headerValue(request, "Accept"), "application/json");
            EXPECT_EQ(headerValue(request, "Content-Type"), "application/json");
            EXPECT_EQ(headerValue(request, "Authorization"), "Bearer " + secret);
            EXPECT_EQ(request.connect_timeout, std::chrono::milliseconds(1234));
            EXPECT_EQ(request.request_timeout, std::chrono::milliseconds(4321));
            EXPECT_EQ(request.max_response_bytes, 4096U);
            EXPECT_FALSE(request.idempotent);
            EXPECT_EQ(request.retry_policy.mode, elizaos::RetryMode::Never);
            EXPECT_EQ(request.retry_policy.max_attempts, 1U);

            const nlohmann::json body = nlohmann::json::parse(request.body);
            const nlohmann::json expected = {
                {"model", "lucid-v1-medium/text"},
                {"messages", nlohmann::json::array({
                    {{"role", "system"}, {"content", "Be concise"}},
                    {{"role", "user"}, {"content", "Hello"}, {"name", "Eliza"}}
                })},
                {"max_tokens", 321},
                {"temperature", 0.6},
                {"frequency_penalty", 0.1},
                {"presence_penalty", 0.1},
                {"extra_body", {
                    {"min_p", 0.05}, {"repetition_penalty", 1.02},
                    {"dry", {{"multiplier", 0.8}, {"base", 1.75},
                             {"allowedLength", 2}}}
                }}
            };
            EXPECT_EQ(body, expected);
            return chatResponse("Dream result", "length");
        });

    const elizaos::HttpRequestOptions options{
        std::chrono::milliseconds(1234), std::chrono::milliseconds(4321), 4096U};
    EXPECT_EQ(elizaos::DreamGenProvider::generate(
                  "Hello", "Be concise", secret,
                  elizaos::DreamGenProvider::MODEL_SMALL, 321, 0.6, "Eliza",
                  transport, options),
              "Dream result");
    EXPECT_EQ(transport->calls(), 1U);
}

TEST(DgenKoboldProvider, NarrationExactMessageShape) {
    auto transport = std::make_shared<FakeTransport>([](const HttpRequest& request) -> HttpResponse {
        const auto body = nlohmann::json::parse(request.body);
        EXPECT_EQ(body["model"], "lucid-v1-extra-large/text");
        EXPECT_EQ(body["messages"].size(), 2U);
        EXPECT_EQ(body["messages"][1],
                  nlohmann::json({{"role", "user"}, {"content", "Scene"},
                                  {"name", ""}}));
        return chatResponse("Narration");
    });
    EXPECT_EQ(elizaos::DreamGenProvider::generate_narration(
                  "Scene", "Narrate", "secret", 44, 1.0, transport),
              "Narration");
}

TEST(DgenKoboldProvider, KoboldChatAndNativeRequestsAreExact) {
    std::mutex mutex;
    std::vector<HttpRequest> requests;
    auto transport = std::make_shared<FakeTransport>(
        [&](const HttpRequest& request) {
            {
                std::lock_guard<std::mutex> lock(mutex);
                requests.push_back(request);
            }
            if (request.url.find("/api/v1/generate") != std::string::npos) {
                return nativeResponse("Native result");
            }
            return chatResponse("Chat result");
        });

    EXPECT_EQ(elizaos::KoboldCppProvider::generate(
                  "Prompt", "System", "http://127.0.0.1:5001/", 99, 0.4,
                  transport),
              "Chat result");
    EXPECT_EQ(elizaos::KoboldCppProvider::generate_native(
                  "Raw", "http://192.168.1.9:5001", 77, 0.5, 1.2, 55, 0.85,
                  transport),
              "Native result");

    ASSERT_EQ(requests.size(), 2U);
    EXPECT_EQ(requests[0].url, "http://127.0.0.1:5001/v1/chat/completions");
    EXPECT_TRUE(headerValue(requests[0], "Authorization").empty());
    EXPECT_EQ(nlohmann::json::parse(requests[0].body), nlohmann::json({
        {"messages", nlohmann::json::array({
            {{"role", "system"}, {"content", "System"}},
            {{"role", "user"}, {"content", "Prompt"}}
        })}, {"max_tokens", 99}, {"temperature", 0.4}}));
    EXPECT_EQ(requests[1].url, "http://192.168.1.9:5001/api/v1/generate");
    EXPECT_EQ(nlohmann::json::parse(requests[1].body), nlohmann::json({
        {"prompt", "Raw"}, {"max_length", 77}, {"temperature", 0.5},
        {"rep_pen", 1.2}, {"top_k", 55}, {"top_p", 0.85}}));
    EXPECT_FALSE(requests[0].idempotent);
    EXPECT_FALSE(requests[1].idempotent);
}

TEST(DgenKoboldProvider, HealthCheckUsesIdempotentGetWithoutRetries) {
    auto transport = std::make_shared<FakeTransport>([](const HttpRequest& request) -> HttpResponse {
        EXPECT_EQ(request.method, elizaos::HttpMethod::Get);
        EXPECT_TRUE(request.idempotent);
        EXPECT_TRUE(request.body.empty());
        EXPECT_EQ(request.retry_policy.mode, elizaos::RetryMode::Never);
        EXPECT_EQ(request.retry_policy.max_attempts, 1U);
        EXPECT_EQ(request.url, "http://localhost:5001/api/v1/model");
        return jsonResponse({{"result", "test-model"}});
    });
    EXPECT_TRUE(elizaos::KoboldCppProvider::is_available(
        "http://localhost:5001", transport));
}

TEST(DgenKoboldProvider, DefaultWithoutTransportFailsTruthfully) {
    const std::string dream_error = exceptionText([] {
        static_cast<void>(elizaos::DreamGenProvider::generate(
            "prompt", "", "secret"));
    });
    EXPECT_EQ(dream_error, "HTTP transport is unavailable");
    const std::string kobold_error = exceptionText([] {
        static_cast<void>(elizaos::KoboldCppProvider::generate("prompt", ""));
    });
    EXPECT_EQ(kobold_error, "HTTP transport is unavailable");
    EXPECT_FALSE(elizaos::KoboldCppProvider::is_available());
}

TEST(DgenKoboldProvider, RemoteKoboldRequiresExplicitOptIn) {
    auto transport = std::make_shared<FakeTransport>([](const HttpRequest&) {
        return chatResponse("remote");
    });
    EXPECT_THROW(static_cast<void>(elizaos::KoboldCppProvider::generate(
                     "prompt", "", "https://example.com:443", 10, 0.5,
                     transport)),
                 std::invalid_argument);
    EXPECT_EQ(transport->calls(), 0U);
    EXPECT_EQ(elizaos::KoboldCppProvider::generate(
                  "prompt", "", "https://example.com:443", 10, 0.5,
                  transport, elizaos::KoboldEndpointPolicy::AllowRemote),
              "remote");
    EXPECT_EQ(transport->calls(), 1U);
}

TEST(DgenKoboldProvider, EndpointPolicyRejectsInjectionAndOddIpForms) {
    auto transport = std::make_shared<FakeTransport>([](const HttpRequest&) {
        return chatResponse("unused");
    });
    const std::vector<std::string> endpoints = {
        "file:///etc/passwd", "http://localhost:5001/path",
        "http://user@localhost:5001", "http://localhost:5001?x=1",
        "http://2130706433:5001", "http://0177.0.0.1:5001",
        "http://localhost:5001;echo", "http://[2001:4860:4860::8888]:5001"};
    for (const auto& endpoint : endpoints) {
        EXPECT_THROW(static_cast<void>(elizaos::KoboldCppProvider::generate(
                         "prompt", "", endpoint, 10, 0.5, transport)),
                     std::invalid_argument);
    }
    EXPECT_EQ(transport->calls(), 0U);
}

TEST(DgenKoboldProvider, ValidatesBoundedInputsBeforeTransport) {
    auto transport = std::make_shared<FakeTransport>([](const HttpRequest&) {
        return chatResponse("unused");
    });
    EXPECT_THROW(static_cast<void>(elizaos::DreamGenProvider::generate(
                     "", "", "secret", elizaos::DreamGenProvider::MODEL_LARGE,
                     1, 0.5, "", transport)), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(elizaos::DreamGenProvider::generate(
                     "x", "", "secret", "arbitrary/model", 1, 0.5, "", transport)),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(elizaos::DreamGenProvider::generate(
                     "x", "", "secret", elizaos::DreamGenProvider::MODEL_LARGE,
                     32769, 0.5, "", transport)), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(elizaos::KoboldCppProvider::generate_native(
                     "x", "http://localhost:5001", 1, 2.1, 1.0, 1, 0.5,
                     transport)), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(elizaos::KoboldCppProvider::generate_native(
                     "x", "http://localhost:5001", 1, 0.5, 3.0, 1, 0.5,
                     transport)), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(elizaos::KoboldCppProvider::generate_native(
                     "x", "http://localhost:5001", 1, 0.5, 1.0, 1, 1.1,
                     transport)), std::invalid_argument);
    EXPECT_THROW(static_cast<void>(elizaos::KoboldCppProvider::generate(
                     std::string(256U * 1024U + 1U, 'x'), "",
                     "http://localhost:5001", 1, 0.5, transport)),
                 std::invalid_argument);
    EXPECT_EQ(transport->calls(), 0U);
}

TEST(DgenKoboldProvider, RejectsStatusContentTypeMalformedAndOversize) {
    const std::vector<HttpResponse> responses = {
        {401L, "application/json", "{\"error\":\"secret server detail\"}"},
        {200L, "text/html", "{}"},
        {200L, "application/json", "{"},
        {200L, "application/json", std::string(65U, 'x')}
    };
    for (std::size_t index = 0U; index < responses.size(); ++index) {
        auto transport = std::make_shared<FakeTransport>(
            [&, index](const HttpRequest&) { return responses[index]; });
        const std::string error = exceptionText([&] {
            static_cast<void>(elizaos::KoboldCppProvider::generate(
                "prompt", "", "http://localhost:5001", 1, 0.5, transport,
                elizaos::KoboldEndpointPolicy::LoopbackOrPrivateOnly,
                {std::chrono::milliseconds(5), std::chrono::milliseconds(10), 64U}));
        });
        EXPECT_FALSE(error.empty());
        EXPECT_EQ(error.find("secret server detail"), std::string::npos);
        EXPECT_EQ(transport->calls(), 1U);
    }
}

TEST(DgenKoboldProvider, RejectsMalformedSchemasFinishReasonsAndEmptyText) {
    const std::vector<nlohmann::json> bodies = {
        nlohmann::json::object(), {{"choices", nlohmann::json::array()}},
        {{"choices", nlohmann::json::array({nlohmann::json::object()})}},
        {{"choices", nlohmann::json::array({{{"finish_reason", "tool_calls"},
                                             {"message", {{"content", "x"}}}}})}},
        {{"choices", nlohmann::json::array({{{"finish_reason", "stop"},
                                             {"message", {{"content", ""}}}}})}},
        {{"choices", nlohmann::json::array({{{"finish_reason", "stop"},
                                             {"message", {{"content", 4}}}}})}}
    };
    for (const auto& body : bodies) {
        auto transport = std::make_shared<FakeTransport>(
            [body](const HttpRequest&) { return jsonResponse(body); });
        EXPECT_THROW(static_cast<void>(elizaos::DreamGenProvider::generate(
                         "prompt", "", "secret",
                         elizaos::DreamGenProvider::MODEL_LARGE, 1, 0.5, "",
                         transport)), std::runtime_error);
    }
    auto native_empty = std::make_shared<FakeTransport>([](const HttpRequest&) {
        return jsonResponse({{"results", nlohmann::json::array({{{"text", " "}}})}});
    });
    EXPECT_THROW(static_cast<void>(elizaos::KoboldCppProvider::generate_native(
                     "prompt", "http://localhost:5001", 1, 0.5, 1.0, 1, 0.5,
                     native_empty)), std::runtime_error);
}

TEST(DgenKoboldProvider, TimeoutAndTransportTextAreSanitizedWithoutRetry) {
    const std::string secret = "do-not-surface-token";
    auto timeout = std::make_shared<FakeTransport>([](const HttpRequest&) -> HttpResponse {
        throw elizaos::TransportError(elizaos::TransportErrorCode::Timeout);
    });
    EXPECT_EQ(exceptionText([&] {
                  static_cast<void>(elizaos::DreamGenProvider::generate(
                      "prompt", "", secret, elizaos::DreamGenProvider::MODEL_LARGE,
                      1, 0.5, "", timeout));
              }),
              "HTTP request timed out");
    EXPECT_EQ(timeout->calls(), 1U);

    auto leaky = std::make_shared<FakeTransport>([&](const HttpRequest&) -> HttpResponse {
        throw std::runtime_error("Authorization: Bearer " + secret + " prompt");
    });
    const std::string error = exceptionText([&] {
        static_cast<void>(elizaos::DreamGenProvider::generate(
            "prompt", "", secret, elizaos::DreamGenProvider::MODEL_LARGE,
            1, 0.5, "", leaky));
    });
    EXPECT_EQ(error, "HTTP transport failed");
    EXPECT_EQ(error.find(secret), std::string::npos);
    EXPECT_EQ(leaky->calls(), 1U);
}

TEST(DgenKoboldProvider, ConcurrentAndReentrantFakeTransportIsSafe) {
    std::weak_ptr<FakeTransport> weak_transport;
    std::atomic<bool> reentered{false};
    auto transport = std::make_shared<FakeTransport>([&](const HttpRequest& request) {
        if (!reentered.exchange(true)) {
            EXPECT_TRUE(elizaos::KoboldCppProvider::is_available(
                "http://localhost:5001", weak_transport.lock()));
        }
        if (request.method == elizaos::HttpMethod::Get) {
            return jsonResponse({{"result", "model"}});
        }
        return chatResponse(nlohmann::json::parse(request.body)["messages"].back()["content"]);
    });
    weak_transport = transport;

    constexpr std::size_t kThreads = 16U;
    std::vector<std::thread> threads;
    std::vector<std::string> results(kThreads);
    threads.reserve(kThreads);
    for (std::size_t index = 0U; index < kThreads; ++index) {
        threads.emplace_back([&, index] {
            results[index] = elizaos::KoboldCppProvider::generate(
                "p" + std::to_string(index), "", "http://localhost:5001",
                3, 0.5, transport);
        });
    }
    for (auto& thread : threads) thread.join();
    for (std::size_t index = 0U; index < kThreads; ++index) {
        EXPECT_EQ(results[index], "p" + std::to_string(index));
    }
    EXPECT_EQ(transport->calls(), kThreads + 1U);
}

TEST(DgenKoboldProvider, PluginMetadataMakesNoRegistrationClaim) {
    EXPECT_EQ(elizaos::DgenKoboldPlugin::getName(), "dgen_kobold_provider");
    const std::string description = elizaos::DgenKoboldPlugin::getDescription();
    EXPECT_NE(description.find("utility"), std::string::npos);
    EXPECT_EQ(description.find("register"), std::string::npos);
}


TEST(DgenKoboldProvider, LibcurlFactoryBehaviorMatchesTheBuild) {
    if (elizaos::HttpClient::libcurlAvailable()) {
        EXPECT_NE(elizaos::HttpClient::createLibcurlTransport(), nullptr);
    } else {
        EXPECT_THROW(static_cast<void>(elizaos::HttpClient::createLibcurlTransport()),
                     elizaos::TransportError);
    }
}

TEST(DgenKoboldProvider, RejectsInvalidTimeoutAndCredentialConfiguration) {
    EXPECT_THROW(static_cast<void>(elizaos::HttpClient(
                     {}, {std::chrono::milliseconds(0),
                          std::chrono::milliseconds(10), 1024U})),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(elizaos::HttpClient(
                     {}, {std::chrono::milliseconds(20),
                          std::chrono::milliseconds(10), 1024U})),
                 std::invalid_argument);
    auto transport = std::make_shared<FakeTransport>([](const HttpRequest&) {
        return chatResponse("unused");
    });
    EXPECT_THROW(static_cast<void>(elizaos::DreamGenProvider::generate(
                     "prompt", "", "bad\ncredential",
                     elizaos::DreamGenProvider::MODEL_LARGE, 1, 0.5, "",
                     transport)),
                 std::invalid_argument);
    EXPECT_EQ(transport->calls(), 0U);
}


} // namespace
