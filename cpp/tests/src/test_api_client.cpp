#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "client.hpp"
#include "lib/base-client.hpp"
#include "services/server.hpp"

namespace {

using elizaos::eliza_api_client::ApiResult;
using elizaos::eliza_api_client::BaseClient;
using elizaos::eliza_api_client::Client;
using elizaos::eliza_api_client::HttpRequest;
using elizaos::eliza_api_client::HttpResponse;
using elizaos::eliza_api_client::IHttpTransport;
using elizaos::eliza_api_client::Server;

class FakeTransport final : public IHttpTransport {
public:
    explicit FakeTransport(bool ready = true)
        : ready_(ready) {}

    ApiResult send(const HttpRequest& request) override {
        requests.push_back(request);
        HttpResponse response;
        response.statusCode = statusCode;
        response.body = responseBody;
        response.jsonBody = responseJson;
        return ApiResult::success(response);
    }

    bool isReady() const override { return ready_; }
    std::string getName() const override { return "fake_transport"; }

    bool ready_ = true;
    int statusCode = 200;
    std::string responseBody = R"({"ok":true})";
    nlohmann::json responseJson = {{"ok", true}};
    std::vector<HttpRequest> requests;
};

nlohmann::json validConfig() {
    return {
        {"baseUrl", "https://eliza.example.test/"},
        {"timeoutMs", 1500},
        {"maxRetries", 1}
    };
}

TEST(ApiClientBaseClientTest, EmptyConfigDoesNotInitialize) {
    BaseClient client;

    EXPECT_FALSE(client.initialize(nlohmann::json::object()));
    EXPECT_FALSE(client.isInitialized());
    EXPECT_FALSE(client.isConfigured());
    EXPECT_EQ(client.getLastErrorCode(), "base_url_missing");

    const auto status = client.getStatus();
    EXPECT_FALSE(status.at("initialized").get<bool>());
    EXPECT_FALSE(status.at("configured").get<bool>());
}

TEST(ApiClientBaseClientTest, ValidConfigInitializesButReportsMissingTransport) {
    BaseClient client;

    ASSERT_TRUE(client.initialize(validConfig()));
    EXPECT_TRUE(client.isInitialized());
    EXPECT_TRUE(client.isConfigured());
    EXPECT_EQ(client.getBaseUrl(), "https://eliza.example.test");
    EXPECT_FALSE(client.isTransportReady());

    const auto status = client.getStatus();
    EXPECT_TRUE(status.at("initialized").get<bool>());
    EXPECT_TRUE(status.at("configured").get<bool>());
    EXPECT_FALSE(status.at("transportConfigured").get<bool>());
    EXPECT_FALSE(status.at("transportReady").get<bool>());
}

TEST(ApiClientBaseClientTest, RequestWithoutTransportFailsTruthfully) {
    BaseClient client;
    ASSERT_TRUE(client.initialize(validConfig()));

    const ApiResult result = client.request("GET", "/api/server/health");

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.errorCode, "transport_missing");
    EXPECT_EQ(result.response.statusCode, 0);
}

TEST(ApiClientBaseClientTest, FakeTransportReceivesNormalizedRequestAndReturnsApiResult) {
    auto transport = std::make_shared<FakeTransport>();
    BaseClient client(transport);
    ASSERT_TRUE(client.initialize(validConfig()));

    const nlohmann::json body = {{"message", "hello"}};
    const ApiResult result = client.request("POST", "api/messages", body, {{"X-Test", "yes"}});

    ASSERT_TRUE(result.ok);
    EXPECT_EQ(result.response.statusCode, 200);
    EXPECT_EQ(result.response.jsonBody.at("ok"), true);
    ASSERT_EQ(transport->requests.size(), 1U);
    EXPECT_EQ(transport->requests.front().method, "POST");
    EXPECT_EQ(transport->requests.front().path, "api/messages");
    EXPECT_EQ(transport->requests.front().url, "https://eliza.example.test/api/messages");
    EXPECT_EQ(transport->requests.front().body.at("message"), "hello");
    EXPECT_EQ(transport->requests.front().headers.at("X-Test"), "yes");
}

TEST(ApiClientServerServiceTest, HealthUsesBaseClientTransportPath) {
    auto transport = std::make_shared<FakeTransport>();
    auto client = std::make_shared<BaseClient>(transport);
    ASSERT_TRUE(client->initialize(validConfig()));

    Server server(client);
    ASSERT_TRUE(server.initialize(nlohmann::json::object()));

    const ApiResult result = server.getHealth();

    ASSERT_TRUE(result.ok);
    ASSERT_EQ(transport->requests.size(), 1U);
    EXPECT_EQ(transport->requests.front().method, "GET");
    EXPECT_EQ(transport->requests.front().path, "/api/server/health");
    EXPECT_EQ(transport->requests.front().url, "https://eliza.example.test/api/server/health");
}

TEST(ApiClientServerServiceTest, ServerStatusUsesConfigurableStatusPath) {
    auto transport = std::make_shared<FakeTransport>();
    auto client = std::make_shared<BaseClient>(transport);
    ASSERT_TRUE(client->initialize(validConfig()));

    Server server(client);
    ASSERT_TRUE(server.initialize({{"statusPath", "/custom/status"}, {"healthPath", "/custom/health"}}));

    const ApiResult result = server.getServerStatus();

    ASSERT_TRUE(result.ok);
    ASSERT_EQ(transport->requests.size(), 1U);
    EXPECT_EQ(transport->requests.front().method, "GET");
    EXPECT_EQ(transport->requests.front().path, "/custom/status");
    EXPECT_EQ(transport->requests.front().url, "https://eliza.example.test/custom/status");

    const auto status = server.getStatus();
    EXPECT_TRUE(status.at("initialized").get<bool>());
    EXPECT_EQ(status.at("healthPath"), "/custom/health");
    EXPECT_EQ(status.at("statusPath"), "/custom/status");
    EXPECT_TRUE(status.at("clientConfigured").get<bool>());
}

TEST(ApiClientClientBoundaryTest, InitializesWithCanonicalBaseUrlAndStatusSnapshot) {
    Client client;

    nlohmann::json config = {
        {"baseUrl", "https://agent.example.test/api/"},
        {"timeoutMs", 12000},
        {"headers", {
            {"X-Agent", "eliza"},
            {"X-Number-Ignored", 7}
        }}
    };

    ASSERT_TRUE(client.initialize(config));
    EXPECT_TRUE(client.isInitialized());
    EXPECT_EQ(client.getBaseUrl(), "https://agent.example.test/api");
    EXPECT_EQ(client.getTimeoutMs(), 12000);
    ASSERT_EQ(client.getDefaultHeaders().size(), 1u);
    EXPECT_EQ(client.getDefaultHeaders().at("X-Agent"), "eliza");

    const auto status = client.getStatus();
    EXPECT_EQ(status.at("name"), "client");
    EXPECT_EQ(status.at("initialized"), true);
    EXPECT_EQ(status.at("baseUrl"), "https://agent.example.test/api");
    EXPECT_EQ(status.at("timeoutMs"), 12000);
    EXPECT_EQ(status.at("headerCount"), 1u);
    EXPECT_EQ(status.at("defaultHeaders").at("X-Agent"), "eliza");
}

TEST(ApiClientClientBoundaryTest, ResolvesRelativeRootQueryAndAbsoluteEndpoints) {
    Client client;
    ASSERT_TRUE(client.initialize({{"base_url", "http://localhost:3000///"}}));

    EXPECT_EQ(client.resolveEndpoint(""), "http://localhost:3000");
    EXPECT_EQ(client.resolveEndpoint("/"), "http://localhost:3000");
    EXPECT_EQ(client.resolveEndpoint("agents"), "http://localhost:3000/agents");
    EXPECT_EQ(client.resolveEndpoint("/agents/42"), "http://localhost:3000/agents/42");
    EXPECT_EQ(client.resolveEndpoint("?health=true"), "http://localhost:3000?health=true");
    EXPECT_EQ(client.resolveEndpoint("https://override.example/status"),
              "https://override.example/status");
}

TEST(ApiClientClientBoundaryTest, RedactsAuthMaterialFromStatus) {
    Client client;
    ASSERT_TRUE(client.initialize({
        {"baseUrl", "https://secure.example.test"},
        {"apiKey", "sk-secret-value"},
        {"bearer_token", "bearer-secret-value"}
    }));

    EXPECT_TRUE(client.hasApiKey());
    EXPECT_TRUE(client.hasBearerToken());
    EXPECT_TRUE(client.hasAuth());

    const std::string status_dump = client.getStatus().dump();
    EXPECT_NE(status_dump.find("\"hasApiKey\":true"), std::string::npos);
    EXPECT_NE(status_dump.find("\"hasBearerToken\":true"), std::string::npos);
    EXPECT_EQ(status_dump.find("sk-secret-value"), std::string::npos);
    EXPECT_EQ(status_dump.find("bearer-secret-value"), std::string::npos);
}

TEST(ApiClientClientBoundaryTest, RejectsMissingInvalidBaseUrlAndInvalidTimeout) {
    Client client;

    EXPECT_THROW(client.initialize({}), std::invalid_argument);
    EXPECT_THROW(client.initialize({{"baseUrl", ""}}), std::invalid_argument);
    EXPECT_THROW(client.initialize({{"baseUrl", "localhost:3000"}}), std::invalid_argument);
    EXPECT_THROW(client.initialize({{"baseUrl", "ftp://example.test"}}), std::invalid_argument);
    EXPECT_THROW(client.initialize({{"baseUrl", "https://example.test"}, {"timeoutMs", 0}}),
                 std::invalid_argument);
    EXPECT_THROW(client.initialize({{"baseUrl", "https://example.test"}, {"timeoutMs", "slow"}}),
                 std::invalid_argument);

    EXPECT_FALSE(client.isInitialized());
    EXPECT_EQ(client.getStatus().at("initialized"), false);
    EXPECT_TRUE(client.getStatus().at("baseUrl").is_null());
}

TEST(ApiClientClientBoundaryTest, ShutdownClearsDerivedRuntimeState) {
    Client client;
    ASSERT_TRUE(client.initialize({
        {"baseUrl", "https://agent.example.test"},
        {"api_key", "secret"},
        {"timeout_ms", 5000},
        {"headers", {{"X-Agent", "eliza"}}}
    }));

    client.shutdown();

    EXPECT_FALSE(client.isInitialized());
    EXPECT_EQ(client.getBaseUrl(), "");
    EXPECT_EQ(client.getTimeoutMs(), 30000);
    EXPECT_FALSE(client.hasAuth());
    EXPECT_TRUE(client.getDefaultHeaders().empty());
    EXPECT_THROW(client.resolveEndpoint("agents"), std::logic_error);
}

TEST(ApiClientClientBoundaryTest, InitializeIsIdempotentAfterSuccessfulInitialization) {
    Client client;
    ASSERT_TRUE(client.initialize({{"baseUrl", "https://first.example.test"}}));
    ASSERT_TRUE(client.initialize({{"baseUrl", "https://second.example.test"}}));

    EXPECT_EQ(client.getBaseUrl(), "https://first.example.test");
    EXPECT_EQ(client.resolveEndpoint("status"), "https://first.example.test/status");
}

TEST(ApiClientRepairedServicesTest, AudioServiceValidatesAndDispatchesConfiguredEndpoints) {
    auto transport = std::make_shared<FakeTransport>();
    Client client;
    ASSERT_TRUE(client.initialize({
        {"baseUrl", "https://agent.example.test"},
        {"speechPath", "/v1/audio/speech/"},
        {"transcription_path", "/v1/audio/transcriptions/"},
        {"voicesPath", "/v1/audio/voices/"}
    }));
    client.setTransport(transport);

    auto& audio = client.audio();
    EXPECT_TRUE(audio.isInitialized());
    EXPECT_FALSE(audio.synthesizeSpeech({{"voice", "eliza"}}).ok);
    EXPECT_EQ(audio.synthesizeSpeech({{"voice", "eliza"}}).errorCode, "missing_speech_text");
    EXPECT_FALSE(audio.transcribeAudio({{"language", "en"}}).ok);
    EXPECT_EQ(audio.transcribeAudio({{"language", "en"}}).errorCode, "missing_audio_source");

    ASSERT_TRUE(audio.synthesizeSpeech({{"text", "hello"}, {"voice", "eliza"}}).ok);
    ASSERT_TRUE(audio.transcribeAudio({{"audioUrl", "s3://bucket/audio.wav"}}).ok);
    ASSERT_TRUE(audio.listVoices().ok);

    ASSERT_EQ(transport->requests.size(), 3U);
    EXPECT_EQ(transport->requests[0].method, "POST");
    EXPECT_EQ(transport->requests[0].path, "/v1/audio/speech");
    EXPECT_EQ(transport->requests[1].method, "POST");
    EXPECT_EQ(transport->requests[1].path, "/v1/audio/transcriptions");
    EXPECT_EQ(transport->requests[2].method, "GET");
    EXPECT_EQ(transport->requests[2].path, "/v1/audio/voices");

    const auto status = client.getStatus();
    ASSERT_TRUE(status.at("services").contains("audio"));
    EXPECT_EQ(status.at("services").at("audio").at("speechPath"), "/v1/audio/speech");
}

TEST(ApiClientRepairedServicesTest, MediaServiceCoversUploadListGetAndDelete) {
    auto transport = std::make_shared<FakeTransport>();
    Client client;
    ASSERT_TRUE(client.initialize({{"baseUrl", "https://agent.example.test"}, {"media_path", "/v1/media/"}}));
    client.setTransport(transport);

    auto& media = client.media();
    EXPECT_FALSE(media.uploadMedia(nlohmann::json::object()).ok);
    EXPECT_EQ(media.uploadMedia(nlohmann::json::object()).errorCode, "empty_media_descriptor");
    EXPECT_FALSE(media.getMedia("").ok);
    EXPECT_EQ(media.getMedia("").errorCode, "missing_media_id");

    ASSERT_TRUE(media.uploadMedia({{"url", "s3://bucket/image.png"}, {"kind", "image"}}).ok);
    ASSERT_TRUE(media.listMedia().ok);
    ASSERT_TRUE(media.getMedia("media-42").ok);
    ASSERT_TRUE(media.deleteMedia("media-42").ok);

    ASSERT_EQ(transport->requests.size(), 4U);
    EXPECT_EQ(transport->requests[0].method, "POST");
    EXPECT_EQ(transport->requests[0].path, "/v1/media");
    EXPECT_EQ(transport->requests[1].method, "GET");
    EXPECT_EQ(transport->requests[1].path, "/v1/media");
    EXPECT_EQ(transport->requests[2].method, "GET");
    EXPECT_EQ(transport->requests[2].path, "/v1/media/media-42");
    EXPECT_EQ(transport->requests[3].method, "DELETE");
    EXPECT_EQ(transport->requests[3].path, "/v1/media/media-42");
}

TEST(ApiClientRepairedServicesTest, MemoryServiceCoversLifecycleAndSearchValidation) {
    auto transport = std::make_shared<FakeTransport>();
    Client client;
    ASSERT_TRUE(client.initialize({
        {"baseUrl", "https://agent.example.test"},
        {"memoryPath", "/v1/memory/"},
        {"memory_search_path", "/v1/memory/query/"}
    }));
    client.setTransport(transport);

    auto& memory = client.memory();
    EXPECT_FALSE(memory.createMemory(nlohmann::json::object()).ok);
    EXPECT_EQ(memory.createMemory(nlohmann::json::object()).errorCode, "invalid_memory");
    EXPECT_FALSE(memory.searchMemories({{"limit", 3}}).ok);
    EXPECT_EQ(memory.searchMemories({{"limit", 3}}).errorCode, "invalid_memory_query");
    EXPECT_FALSE(memory.deleteMemory("").ok);
    EXPECT_EQ(memory.deleteMemory("").errorCode, "missing_memory_id");

    ASSERT_TRUE(memory.createMemory({{"id", "m1"}, {"text", "Dan prefers real functionality"}}).ok);
    ASSERT_TRUE(memory.listMemories().ok);
    ASSERT_TRUE(memory.getMemory("m1").ok);
    ASSERT_TRUE(memory.searchMemories({{"query", "real functionality"}, {"limit", 5}}).ok);
    ASSERT_TRUE(memory.deleteMemory("m1").ok);

    ASSERT_EQ(transport->requests.size(), 5U);
    EXPECT_EQ(transport->requests[0].method, "POST");
    EXPECT_EQ(transport->requests[0].path, "/v1/memory");
    EXPECT_EQ(transport->requests[1].method, "GET");
    EXPECT_EQ(transport->requests[1].path, "/v1/memory");
    EXPECT_EQ(transport->requests[2].method, "GET");
    EXPECT_EQ(transport->requests[2].path, "/v1/memory/m1");
    EXPECT_EQ(transport->requests[3].method, "POST");
    EXPECT_EQ(transport->requests[3].path, "/v1/memory/query");
    EXPECT_EQ(transport->requests[4].method, "DELETE");
    EXPECT_EQ(transport->requests[4].path, "/v1/memory/m1");
}

TEST(ApiClientRepairedServicesTest, MessagingServiceCoversConversationAndReadPaths) {
    auto transport = std::make_shared<FakeTransport>();
    Client client;
    ASSERT_TRUE(client.initialize({
        {"baseUrl", "https://agent.example.test"},
        {"messagesPath", "/v1/messages/"},
        {"conversations_path", "/v1/conversations/"}
    }));
    client.setTransport(transport);

    auto& messaging = client.messaging();
    EXPECT_FALSE(messaging.sendMessage({{"channel", "room-1"}}).ok);
    EXPECT_EQ(messaging.sendMessage({{"channel", "room-1"}}).errorCode, "invalid_message");
    EXPECT_FALSE(messaging.getConversation("").ok);
    EXPECT_EQ(messaging.getConversation("").errorCode, "missing_conversation_id");
    EXPECT_FALSE(messaging.markMessageRead("").ok);
    EXPECT_EQ(messaging.markMessageRead("").errorCode, "missing_message_id");

    ASSERT_TRUE(messaging.sendMessage({{"text", "hello"}, {"conversationId", "c1"}}).ok);
    ASSERT_TRUE(messaging.listMessages().ok);
    ASSERT_TRUE(messaging.getConversation("c1").ok);
    ASSERT_TRUE(messaging.markMessageRead("msg-1").ok);
    ASSERT_TRUE(messaging.deleteMessage("msg-1").ok);

    ASSERT_EQ(transport->requests.size(), 5U);
    EXPECT_EQ(transport->requests[0].method, "POST");
    EXPECT_EQ(transport->requests[0].path, "/v1/messages");
    EXPECT_EQ(transport->requests[1].method, "GET");
    EXPECT_EQ(transport->requests[1].path, "/v1/messages");
    EXPECT_EQ(transport->requests[2].method, "GET");
    EXPECT_EQ(transport->requests[2].path, "/v1/conversations/c1");
    EXPECT_EQ(transport->requests[3].method, "POST");
    EXPECT_EQ(transport->requests[3].path, "/v1/messages/msg-1/read");
    EXPECT_TRUE(transport->requests[3].body.is_object());
    EXPECT_EQ(transport->requests[4].method, "DELETE");
    EXPECT_EQ(transport->requests[4].path, "/v1/messages/msg-1");
}

TEST(ApiClientRepairedServicesTest, ClientShutdownResetsAllLazyRepairedServices) {
    auto transport = std::make_shared<FakeTransport>();
    Client client;
    ASSERT_TRUE(client.initialize(validConfig()));
    client.setTransport(transport);

    EXPECT_TRUE(client.audio().listVoices().ok);
    EXPECT_TRUE(client.media().listMedia().ok);
    EXPECT_TRUE(client.memory().listMemories().ok);
    EXPECT_TRUE(client.messaging().listMessages().ok);
    ASSERT_TRUE(client.getStatus().at("services").contains("audio"));
    ASSERT_TRUE(client.getStatus().at("services").contains("media"));
    ASSERT_TRUE(client.getStatus().at("services").contains("memory"));
    ASSERT_TRUE(client.getStatus().at("services").contains("messaging"));

    client.shutdown();

    EXPECT_FALSE(client.isInitialized());
    EXPECT_FALSE(client.getStatus().contains("services"));
    EXPECT_THROW(client.audio(), std::logic_error);
}

} // namespace
