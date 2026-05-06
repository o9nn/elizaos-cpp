#include <gtest/gtest.h>
#include "elizaos/agentaction.hpp"

using namespace elizaos;

class AgentActionTest : public ::testing::Test {
protected:
    void SetUp() override {
        agentAction = std::make_unique<AgentAction>();
    }

    void TearDown() override {
        agentAction.reset();
    }

    std::unique_ptr<AgentAction> agentAction;
};

TEST_F(AgentActionTest, BasicActionManagement) {
    // Create a simple test action
    ManagedAction testAction;
    testAction.name = "test_action";
    testAction.prompt = "This is a test action";
    testAction.description = "A simple test action for testing";
    testAction.handler = [](const JsonValue& /*args*/) -> JsonValue {
        JsonValue result;
        result["output"] = std::string("Test executed successfully");
        return result;
    };
    
    // Add the action
    agentAction->addAction("test_action", testAction);
    
    // Check that we can retrieve it
    auto retrievedAction = agentAction->getAction("test_action");
    ASSERT_NE(retrievedAction, nullptr);
    EXPECT_EQ(retrievedAction->name, "test_action");
    EXPECT_EQ(retrievedAction->description, "A simple test action for testing");
}

TEST_F(AgentActionTest, ActionExecution) {
    // Create an action with a handler
    ManagedAction testAction;
    testAction.name = "echo_action";
    testAction.prompt = "Echo the input";
    testAction.description = "Returns the input message";
    testAction.handler = [](const JsonValue& args) -> JsonValue {
        JsonValue result;
        if (args.count("message")) {
            try {
                auto message = std::any_cast<std::string>(args.at("message"));
                result["echo"] = message;
            } catch (const std::bad_any_cast&) {
                result["echo"] = std::string("Could not parse message");
            }
        } else {
            result["echo"] = std::string("No message provided");
        }
        return result;
    };
    
    // Add and execute the action
    agentAction->addAction("echo_action", testAction);
    
    JsonValue arguments;
    arguments["message"] = std::string("Hello, World!");
    
    auto result = agentAction->useAction("echo_action", arguments);
    
    EXPECT_TRUE(std::any_cast<bool>(result["success"]));
    EXPECT_EQ(std::any_cast<std::string>(result["echo"]), "Hello, World!");
}

TEST_F(AgentActionTest, NonExistentAction) {
    // Try to execute an action that doesn't exist
    JsonValue arguments;
    auto result = agentAction->useAction("nonexistent_action", arguments);
    
    EXPECT_FALSE(std::any_cast<bool>(result["success"]));
    EXPECT_EQ(std::any_cast<std::string>(result["error"]), "Action not found");
}

TEST_F(AgentActionTest, ActionHistory) {
    // Create a simple action
    ManagedAction testAction;
    testAction.name = "history_test";
    testAction.description = "Test action for history";
    testAction.handler = [](const JsonValue& /*args*/) -> JsonValue {
        JsonValue result;
        result["output"] = std::string("History test complete");
        return result;
    };
    
    agentAction->addAction("history_test", testAction);
    
    // Execute the action
    JsonValue arguments;
    arguments["test_param"] = std::string("test_value");
    agentAction->useAction("history_test", arguments);
    
    // Check action history
    auto history = agentAction->getActionHistory(10);
    EXPECT_FALSE(history.empty());
    
    // Check last action
    auto lastAction = agentAction->getLastAction();
    EXPECT_FALSE(lastAction.empty());
    EXPECT_EQ(std::any_cast<std::string>(lastAction["document"]), "history_test");
}

TEST_F(AgentActionTest, PromptComposition) {
    // Create an action with a custom prompt
    ManagedAction testAction;
    testAction.name = "prompt_test";
    testAction.prompt = "Default prompt";
    testAction.description = "Test prompt composition";
    testAction.builder = [](const JsonValue& values) -> std::string {
        if (values.count("custom_prompt")) {
            try {
                return std::any_cast<std::string>(values.at("custom_prompt"));
            } catch (const std::bad_any_cast&) {}
        }
        return "Built prompt";
    };
    
    agentAction->addAction("prompt_test", testAction);
    auto action = agentAction->getAction("prompt_test");
    
    // Test default prompt
    JsonValue emptyValues;
    auto defaultPrompt = agentAction->composeActionPrompt(*action, emptyValues);
    EXPECT_EQ(defaultPrompt, "Built prompt");
    
    // Test custom prompt via builder
    JsonValue customValues;
    customValues["custom_prompt"] = std::string("Custom built prompt");
    auto customPrompt = agentAction->composeActionPrompt(*action, customValues);
    EXPECT_EQ(customPrompt, "Custom built prompt");
}

TEST_F(AgentActionTest, SearchActions) {
    // Add multiple actions
    ManagedAction action1;
    action1.name = "search_test_1";
    action1.description = "First search test action";
    agentAction->addAction("search_test_1", action1);
    
    ManagedAction action2;
    action2.name = "search_test_2";
    action2.description = "Second search test action";
    agentAction->addAction("search_test_2", action2);
    
    ManagedAction action3;
    action3.name = "different_action";
    action3.description = "A completely different action";
    agentAction->addAction("different_action", action3);
    
    // Search for actions containing "search"
    auto results = agentAction->searchActions("search", 10);
    
    // Should find at least the two search actions
    EXPECT_GE(results.size(), 2);
    
    // Check that search results contain expected actions
    bool found_search_1 = false, found_search_2 = false;
    for (const auto& result : results) {
        auto document = std::any_cast<std::string>(result.at("document"));
        if (document.find("First search test") != std::string::npos) {
            found_search_1 = true;
        }
        if (document.find("Second search test") != std::string::npos) {
            found_search_2 = true;
        }
    }
    
    EXPECT_TRUE(found_search_1);
    EXPECT_TRUE(found_search_2);
}

TEST_F(AgentActionTest, GetFormattedActions) {
    // Add an action
    ManagedAction testAction;
    testAction.name = "format_test";
    testAction.description = "Test formatted output";
    agentAction->addAction("format_test", testAction);
    
    // Get formatted actions
    auto formatted = agentAction->getFormattedActions("format");
    
    EXPECT_TRUE(formatted.count("available_actions"));
    EXPECT_TRUE(formatted.count("formatted_actions"));
    EXPECT_TRUE(formatted.count("short_actions"));
    
    // Check that the formatted std::string contains expected content
    auto formattedStr = std::any_cast<std::string>(formatted["formatted_actions"]);
    EXPECT_TRUE(formattedStr.find("Available actions") != std::string::npos);
    
    auto shortStr = std::any_cast<std::string>(formatted["short_actions"]);
    EXPECT_TRUE(shortStr.find("Available actions (name)") != std::string::npos);
}

TEST_F(AgentActionTest, RemoveAction) {
    // Add an action
    ManagedAction testAction;
    testAction.name = "remove_test";
    testAction.description = "Action to be removed";
    agentAction->addAction("remove_test", testAction);
    
    // Verify it exists
    EXPECT_NE(agentAction->getAction("remove_test"), nullptr);
    
    // Remove it
    EXPECT_TRUE(agentAction->removeAction("remove_test"));
    
    // Verify it's gone
    EXPECT_EQ(agentAction->getAction("remove_test"), nullptr);
    
    // Try to remove again
    EXPECT_FALSE(agentAction->removeAction("remove_test"));
}

TEST_F(AgentActionTest, ClearActions) {
    // Add multiple actions
    ManagedAction action1;
    action1.name = "clear_test_1";
    agentAction->addAction("clear_test_1", action1);
    
    ManagedAction action2;
    action2.name = "clear_test_2";
    agentAction->addAction("clear_test_2", action2);
    
    // Verify they exist
    EXPECT_EQ(agentAction->getActions().size(), 2);
    
    // Clear all actions
    agentAction->clearActions();
    
    // Verify they're gone
    EXPECT_EQ(agentAction->getActions().size(), 0);
    EXPECT_EQ(agentAction->getAction("clear_test_1"), nullptr);
    EXPECT_EQ(agentAction->getAction("clear_test_2"), nullptr);
}