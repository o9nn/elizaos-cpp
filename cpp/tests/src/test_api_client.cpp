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

} // namespace
