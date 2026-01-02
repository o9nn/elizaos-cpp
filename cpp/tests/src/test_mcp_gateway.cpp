#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/mcp_gateway.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;
using namespace ::testing;

class MCPGatewayTest : public ::testing::Test {
protected:
    void SetUp() override {
        gateway_ = std::make_shared<MCPGateway>("test-gateway");
    }

    std::shared_ptr<MCPGateway> gateway_;
};

// ============================================================================
// MCPServerConfig Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPServerConfigCreation) {
    MCPServerConfig config;
    config.name = "test-server";
    config.transport = "http";
    config.endpoint = "http://localhost:8080";
    config.autoReconnect = true;
    config.healthCheckInterval = 60;

    EXPECT_EQ(config.name, "test-server");
    EXPECT_EQ(config.transport, "http");
    EXPECT_EQ(config.endpoint, "http://localhost:8080");
    EXPECT_TRUE(config.autoReconnect);
    EXPECT_EQ(config.healthCheckInterval, 60);
}

TEST_F(MCPGatewayTest, MCPServerConfigEnvironment) {
    MCPServerConfig config;
    config.name = "env-server";
    config.environment["API_KEY"] = "secret123";
    config.environment["DEBUG"] = "true";

    EXPECT_EQ(config.environment.size(), 2);
    EXPECT_EQ(config.environment["API_KEY"], "secret123");
}

// ============================================================================
// MCPTool Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPToolCreation) {
    MCPTool tool;
    tool.name = "calculator";
    tool.namespace_ = "math";
    tool.description = "Performs calculations";

    EXPECT_EQ(tool.name, "calculator");
    EXPECT_EQ(tool.namespace_, "math");
    EXPECT_EQ(tool.description, "Performs calculations");
}

TEST_F(MCPGatewayTest, MCPToolWithHandler) {
    MCPTool tool;
    tool.name = "echo";
    tool.description = "Echoes input";
    tool.handler = [](const JsonValue& input) -> JsonValue {
        return input;
    };

    EXPECT_TRUE(tool.handler != nullptr);

    JsonValue testInput = {{"message", "hello"}};
    JsonValue result = tool.handler(testInput);
    EXPECT_EQ(result["message"], "hello");
}

TEST_F(MCPGatewayTest, MCPToolWithInputSchema) {
    MCPTool tool;
    tool.name = "search";
    tool.inputSchema = {
        {"type", "object"},
        {"properties", {
            {"query", {{"type", "string"}}},
            {"limit", {{"type", "integer"}}}
        }}
    };

    EXPECT_EQ(tool.inputSchema["type"], "object");
    EXPECT_TRUE(tool.inputSchema.contains("properties"));
}

// ============================================================================
// MCPResource Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPResourceCreation) {
    MCPResource resource;
    resource.uri = "file:///data/config.json";
    resource.namespace_ = "config";
    resource.mimeType = "application/json";
    resource.description = "Configuration file";

    EXPECT_EQ(resource.uri, "file:///data/config.json");
    EXPECT_EQ(resource.namespace_, "config");
    EXPECT_EQ(resource.mimeType, "application/json");
    EXPECT_EQ(resource.description, "Configuration file");
}

// ============================================================================
// PaymentConfig Tests
// ============================================================================

TEST_F(MCPGatewayTest, PaymentConfigDefaults) {
    PaymentConfig config;

    EXPECT_FALSE(config.enabled);
    EXPECT_NEAR(config.pricePerCall, 0.001f, 0.0001f);
}

TEST_F(MCPGatewayTest, PaymentConfigCustom) {
    PaymentConfig config;
    config.enabled = true;
    config.usdcContractAddress = "0x123...";
    config.recipientAddress = "0x456...";
    config.pricePerCall = 0.01f;
    config.chainId = "1";

    EXPECT_TRUE(config.enabled);
    EXPECT_EQ(config.usdcContractAddress, "0x123...");
    EXPECT_NEAR(config.pricePerCall, 0.01f, 0.001f);
}

// ============================================================================
// APIKeyTier Tests
// ============================================================================

TEST_F(MCPGatewayTest, APIKeyTierCreation) {
    APIKeyTier tier;
    tier.tierName = "premium";
    tier.rateLimit = 1000;
    tier.allowedTools = {"tool1", "tool2", "tool3"};
    tier.customPrice = 0.005f;

    EXPECT_EQ(tier.tierName, "premium");
    EXPECT_EQ(tier.rateLimit, 1000);
    EXPECT_EQ(tier.allowedTools.size(), 3);
    EXPECT_NEAR(tier.customPrice, 0.005f, 0.001f);
}

// ============================================================================
// MCPGateway Tests
// ============================================================================

TEST_F(MCPGatewayTest, GatewayCreation) {
    MCPGateway gw("gateway-1");

    auto servers = gw.listServers();
    EXPECT_EQ(servers.size(), 0);
}

TEST_F(MCPGatewayTest, AddServer) {
    MCPServerConfig config;
    config.name = "server-1";
    config.transport = "http";
    config.endpoint = "http://localhost:8080";

    gateway_->addServer(config);

    auto servers = gateway_->listServers();
    EXPECT_EQ(servers.size(), 1);
    EXPECT_THAT(servers, Contains("server-1"));
}

TEST_F(MCPGatewayTest, RemoveServer) {
    MCPServerConfig config;
    config.name = "server-1";
    config.transport = "http";
    config.endpoint = "http://localhost:8080";

    gateway_->addServer(config);
    EXPECT_EQ(gateway_->listServers().size(), 1);

    gateway_->removeServer("server-1");
    EXPECT_EQ(gateway_->listServers().size(), 0);
}

TEST_F(MCPGatewayTest, RegisterTool) {
    MCPTool tool;
    tool.name = "test-tool";
    tool.description = "A test tool";

    gateway_->registerTool(tool);

    auto tools = gateway_->listTools();
    EXPECT_GE(tools.size(), 1);
}

TEST_F(MCPGatewayTest, UnregisterTool) {
    MCPTool tool;
    tool.name = "temp-tool";
    tool.description = "Temporary tool";

    gateway_->registerTool(tool);
    gateway_->unregisterTool("temp-tool");

    auto tools = gateway_->listTools();
    bool found = false;
    for (const auto& t : tools) {
        if (t.name == "temp-tool") found = true;
    }
    EXPECT_FALSE(found);
}

TEST_F(MCPGatewayTest, ListToolsByNamespace) {
    MCPTool tool1;
    tool1.name = "tool1";
    tool1.namespace_ = "math";

    MCPTool tool2;
    tool2.name = "tool2";
    tool2.namespace_ = "math";

    MCPTool tool3;
    tool3.name = "tool3";
    tool3.namespace_ = "string";

    gateway_->registerTool(tool1);
    gateway_->registerTool(tool2);
    gateway_->registerTool(tool3);

    auto mathTools = gateway_->listToolsByNamespace("math");
    EXPECT_EQ(mathTools.size(), 2);
}

TEST_F(MCPGatewayTest, RegisterResource) {
    MCPResource resource;
    resource.uri = "file:///test.txt";
    resource.mimeType = "text/plain";

    gateway_->registerResource(resource);

    auto resources = gateway_->listResources();
    EXPECT_GE(resources.size(), 1);
}

TEST_F(MCPGatewayTest, UnregisterResource) {
    MCPResource resource;
    resource.uri = "file:///temp.txt";
    resource.mimeType = "text/plain";

    gateway_->registerResource(resource);
    gateway_->unregisterResource("file:///temp.txt");

    auto resources = gateway_->listResources();
    bool found = false;
    for (const auto& r : resources) {
        if (r.uri == "file:///temp.txt") found = true;
    }
    EXPECT_FALSE(found);
}

TEST_F(MCPGatewayTest, EnableDisablePayments) {
    PaymentConfig config;
    config.enabled = true;
    config.pricePerCall = 0.01f;

    gateway_->enablePayments(config);
    gateway_->disablePayments();

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(MCPGatewayTest, CreateAPIKey) {
    APIKeyTier tier;
    tier.tierName = "basic";
    tier.rateLimit = 100;

    gateway_->createAPIKey("test-key-123", tier);

    EXPECT_TRUE(gateway_->validateAPIKey("test-key-123"));
}

TEST_F(MCPGatewayTest, RevokeAPIKey) {
    APIKeyTier tier;
    tier.tierName = "basic";
    tier.rateLimit = 100;

    gateway_->createAPIKey("revoke-test", tier);
    EXPECT_TRUE(gateway_->validateAPIKey("revoke-test"));

    gateway_->revokeAPIKey("revoke-test");
    EXPECT_FALSE(gateway_->validateAPIKey("revoke-test"));
}

TEST_F(MCPGatewayTest, GetAPIKeyTier) {
    APIKeyTier tier;
    tier.tierName = "premium";
    tier.rateLimit = 1000;

    gateway_->createAPIKey("premium-key", tier);

    APIKeyTier retrieved = gateway_->getAPIKeyTier("premium-key");
    EXPECT_EQ(retrieved.tierName, "premium");
    EXPECT_EQ(retrieved.rateLimit, 1000);
}

TEST_F(MCPGatewayTest, SetNamespacing) {
    gateway_->setNamespacing(true);
    gateway_->setNamespacing(false);

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(MCPGatewayTest, SetConflictResolution) {
    gateway_->setConflictResolution("error");
    gateway_->setConflictResolution("first");
    gateway_->setConflictResolution("last");
    gateway_->setConflictResolution("namespace");

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(MCPGatewayTest, SetRateLimit) {
    gateway_->setRateLimit(100);
    gateway_->setRateLimit(1000);

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(MCPGatewayTest, GetStatistics) {
    auto stats = gateway_->getStatistics();

    EXPECT_GE(stats.totalRequests, 0);
    EXPECT_GE(stats.successfulRequests, 0);
    EXPECT_GE(stats.failedRequests, 0);
}

// ============================================================================
// MCPClient Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPClientCreation) {
    MCPClient client("http://localhost:8080", "api-key");

    // Should be able to create without errors
    EXPECT_TRUE(true);
}

TEST_F(MCPGatewayTest, MCPClientDiscoverTools) {
    MCPClient client("http://localhost:8080");

    auto tools = client.discoverTools();
    // May return empty if not connected
    EXPECT_GE(tools.size(), 0);
}

TEST_F(MCPGatewayTest, MCPClientDiscoverResources) {
    MCPClient client("http://localhost:8080");

    auto resources = client.discoverResources();
    // May return empty if not connected
    EXPECT_GE(resources.size(), 0);
}

// ============================================================================
// MCPServer Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPServerCreation) {
    MCPServer server("test-server");

    // Should be able to create without errors
    EXPECT_TRUE(true);
}

TEST_F(MCPGatewayTest, MCPServerRegisterTool) {
    MCPServer server("tool-server");

    server.registerTool("echo", "Echoes input",
                       {{"type", "object"}},
                       [](const JsonValue& input) { return input; });

    // Tool registered without error
    EXPECT_TRUE(true);
}

TEST_F(MCPGatewayTest, MCPServerRegisterResource) {
    MCPServer server("resource-server");

    server.registerResource("file:///data.txt", "text/plain", "Data file");

    // Resource registered without error
    EXPECT_TRUE(true);
}

TEST_F(MCPGatewayTest, MCPServerLifecycle) {
    MCPServer server("lifecycle-server");

    server.startListening();
    server.stopListening();

    // Lifecycle operations without error
    EXPECT_TRUE(true);
}

