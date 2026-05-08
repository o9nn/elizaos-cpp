// mcp_gateway_test.cpp - E2E tests for MCPGateway / MCPClient.
#include <gtest/gtest.h>
#include "elizaos/mcp_gateway.hpp"

using namespace elizaos;

class MCPGatewayTest : public ::testing::Test {
protected:
    MCPGatewayTest() : gw("test-gateway") {}
    MCPGateway gw;
};

TEST_F(MCPGatewayTest, AddRemoveServer) {
    MCPServerConfig cfg;
    cfg.name = "srv-1";
    cfg.transport = "stdio";
    cfg.endpoint = "echo";
    EXPECT_NO_THROW(gw.addServer(cfg));
    EXPECT_GE(gw.listServers().size(), 1u);
    EXPECT_NO_THROW(gw.removeServer("srv-1"));
}

TEST_F(MCPGatewayTest, RegisterAndListTool) {
    MCPTool t;
    t.name = "echo";
    t.namespace_ = "test";
    t.description = "echo";
    t.handler = [](const MCPJsonValue& in) { return in; };
    EXPECT_NO_THROW(gw.registerTool(t));
    EXPECT_GE(gw.listTools().size(), 1u);
    EXPECT_NO_THROW(gw.unregisterTool("echo"));
}

TEST_F(MCPGatewayTest, ListByNamespace) {
    MCPTool t;
    t.name = "ns_tool";
    t.namespace_ = "ns";
    t.handler = [](const MCPJsonValue& in) { return in; };
    gw.registerTool(t);
    auto ns_tools = gw.listToolsByNamespace("ns");
    EXPECT_GE(ns_tools.size(), 1u);
}

TEST_F(MCPGatewayTest, ResourceLifecycle) {
    MCPResource r;
    r.uri = "res://x";
    r.namespace_ = "ns";
    r.mimeType = "text/plain";
    EXPECT_NO_THROW(gw.registerResource(r));
    EXPECT_GE(gw.listResources().size(), 1u);
    EXPECT_NO_THROW(gw.unregisterResource("res://x"));
}

TEST_F(MCPGatewayTest, APIKeyManagement) {
    APIKeyTier t;
    t.tierName = "free";
    t.rateLimit = 60;
    EXPECT_NO_THROW(gw.createAPIKey("k1", t));
    EXPECT_TRUE(gw.validateAPIKey("k1"));
    EXPECT_NO_THROW(gw.revokeAPIKey("k1"));
}

TEST_F(MCPGatewayTest, PaymentToggle) {
    PaymentConfig c;
    c.enabled = true;
    c.pricePerCall = 0.01f;
    EXPECT_NO_THROW(gw.enablePayments(c));
    EXPECT_NO_THROW(gw.disablePayments());
}

TEST_F(MCPGatewayTest, ConfigurationKnobs) {
    EXPECT_NO_THROW(gw.setNamespacing(true));
    EXPECT_NO_THROW(gw.setConflictResolution("namespace"));
    EXPECT_NO_THROW(gw.setRateLimit(120));
}

TEST_F(MCPGatewayTest, StatisticsAvailable) {
    auto s = gw.getStatistics();
    EXPECT_GE(s.totalRequests, 0);
}

TEST(MCPClient, ConstructionNoCrash) {
    MCPClient c("http://localhost:9999", "key");
    SUCCEED();
}
