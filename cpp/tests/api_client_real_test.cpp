#include <gtest/gtest.h>

#include "client.hpp"

#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

using elizaos::eliza_api_client::Client;

TEST(ApiClientRealTest, InitializesWithCanonicalBaseUrlAndStatusSnapshot) {
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

TEST(ApiClientRealTest, ResolvesRelativeRootQueryAndAbsoluteEndpoints) {
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

TEST(ApiClientRealTest, RedactsAuthMaterialFromStatus) {
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

TEST(ApiClientRealTest, RejectsMissingInvalidBaseUrlAndInvalidTimeout) {
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

TEST(ApiClientRealTest, ShutdownClearsDerivedRuntimeState) {
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

TEST(ApiClientRealTest, InitializeIsIdempotentAfterSuccessfulInitialization) {
    Client client;
    ASSERT_TRUE(client.initialize({{"baseUrl", "https://first.example.test"}}));
    ASSERT_TRUE(client.initialize({{"baseUrl", "https://second.example.test"}}));

    EXPECT_EQ(client.getBaseUrl(), "https://first.example.test");
    EXPECT_EQ(client.resolveEndpoint("status"), "https://first.example.test/status");
}
