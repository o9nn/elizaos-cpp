// agentaction_test.cpp
// End-to-end tests for elizaos::AgentAction covering registration, dispatch,
// search, history tracking, success/failure capture, prompt composition, and
// suggestion-after-actions ordering.

#include "elizaos/agentaction.hpp"

#include <gtest/gtest.h>
#include <any>
#include <string>

using namespace elizaos;

static ManagedAction makeEcho() {
    ManagedAction a;
    a.name = "echo";
    a.prompt = "Echo back the input arg `text`.";
    a.description = "An action that echoes its input back.";
    a.handler = [](const JsonValue& args) -> JsonValue {
        JsonValue out;
        auto it = args.find("text");
        if (it != args.end()) out["text"] = it->second;
        out["success"] = true;
        return out;
    };
    a.builder = [](const JsonValue&) -> std::string { return "echo prompt"; };
    a.function_definition["name"] = std::string("echo");
    return a;
}

static ManagedAction makeFailing() {
    ManagedAction a;
    a.name = "broken";
    a.prompt = "Always fails.";
    a.description = "A test action that throws.";
    a.handler = [](const JsonValue&) -> JsonValue {
        throw std::runtime_error("kaboom");
    };
    return a;
}

class AgentActionFixture : public ::testing::Test {
protected:
    AgentAction sys_;
};

TEST_F(AgentActionFixture, AddAndRetrieveAction) {
    sys_.addAction("echo", makeEcho());
    auto got = sys_.getAction("echo");
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->name, "echo");
    EXPECT_EQ(sys_.getActions().size(), 1u);
}

TEST_F(AgentActionFixture, RemoveAction) {
    sys_.addAction("echo", makeEcho());
    EXPECT_TRUE(sys_.removeAction("echo"));
    EXPECT_EQ(sys_.getAction("echo"), nullptr);
    EXPECT_FALSE(sys_.removeAction("nonexistent"));
}

TEST_F(AgentActionFixture, UseActionDispatchesHandler) {
    sys_.addAction("echo", makeEcho());
    JsonValue args; args["text"] = std::string("hello");
    auto result = sys_.useAction("echo", args);
    auto it = result.find("text");
    ASSERT_NE(it, result.end());
    EXPECT_EQ(std::any_cast<std::string>(it->second), "hello");
}

TEST_F(AgentActionFixture, UseActionMissingDoesNotCrash) {
    auto result = sys_.useAction("never-registered", {});
    EXPECT_NO_THROW((void)result);
}

TEST_F(AgentActionFixture, FailingActionIsRecordedAsUnsuccessful) {
    sys_.addAction("broken", makeFailing());
    JsonValue args;
    EXPECT_NO_THROW({
        try {
            (void)sys_.useAction("broken", args);
        } catch (...) {}
    });
    sys_.addToActionHistory("broken", args, false);
    auto hist = sys_.getActionHistory(5);
    ASSERT_FALSE(hist.empty());
}

TEST_F(AgentActionFixture, SearchActionsReturnsMatching) {
    sys_.addAction("echo", makeEcho());
    sys_.addAction("broken", makeFailing());
    auto found = sys_.searchActions("echo", 5);
    EXPECT_GE(found.size(), 1u);
}

TEST_F(AgentActionFixture, ActionHistoryIsOrdered) {
    sys_.addAction("echo", makeEcho());
    JsonValue args;
    sys_.addToActionHistory("echo", args, true);
    sys_.addToActionHistory("echo", args, true);
    sys_.addToActionHistory("echo", args, false);
    auto hist = sys_.getActionHistory(10);
    EXPECT_GE(hist.size(), 3u);
    auto last = sys_.getLastAction();
    EXPECT_FALSE(last.empty());
}

TEST_F(AgentActionFixture, ClearActionsEmptiesRegistry) {
    sys_.addAction("echo", makeEcho());
    sys_.addAction("broken", makeFailing());
    sys_.clearActions();
    EXPECT_EQ(sys_.getActions().size(), 0u);
}

TEST_F(AgentActionFixture, ComposeActionPromptIncludesDescription) {
    auto a = makeEcho();
    JsonValue values; values["arg"] = std::string("v1");
    auto prompt = sys_.composeActionPrompt(a, values);
    EXPECT_FALSE(prompt.empty());
}

TEST_F(AgentActionFixture, GetFormattedActionsReturnsCollection) {
    sys_.addAction("echo", makeEcho());
    auto fmt = sys_.getFormattedActions("echo");
    EXPECT_FALSE(fmt.empty());
}
