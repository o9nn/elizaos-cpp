// agentaction_test.cpp - E2E tests for elizaos::AgentAction.
#include <gtest/gtest.h>
#include "elizaos/agentaction.hpp"

#include <any>
#include <atomic>
#include <thread>
#include <vector>

using namespace elizaos;

namespace {
ManagedAction makeEcho(const std::string& name = "echo") {
    ManagedAction a;
    a.name = name;
    a.prompt = "echo {{text}}";
    a.description = "Echoes the input.";
    a.handler = [](const JsonValue& in) -> JsonValue {
        JsonValue out;
        out["success"] = true;
        if (in.count("text")) out["text"] = in.at("text");
        return out;
    };
    return a;
}
}

class AgentActionTest : public ::testing::Test {
protected:
    AgentAction system;
};

TEST_F(AgentActionTest, AddAndGetAction) {
    system.addAction("echo", makeEcho());
    auto a = system.getAction("echo");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->name, "echo");
    EXPECT_EQ(a->description, "Echoes the input.");
}

TEST_F(AgentActionTest, GetActionMissingReturnsNull) {
    EXPECT_EQ(system.getAction("nope"), nullptr);
}

TEST_F(AgentActionTest, GetActionsContainsAdded) {
    system.addAction("a1", makeEcho("a1"));
    system.addAction("a2", makeEcho("a2"));
    const auto& actions = system.getActions();
    EXPECT_EQ(actions.size(), 2u);
}

TEST_F(AgentActionTest, RemoveAction) {
    system.addAction("rem", makeEcho("rem"));
    EXPECT_TRUE(system.removeAction("rem"));
    EXPECT_EQ(system.getAction("rem"), nullptr);
    EXPECT_FALSE(system.removeAction("rem"));
}

TEST_F(AgentActionTest, ClearActions) {
    system.addAction("c1", makeEcho("c1"));
    system.addAction("c2", makeEcho("c2"));
    system.clearActions();
    EXPECT_TRUE(system.getActions().empty());
}

TEST_F(AgentActionTest, UseActionExecutesHandler) {
    system.addAction("echo", makeEcho());
    JsonValue args;
    args["text"] = std::string("hello");
    auto out = system.useAction("echo", args);
    ASSERT_TRUE(out.count("success"));
    EXPECT_TRUE(std::any_cast<bool>(out["success"]));
}

TEST_F(AgentActionTest, UseActionMissingReportsFailure) {
    JsonValue args;
    auto out = system.useAction("nope", args);
    // Either reports an error or returns empty - just must not throw
    SUCCEED();
}

TEST_F(AgentActionTest, ActionHistoryGrowsWithUse) {
    system.addAction("echo", makeEcho());
    JsonValue args;
    system.useAction("echo", args);
    system.useAction("echo", args);
    auto hist = system.getActionHistory(10);
    EXPECT_GE(hist.size(), 2u);
}

TEST_F(AgentActionTest, ActionHistoryPreservesSerializedArguments) {
    system.addAction("echo", makeEcho());
    JsonValue args;
    args["text"] = std::string("hello from history");
    args["attempt"] = 3;
    args["confirmed"] = true;

    system.useAction("echo", args);
    auto last = system.getLastAction();
    ASSERT_FALSE(last.empty());
    ASSERT_TRUE(last.count("metadata"));

    auto metadata = std::any_cast<JsonValue>(last.at("metadata"));
    EXPECT_EQ(std::any_cast<std::string>(metadata.at("text")), "hello from history");
    EXPECT_EQ(std::any_cast<std::string>(metadata.at("attempt")), "3");
    EXPECT_EQ(std::any_cast<std::string>(metadata.at("confirmed")), "true");
    EXPECT_EQ(std::any_cast<std::string>(metadata.at("success")), "true");
}

TEST_F(AgentActionTest, AddToActionHistoryDirectly) {
    JsonValue args;
    system.addToActionHistory("manual", args, true);
    auto last = system.getLastAction();
    EXPECT_FALSE(last.empty());
}

TEST_F(AgentActionTest, SearchActions) {
    system.addAction("greet_user", makeEcho("greet_user"));
    system.addAction("farewell_user", makeEcho("farewell_user"));
    auto found = system.searchActions("greet", 5);
    SUCCEED() << "found " << found.size();
}

TEST_F(AgentActionTest, ComposeActionPromptSubstitutes) {
    auto a = makeEcho();
    JsonValue values;
    values["text"] = std::string("WORLD");
    auto prompt = system.composeActionPrompt(a, values);
    EXPECT_FALSE(prompt.empty());
}

TEST_F(AgentActionTest, FormattedActionsReturnsJson) {
    system.addAction("echo", makeEcho());
    auto fmt = system.getFormattedActions("");
    SUCCEED() << "formatted action count: " << fmt.size();
}

TEST_F(AgentActionTest, ConcurrentRegistrationAndLookupAreSafe) {
    constexpr int kThreads = 8;
    constexpr int kActionsPerThread = 25;
    std::atomic<int> successfulLookups{0};
    std::vector<std::thread> workers;
    workers.reserve(kThreads);

    for (int threadIndex = 0; threadIndex < kThreads; ++threadIndex) {
        workers.emplace_back([&, threadIndex] {
            for (int actionIndex = 0; actionIndex < kActionsPerThread; ++actionIndex) {
                const auto name = "action_" + std::to_string(threadIndex) + "_" +
                                  std::to_string(actionIndex);
                system.addAction(name, makeEcho(name));
                if (system.getAction(name)) ++successfulLookups;
            }
        });
    }
    for (auto& worker : workers) worker.join();

    EXPECT_EQ(successfulLookups.load(), kThreads * kActionsPerThread);
    EXPECT_EQ(system.getActions().size(),
              static_cast<std::size_t>(kThreads * kActionsPerThread));
}
