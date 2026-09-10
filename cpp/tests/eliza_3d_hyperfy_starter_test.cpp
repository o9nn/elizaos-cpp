#include <gtest/gtest.h>

#include "elizaos/eliza_3d_hyperfy_starter.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos::hyperfy;
using namespace std::chrono_literals;

namespace {

bool waitUntil(const std::function<bool()>& predicate) {
    const auto deadline = std::chrono::steady_clock::now() + 1s;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate()) {
            return true;
        }
        std::this_thread::sleep_for(1ms);
    }
    return predicate();
}

class RecordingAction final : public HyperfyAction {
public:
    std::string getName() const override { return "recording"; }
    std::string getDescription() const override { return "records parameters"; }
    bool execute(const std::string& parameters) override {
        lastParameters = parameters;
        ++calls;
        return result;
    }

    bool result = true;
    int calls = 0;
    std::string lastParameters;
};

class RecordingManager final : public HyperfyManager {
public:
    bool initialize(std::shared_ptr<HyperfyService> service) override {
        initializedWith = service;
        ++initializations;
        return initializeResult;
    }
    void cleanup() override { ++cleanups; }

    bool initializeResult = true;
    int initializations = 0;
    int cleanups = 0;
    std::weak_ptr<HyperfyService> initializedWith;
};

}  // namespace

TEST(HyperfySerialization, WebSocketMessageRoundTripsEveryTypeAndEscapedContent) {
    const std::vector<MessageType> types{
        MessageType::CONNECT, MessageType::DISCONNECT, MessageType::POSITION_UPDATE,
        MessageType::CHAT_MESSAGE, MessageType::ACTION, MessageType::WORLD_STATE,
        MessageType::HEARTBEAT, MessageType::ERROR,
    };
    for (const MessageType type : types) {
        const WebSocketMessage original{type, "line 1\n\"quoted\" \\ slash", "server", 42};
        const WebSocketMessage decoded = WebSocketMessage::fromJson(original.toJson());
        EXPECT_EQ(decoded.type, original.type);
        EXPECT_EQ(decoded.payload, original.payload);
        EXPECT_EQ(decoded.sender, original.sender);
        EXPECT_EQ(decoded.timestamp, original.timestamp);
    }

    const auto objectPayload = WebSocketMessage::fromJson(
        R"({"type":"world_state","payload":{"weather":"sunny"},"sender":"server","timestamp":7})");
    EXPECT_EQ(objectPayload.payload, R"({"weather":"sunny"})");
}

TEST(HyperfySerialization, RejectsMalformedOrIncompleteMessages) {
    EXPECT_THROW(WebSocketMessage::fromJson("not-json"), std::invalid_argument);
    EXPECT_THROW(WebSocketMessage::fromJson(R"({"type":"chat_message"})"), std::invalid_argument);
    EXPECT_THROW(WebSocketMessage::fromJson(
                     R"({"type":"bogus","payload":"x","sender":"s","timestamp":1})"),
                 std::invalid_argument);
    EXPECT_THROW(WebSocketMessage::fromJson(
                     R"({"type":"chat_message","payload":"x","sender":2,"timestamp":1})"),
                 std::invalid_argument);
    EXPECT_THROW((WebSocketMessage{static_cast<MessageType>(99), "x", "s", 1}.toJson()),
                 std::invalid_argument);
}

TEST(HyperfyPosition, StrictRoundTripAndMalformedInput) {
    const Position3D original{1.25, -2.5, 30.0};
    const Position3D decoded = Position3D::fromString(original.toString());
    EXPECT_DOUBLE_EQ(decoded.x, original.x);
    EXPECT_DOUBLE_EQ(decoded.y, original.y);
    EXPECT_DOUBLE_EQ(decoded.z, original.z);

    const Position3D spaced = Position3D::fromString(" 1 , 2 , 3 ");
    EXPECT_DOUBLE_EQ(spaced.x, 1.0);
    EXPECT_DOUBLE_EQ(spaced.y, 2.0);
    EXPECT_DOUBLE_EQ(spaced.z, 3.0);

    EXPECT_THROW(Position3D::fromString("1,2"), std::invalid_argument);
    EXPECT_THROW(Position3D::fromString("1,2,3 trailing"), std::invalid_argument);
    EXPECT_THROW(Position3D::fromString("nan,2,3"), std::invalid_argument);
    Position3D nonFinite{std::numeric_limits<double>::infinity(), 0.0, 0.0};
    EXPECT_THROW(nonFinite.toString(), std::invalid_argument);
}

TEST(HyperfyWebSocketClient, LifecycleSendQueuesCallbacksAndCleanTeardown) {
    WebSocketClient client;
    std::atomic<int> connects{0};
    std::atomic<int> disconnects{0};
    std::atomic<int> errors{0};
    std::atomic<int> received{0};
    std::string callbackPayload;
    std::mutex callbackMutex;

    client.setOnConnect([&] { ++connects; });
    client.setOnDisconnect([&](const std::string& reason) {
        EXPECT_EQ(reason, "client disconnect");
        ++disconnects;
    });
    client.setOnError([&](const std::string&) { ++errors; });
    client.setOnMessage([&](const WebSocketMessage& message) {
        std::lock_guard<std::mutex> lock(callbackMutex);
        callbackPayload = message.payload;
        ++received;
    });

    EXPECT_FALSE(client.connect(""));
    EXPECT_EQ(errors.load(), 1);
    EXPECT_FALSE(client.sendText("offline"));
    EXPECT_EQ(errors.load(), 2);

    ASSERT_TRUE(client.connect("memory://hyperfy", "secret"));
    EXPECT_TRUE(client.isConnected());
    EXPECT_EQ(connects.load(), 1);
    EXPECT_TRUE(client.connect("memory://hyperfy"));
    EXPECT_EQ(connects.load(), 1);

    ASSERT_TRUE(client.sendText("outbound"));
    ASSERT_TRUE(client.send({MessageType::ACTION, "jump", "local", 10}));
    ASSERT_TRUE(client.hasPendingOutgoingMessages());
    const WebSocketMessage outbound = client.popOutgoingMessage();
    EXPECT_EQ(outbound.type, MessageType::CHAT_MESSAGE);
    EXPECT_EQ(outbound.payload, "outbound");
    EXPECT_TRUE(client.hasPendingOutgoingMessages());
    EXPECT_EQ(client.popOutgoingMessage().type, MessageType::ACTION);
    EXPECT_FALSE(client.hasPendingOutgoingMessages());

    const WebSocketMessage inbound{MessageType::WORLD_STATE, "inbound", "server", 9};
    ASSERT_TRUE(client.injectIncomingMessage(inbound));
    ASSERT_TRUE(waitUntil([&] { return received.load() == 1; }));
    {
        std::lock_guard<std::mutex> lock(callbackMutex);
        EXPECT_EQ(callbackPayload, "inbound");
    }
    ASSERT_TRUE(client.hasPendingMessages());
    EXPECT_EQ(client.popMessage().sender, "server");
    EXPECT_FALSE(client.hasPendingMessages());
    EXPECT_EQ(client.popMessage().type, MessageType::ERROR);

    client.disconnect();
    EXPECT_FALSE(client.isConnected());
    EXPECT_EQ(disconnects.load(), 1);
    EXPECT_FALSE(client.injectIncomingMessage(inbound));
    EXPECT_EQ(errors.load(), 3);
    client.disconnect();
    EXPECT_EQ(disconnects.load(), 1);

    ASSERT_TRUE(client.connect("memory://again"));
    EXPECT_EQ(connects.load(), 2);
    client.disconnect();
    EXPECT_EQ(disconnects.load(), 2);
}

TEST(HyperfyWorld, IncomingStatePositionPerceptionAndMalformedPayloads) {
    HyperfyWorld world("world-1", "memory://world");
    EXPECT_FALSE(world.isConnected());
    EXPECT_EQ(world.getWorldId(), "world-1");
    EXPECT_EQ(world.getWsUrl(), "memory://world");
    EXPECT_FALSE(world.sendMessage("offline"));
    EXPECT_FALSE(world.moveToPosition(1, 2, 3));
    EXPECT_FALSE(world.performAction("wave"));
    EXPECT_FALSE(world.sendHeartbeat());

    ASSERT_TRUE(world.connect("token"));
    EXPECT_TRUE(world.isConnected());
    EXPECT_TRUE(world.sendMessage("hello"));
    EXPECT_EQ(world.getState("last_sent_message"), "hello");
    EXPECT_TRUE(world.performAction("wave", "fast"));
    EXPECT_EQ(world.getState("last_performed_action"), "wave");
    EXPECT_EQ(world.getState("last_action_parameters"), "fast");
    EXPECT_FALSE(world.performAction("", "fast"));
    EXPECT_TRUE(world.sendHeartbeat());
    EXPECT_FALSE(world.getState("last_heartbeat_sent").empty());

    const std::string state = R"({
        "weather":"sunny",
        "environmentDescription":"A bright plaza",
        "position":{"x":4.0,"y":5.0,"z":6.0},
        "entities":[
          {"id":"p1","type":"player","name":"Ada","position":{"x":5,"y":5,"z":6},"properties":{"mood":"curious"}},
          {"id":"tree","type":"object","name":"Oak","position":{"x":9,"y":0,"z":2}}
        ]
    })";
    ASSERT_TRUE(world.dispatchIncomingMessage(
        {MessageType::WORLD_STATE, state, "server", 100}));
    ASSERT_TRUE(waitUntil([&] { return world.getState("weather") == "sunny"; }));

    Position3D position = world.getCurrentPosition();
    EXPECT_DOUBLE_EQ(position.x, 4.0);
    EXPECT_DOUBLE_EQ(position.y, 5.0);
    EXPECT_DOUBLE_EQ(position.z, 6.0);
    EXPECT_EQ(world.getState("position_x"), "4");

    ASSERT_TRUE(world.dispatchIncomingMessage(
        {MessageType::POSITION_UPDATE, R"({"x":7,"y":8,"z":9})", "server", 101}));
    ASSERT_TRUE(waitUntil([&] { return world.getCurrentPosition().x == 7.0; }));

    const ScenePerception perception = world.perceiveScene();
    EXPECT_DOUBLE_EQ(perception.viewerPosition.x, 7.0);
    ASSERT_EQ(perception.visibleEntities.size(), 2u);
    EXPECT_EQ(perception.visibleEntities[0].properties.at("mood"), "curious");
    ASSERT_EQ(perception.nearbyPlayers.size(), 1u);
    EXPECT_EQ(perception.nearbyPlayers[0], "Ada");
    EXPECT_EQ(perception.environmentDescription, "A bright plaza");
    EXPECT_GT(perception.timestamp, 0);
    EXPECT_EQ(world.getLastPerception().visibleEntities.size(), 2u);

    world.updateState("manual", "value");
    EXPECT_EQ(world.getState("manual"), "value");
    EXPECT_EQ(world.getState("missing"), "");
    world.setCurrentPosition({10, 20, 30});
    EXPECT_DOUBLE_EQ(world.getCurrentPosition().z, 30.0);
    EXPECT_THROW(world.setCurrentPosition(
                     {std::numeric_limits<double>::quiet_NaN(), 0, 0}),
                 std::invalid_argument);

    ASSERT_TRUE(world.dispatchIncomingMessage(
        {MessageType::CHAT_MESSAGE, "server hello", "Ada", 102}));
    ASSERT_TRUE(waitUntil([&] { return world.getState("last_chat_message") == "server hello"; }));
    EXPECT_EQ(world.getState("last_chat_sender"), "Ada");
    ASSERT_TRUE(world.dispatchIncomingMessage(
        {MessageType::ACTION, "dance", "server", 103}));
    ASSERT_TRUE(waitUntil([&] { return world.getState("last_action") == "dance"; }));
    ASSERT_TRUE(world.dispatchIncomingMessage(
        {MessageType::HEARTBEAT, "", "server", 104}));
    ASSERT_TRUE(waitUntil([&] { return world.getState("last_heartbeat") == "104"; }));
    ASSERT_TRUE(world.dispatchIncomingMessage(
        {MessageType::ERROR, "remote failure", "server", 105}));
    ASSERT_TRUE(waitUntil([&] { return world.getState("last_error") == "remote failure"; }));
    ASSERT_TRUE(world.dispatchIncomingMessage(
        {MessageType::POSITION_UPDATE, "bad-json", "server", 106}));
    ASSERT_TRUE(waitUntil([&] { return world.getState("last_error") != "remote failure"; }));
    ASSERT_TRUE(world.dispatchIncomingMessage(
        {MessageType::DISCONNECT, "", "server", 107}));
    ASSERT_TRUE(waitUntil([&] { return !world.isConnected(); }));
    ASSERT_TRUE(world.dispatchIncomingMessage(
        {MessageType::CONNECT, "", "server", 108}));
    ASSERT_TRUE(waitUntil([&] { return world.isConnected(); }));

    world.disconnect();
    EXPECT_FALSE(world.isConnected());
    EXPECT_EQ(world.getState("weather"), "");
}

TEST(HyperfyService, RegistryManagerLifecycleAndWorkerJoin) {
    auto service = HyperfyServiceFactory::createService();
    ASSERT_NE(service, nullptr);
    EXPECT_FALSE(service->isRunning());
    EXPECT_EQ(service->getWorld(), nullptr);
    ASSERT_NE(service->getAction("goto"), nullptr);
    ASSERT_NE(service->getAction("reply"), nullptr);
    ASSERT_NE(service->getAction("perception"), nullptr);
    EXPECT_EQ(service->getAction("goto")->getName(), "goto");
    EXPECT_FALSE(service->getAction("goto")->getDescription().empty());
    EXPECT_EQ(service->getAction("reply")->getName(), "reply");
    EXPECT_FALSE(service->getAction("reply")->getDescription().empty());
    EXPECT_EQ(service->getAction("perception")->getName(), "perception");
    EXPECT_FALSE(service->getAction("perception")->getDescription().empty());

    auto action = std::make_shared<RecordingAction>();
    service->registerAction("recording", action);
    EXPECT_EQ(service->getAction("recording"), action);
    EXPECT_EQ(service->getAction("missing"), nullptr);
    EXPECT_FALSE(service->executeAction("recording", "stopped"));

    auto firstManager = std::make_shared<RecordingManager>();
    service->registerManager("behavior", firstManager);
    EXPECT_EQ(service->getManager("behavior"), firstManager);
    EXPECT_EQ(service->getManager("missing"), nullptr);

    HyperfyConfig config;
    config.wsUrl = "memory://service";
    config.worldId = "configured-world";
    config.authToken = "configured-token";
    config.heartbeatIntervalMs = 5;
    EXPECT_FALSE(service->connectToWorld("before-start", "memory://service"));
    ASSERT_TRUE(service->start(config));
    EXPECT_TRUE(service->start(config));
    EXPECT_TRUE(service->isRunning());
    EXPECT_EQ(firstManager->initializations, 1);
    EXPECT_EQ(firstManager->initializedWith.lock(), service);

    EXPECT_TRUE(service->executeAction("recording", "running"));
    EXPECT_EQ(action->calls, 1);
    EXPECT_EQ(action->lastParameters, "running");
    EXPECT_FALSE(service->executeAction("missing"));

    auto replacementManager = std::make_shared<RecordingManager>();
    service->registerManager("behavior", replacementManager);
    EXPECT_EQ(firstManager->cleanups, 1);
    EXPECT_EQ(replacementManager->initializations, 1);
    EXPECT_EQ(service->getManager("behavior"), replacementManager);

    ASSERT_TRUE(service->connectToWorld("world-2", "", "token"));
    ASSERT_NE(service->getWorld(), nullptr);
    EXPECT_EQ(service->getWorld()->getWsUrl(), "memory://service");
    EXPECT_TRUE(service->getWorld()->isConnected());
    service->disconnectFromWorld();
    EXPECT_EQ(service->getWorld(), nullptr);

    service->stop();
    EXPECT_FALSE(service->isRunning());
    EXPECT_EQ(replacementManager->cleanups, 1);
    service->stop();
    EXPECT_EQ(replacementManager->cleanups, 1);
}

TEST(HyperfyActions, BuiltInsParseAndApplyWorldEffects) {
    HyperfyConfig config;
    config.wsUrl = "memory://actions";
    config.heartbeatIntervalMs = 1000;
    auto service = HyperfyServiceFactory::createServiceWithConfig(config);
    ASSERT_EQ(service->getConfig().wsUrl, "memory://actions");
    ASSERT_TRUE(service->start(service->getConfig()));
    ASSERT_TRUE(service->connectToWorld("action-world", "", ""));

    EXPECT_TRUE(service->executeAction("goto", "1.5, 2.5, -3.5"));
    const Position3D position = service->getWorld()->getCurrentPosition();
    EXPECT_DOUBLE_EQ(position.x, 1.5);
    EXPECT_DOUBLE_EQ(position.y, 2.5);
    EXPECT_DOUBLE_EQ(position.z, -3.5);
    EXPECT_FALSE(service->executeAction("goto", "1,2"));
    EXPECT_FALSE(service->executeAction("goto", "a,b,c"));
    EXPECT_TRUE(service->executeAction("reply", "hello world"));
    EXPECT_EQ(service->getWorld()->getState("last_sent_message"), "hello world");
    EXPECT_TRUE(service->executeAction("perception", "ignored"));
    EXPECT_GT(service->getWorld()->getLastPerception().timestamp, 0);

    auto detached = service->getAction("goto");
    service->stop();
    service.reset();
    EXPECT_FALSE(detached->execute("1,2,3"));

    EXPECT_FALSE(GotoAction(nullptr).execute("1,2,3"));
    EXPECT_FALSE(ReplyAction(nullptr).execute("hello"));
    EXPECT_FALSE(PerceptionAction(nullptr).execute(""));
}

TEST(HyperfyConsumerLink, InstantiatesAndCallsTheCompletePublicSurface) {
    WebSocketMessage message;
    EXPECT_EQ(message.type, MessageType::ERROR);
    EXPECT_NO_THROW(message.toJson());

    Position3D position;
    EXPECT_EQ(position.toString(), "0,0,0");

    HyperfyWorld world("consumer", "memory://consumer");
    EXPECT_NO_THROW(world.updateState("key", "value"));
    EXPECT_EQ(world.getState("key"), "value");
    EXPECT_NO_THROW(world.setCurrentPosition(position));
    EXPECT_DOUBLE_EQ(world.getCurrentPosition().x, 0.0);
    EXPECT_NO_THROW(world.perceiveScene());

    HyperfyService stackService;
    HyperfyConfig config;
    config.heartbeatIntervalMs = 1;
    EXPECT_TRUE(stackService.start(config));
    stackService.stop();
}
