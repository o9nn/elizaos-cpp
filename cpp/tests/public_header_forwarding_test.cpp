#include <gtest/gtest.h>

#include <string>

// This test target deliberately places cpp/include before include/ in its
// include path. It proves that legacy include-order configurations still see
// the authoritative public API through cpp/include compatibility wrappers.
#include <elizaos/cognitive_bridge.hpp>
#include <elizaos/plugin_specification.hpp>

namespace {

TEST(PublicHeaderForwardingTest, CognitiveBridgeAuthoritativeApiIsVisible) {
    elizaos::CognitiveState state;
    state.agentId = "agent-forwarding";
    state.mood = "curious";
    state.focus = "header drift regression";
    state.valence = 0.25;
    state.arousal = 0.75;
    state.metadata["schema"] = "active-root-header";

    EXPECT_EQ(state.agentId, "agent-forwarding");
    EXPECT_EQ(state.metadata.at("schema"), "active-root-header");
    EXPECT_DOUBLE_EQ(state.valence, 0.25);
    EXPECT_DOUBLE_EQ(state.arousal, 0.75);

    elizaos::SensoryInput sensory;
    sensory.sourceId = "renderer";
    sensory.modality = "text";
    sensory.payload = "hello from a legacy include path";
    sensory.metadata["channel"] = "sensory_input";

    EXPECT_EQ(sensory.modality, "text");
    EXPECT_EQ(sensory.payload, "hello from a legacy include path");

    elizaos::SpeechOutput speech;
    speech.agentId = "agent-forwarding";
    speech.text = "forwarded headers preserve speech output fields";
    speech.voice = "test-voice";
    speech.prosodyRate = 1.2;
    speech.prosodyPitch = 0.9;

    EXPECT_EQ(speech.agentId, "agent-forwarding");
    EXPECT_EQ(speech.voice, "test-voice");
    EXPECT_DOUBLE_EQ(speech.prosodyRate, 1.2);
    EXPECT_DOUBLE_EQ(speech.prosodyPitch, 0.9);

    EXPECT_EQ(elizaos::CognitiveBridge::phaseForStep(1), 0);
    EXPECT_EQ(elizaos::CognitiveBridge::phaseForStep(12), 3);
    EXPECT_EQ(elizaos::CognitiveBridge::phaseForStep(13), 0);

    elizaos::CognitiveBridge bridge("forwarding-test-bridge");
    EXPECT_EQ(bridge.id(), "forwarding-test-bridge");
    EXPECT_FALSE(bridge.isEchobeatsRunning());
    EXPECT_EQ(bridge.stats().echobeatsTicks, 0u);
}

TEST(PublicHeaderForwardingTest, PluginLifecycleAuthoritativeApiIsVisible) {
    elizaos::PluginMetadata metadata;
    metadata.name = "forwarded-plugin";
    metadata.displayName = "Forwarded Plugin";
    metadata.description = "Verifies plugin lifecycle types through cpp/include forwarding wrappers.";
    metadata.capabilities.push_back(elizaos::PluginCapability::MESSAGE_HANDLING);
    metadata.customFields["include_order"] = "cpp-first";

    ASSERT_EQ(metadata.capabilities.size(), 1u);
    EXPECT_EQ(metadata.capabilities.front(), elizaos::PluginCapability::MESSAGE_HANDLING);
    EXPECT_EQ(metadata.customFields.at("include_order"), "cpp-first");

    elizaos::PluginContext context;
    context.parameters["mode"] = std::string("test");
    EXPECT_EQ(context.getParameter<std::string>("mode", "missing"), "test");
    EXPECT_EQ(context.getParameter<std::string>("absent", "fallback"), "fallback");

    context.setSessionData("count", 7);
    EXPECT_EQ(context.getSessionData<int>("count", 0), 7);
    EXPECT_EQ(context.getSessionData<int>("absent", 42), 42);

    elizaos::PluginResult result;
    result.success = true;
    result.message = "ok";
    result.data = std::string("payload");
    result.metadata["latency_ms"] = 3;

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message, "ok");
    EXPECT_EQ(result.getData<std::string>(), "payload");
    EXPECT_EQ(result.getMetadata<int>("latency_ms", 0), 3);
}

} // namespace
