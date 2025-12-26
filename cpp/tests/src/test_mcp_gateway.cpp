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
        config_.serverAddress = "localhost";
        config_.serverPort = 8080;
        config_.timeout = 30;
        config_.maxConnections = 10;
    }

    MCPConfig config_;
};

// ============================================================================
// MCPConfig Tests
// ============================================================================

TEST_F(MCPGatewayTest, DefaultMCPConfigValues) {
    MCPConfig defaultConfig;

    EXPECT_EQ(defaultConfig.serverAddress, "localhost");
    EXPECT_EQ(defaultConfig.serverPort, 8080);
    EXPECT_EQ(defaultConfig.timeout, 30);
    EXPECT_EQ(defaultConfig.maxConnections, 10);
    EXPECT_TRUE(defaultConfig.enableTLS);
    EXPECT_TRUE(defaultConfig.enableCompression);
}

TEST_F(MCPGatewayTest, CustomMCPConfigValues) {
    config_.serverAddress = "192.168.1.100";
    config_.serverPort = 9090;
    config_.timeout = 60;
    config_.maxConnections = 50;

    EXPECT_EQ(config_.serverAddress, "192.168.1.100");
    EXPECT_EQ(config_.serverPort, 9090);
    EXPECT_EQ(config_.timeout, 60);
    EXPECT_EQ(config_.maxConnections, 50);
}

// ============================================================================
// MCPMessage Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPMessageCreation) {
    MCPMessage msg;
    msg.id = "msg-001";
    msg.type = MCPMessageType::REQUEST;
    msg.method = "tools/call";
    msg.payload = R"({"tool": "calculator", "args": {"a": 1, "b": 2}})";

    EXPECT_EQ(msg.id, "msg-001");
    EXPECT_EQ(msg.type, MCPMessageType::REQUEST);
    EXPECT_EQ(msg.method, "tools/call");
    EXPECT_FALSE(msg.payload.empty());
}

TEST_F(MCPGatewayTest, MCPMessageResponse) {
    MCPMessage msg;
    msg.id = "msg-002";
    msg.type = MCPMessageType::RESPONSE;
    msg.status = MCPStatus::SUCCESS;
    msg.result = R"({"value": 3})";

    EXPECT_EQ(msg.type, MCPMessageType::RESPONSE);
    EXPECT_EQ(msg.status, MCPStatus::SUCCESS);
    EXPECT_EQ(msg.result, R"({"value": 3})");
}

TEST_F(MCPGatewayTest, MCPMessageError) {
    MCPMessage msg;
    msg.id = "msg-003";
    msg.type = MCPMessageType::RESPONSE;
    msg.status = MCPStatus::ERROR;
    msg.error = "Tool not found";
    msg.errorCode = 404;

    EXPECT_EQ(msg.status, MCPStatus::ERROR);
    EXPECT_EQ(msg.error, "Tool not found");
    EXPECT_EQ(msg.errorCode, 404);
}

// ============================================================================
// MCPTool Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPToolCreation) {
    MCPTool tool;
    tool.name = "calculator";
    tool.description = "Performs basic arithmetic operations";
    tool.version = "1.0.0";

    EXPECT_EQ(tool.name, "calculator");
    EXPECT_EQ(tool.description, "Performs basic arithmetic operations");
    EXPECT_EQ(tool.version, "1.0.0");
}

TEST_F(MCPGatewayTest, MCPToolInputSchema) {
    MCPTool tool;
    tool.name = "search";

    MCPInputSchema schema;
    schema.type = "object";
    schema.properties["query"] = {.type = "string", .required = true};
    schema.properties["limit"] = {.type = "integer", .required = false, .defaultValue = "10"};

    tool.inputSchema = schema;

    EXPECT_EQ(tool.inputSchema.type, "object");
    EXPECT_EQ(tool.inputSchema.properties.size(), 2);
    EXPECT_TRUE(tool.inputSchema.properties["query"].required);
}

// ============================================================================
// MCPResource Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPResourceCreation) {
    MCPResource resource;
    resource.uri = "file:///data/config.json";
    resource.name = "config.json";
    resource.mimeType = "application/json";

    EXPECT_EQ(resource.uri, "file:///data/config.json");
    EXPECT_EQ(resource.name, "config.json");
    EXPECT_EQ(resource.mimeType, "application/json");
}

TEST_F(MCPGatewayTest, MCPResourceWithMetadata) {
    MCPResource resource;
    resource.uri = "https://api.example.com/data";
    resource.metadata["size"] = "1024";
    resource.metadata["lastModified"] = "2024-01-15T10:30:00Z";

    EXPECT_EQ(resource.metadata.size(), 2);
    EXPECT_EQ(resource.metadata["size"], "1024");
}

// ============================================================================
// MCPConnection Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPConnectionState) {
    MCPConnection conn;
    conn.id = "conn-001";
    conn.state = MCPConnectionState::CONNECTED;
    conn.serverCapabilities = {"tools", "resources", "prompts"};

    EXPECT_EQ(conn.id, "conn-001");
    EXPECT_EQ(conn.state, MCPConnectionState::CONNECTED);
    EXPECT_EQ(conn.serverCapabilities.size(), 3);
    EXPECT_THAT(conn.serverCapabilities, Contains("tools"));
}

TEST_F(MCPGatewayTest, MCPConnectionDisconnected) {
    MCPConnection conn;
    conn.state = MCPConnectionState::DISCONNECTED;
    conn.disconnectReason = "Timeout";

    EXPECT_EQ(conn.state, MCPConnectionState::DISCONNECTED);
    EXPECT_EQ(conn.disconnectReason, "Timeout");
}

// ============================================================================
// MCPPrompt Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPPromptCreation) {
    MCPPrompt prompt;
    prompt.name = "code_review";
    prompt.description = "Review code for best practices";
    prompt.template_ = "Please review the following code:\n{{code}}";

    EXPECT_EQ(prompt.name, "code_review");
    EXPECT_NE(prompt.template_.find("{{code}}"), std::string::npos);
}

TEST_F(MCPGatewayTest, MCPPromptArguments) {
    MCPPrompt prompt;
    prompt.name = "summarize";

    MCPPromptArgument arg1;
    arg1.name = "text";
    arg1.description = "Text to summarize";
    arg1.required = true;
    prompt.arguments.push_back(arg1);

    MCPPromptArgument arg2;
    arg2.name = "maxLength";
    arg2.description = "Maximum summary length";
    arg2.required = false;
    prompt.arguments.push_back(arg2);

    EXPECT_EQ(prompt.arguments.size(), 2);
    EXPECT_TRUE(prompt.arguments[0].required);
    EXPECT_FALSE(prompt.arguments[1].required);
}

// ============================================================================
// MCPSerialization Tests
// ============================================================================

TEST_F(MCPGatewayTest, SerializeMCPMessage) {
    MCPMessage msg;
    msg.id = "test-123";
    msg.type = MCPMessageType::REQUEST;
    msg.method = "tools/list";

    MCPSerializer serializer;
    std::string json = serializer.serialize(msg);

    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("test-123"), std::string::npos);
    EXPECT_NE(json.find("tools/list"), std::string::npos);
}

TEST_F(MCPGatewayTest, DeserializeMCPMessage) {
    std::string json = R"({
        "id": "parsed-001",
        "type": "request",
        "method": "resources/read"
    })";

    MCPSerializer serializer;
    auto result = serializer.deserialize(json);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message.id, "parsed-001");
    EXPECT_EQ(result.message.type, MCPMessageType::REQUEST);
    EXPECT_EQ(result.message.method, "resources/read");
}

// ============================================================================
// MCPCapabilities Tests
// ============================================================================

TEST_F(MCPGatewayTest, MCPCapabilitiesToolSupport) {
    MCPCapabilities caps;
    caps.supportsTools = true;
    caps.supportsResources = true;
    caps.supportsPrompts = false;
    caps.supportsSampling = true;

    EXPECT_TRUE(caps.supportsTools);
    EXPECT_TRUE(caps.supportsResources);
    EXPECT_FALSE(caps.supportsPrompts);
    EXPECT_TRUE(caps.supportsSampling);
}

TEST_F(MCPGatewayTest, MCPCapabilitiesVersionCheck) {
    MCPCapabilities caps;
    caps.protocolVersion = "2024-11-05";
    caps.serverVersion = "1.0.0";

    EXPECT_EQ(caps.protocolVersion, "2024-11-05");
    EXPECT_TRUE(caps.isCompatible("2024-11-05"));
    EXPECT_FALSE(caps.isCompatible("2025-01-01")); // Future version
}
