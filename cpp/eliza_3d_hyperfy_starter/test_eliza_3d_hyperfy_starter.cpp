#include <gtest/gtest.h>
#include "elizaos/eliza_3d_hyperfy_starter.hpp"

using namespace elizaos::hyperfy;

class HyperfyServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = HyperfyServiceFactory::createService();
    }
    
    void TearDown() override {
        if (service && service->isRunning()) {
            service->stop();
        }
    }
    
    std::shared_ptr<HyperfyService> service;
};

TEST_F(HyperfyServiceTest, ServiceCreation) {
    ASSERT_NE(service, nullptr);
    EXPECT_FALSE(service->isRunning());
}

TEST_F(HyperfyServiceTest, ServiceStartStop) {
    HyperfyConfig config;
    config.wsUrl = "ws://test.example.com";
    config.worldId = "test-world";
    
    EXPECT_TRUE(service->start(config));
    EXPECT_TRUE(service->isRunning());
    
    service->stop();
    EXPECT_FALSE(service->isRunning());
}

TEST_F(HyperfyServiceTest, WorldConnection) {
    HyperfyConfig config;
    service->start(config);
    
    EXPECT_TRUE(service->connectToWorld("test-world", "ws://test.example.com"));
    ASSERT_NE(service->getWorld(), nullptr);
    EXPECT_TRUE(service->getWorld()->isConnected());
    EXPECT_EQ(service->getWorld()->getWorldId(), "test-world");
    
    service->disconnectFromWorld();
    // Note: getWorld() might still return a pointer but it should be disconnected
    if (service->getWorld()) {
        EXPECT_FALSE(service->getWorld()->isConnected());
    }
}

TEST_F(HyperfyServiceTest, ActionRegistrationAndExecution) {
    service->start(HyperfyConfig{});
    service->connectToWorld("test-world", "ws://test.example.com");
    
    // Test that default actions are registered
    EXPECT_NE(service->getAction("goto"), nullptr);
    EXPECT_NE(service->getAction("reply"), nullptr);
    EXPECT_NE(service->getAction("perception"), nullptr);
    
    // Test action execution
    EXPECT_TRUE(service->executeAction("goto", "1.0,2.0,3.0"));
    EXPECT_TRUE(service->executeAction("reply", "Test message"));
    EXPECT_TRUE(service->executeAction("perception", ""));
    
    // Test invalid action
    EXPECT_FALSE(service->executeAction("nonexistent", ""));
}

TEST_F(HyperfyServiceTest, WorldState) {
    auto world = std::make_shared<HyperfyWorld>("test", "ws://test.com");
    world->connect();
    
    EXPECT_TRUE(world->isConnected());
    
    // Test state management
    world->updateState("test_key", "test_value");
    EXPECT_EQ(world->getState("test_key"), "test_value");
    EXPECT_EQ(world->getState("nonexistent_key"), "");
    
    world->disconnect();
    EXPECT_FALSE(world->isConnected());
}

TEST_F(HyperfyServiceTest, GotoActionParameterParsing) {
    service->start(HyperfyConfig{});
    service->connectToWorld("test-world", "ws://test.example.com");
    
    auto gotoAction = service->getAction("goto");
    ASSERT_NE(gotoAction, nullptr);
    
    // Test valid parameters
    EXPECT_TRUE(gotoAction->execute("1.0,2.0,3.0"));
    
    // Test invalid parameters
    EXPECT_FALSE(gotoAction->execute("1.0,2.0")); // Missing z coordinate
    EXPECT_FALSE(gotoAction->execute("invalid,params,here"));
    EXPECT_FALSE(gotoAction->execute(""));
}

TEST(HyperfyWorldTest, BasicFunctionality) {
    HyperfyWorld world("test-world", "ws://test.example.com");
    
    EXPECT_FALSE(world.isConnected());
    EXPECT_EQ(world.getWorldId(), "test-world");
    EXPECT_EQ(world.getWsUrl(), "ws://test.example.com");
    
    EXPECT_TRUE(world.connect());
    EXPECT_TRUE(world.isConnected());
    
    // Test world operations
    EXPECT_TRUE(world.sendMessage("Hello World"));
    EXPECT_TRUE(world.moveToPosition(10.0, 20.0, 30.0));
    EXPECT_TRUE(world.performAction("test_action", "test_params"));
    
    world.disconnect();
    EXPECT_FALSE(world.isConnected());
}