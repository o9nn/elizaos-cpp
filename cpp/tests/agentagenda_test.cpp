// agentagenda_test.cpp - E2E tests for elizaos::AgentAgenda.
#include <gtest/gtest.h>
#include "elizaos/agentagenda.hpp"

using namespace elizaos;

class AgentAgendaTest : public ::testing::Test {
protected:
    AgentAgenda agenda;

    void TearDown() override { agenda.clearTasks(); }
};

TEST_F(AgentAgendaTest, CreateTaskBasic) {
    auto t = agenda.createTask("write the report");
    EXPECT_FALSE(t.id.empty());
    EXPECT_EQ(t.goal, "write the report");
}

TEST_F(AgentAgendaTest, CreateTaskWithPlanAndSteps) {
    std::vector<AgendaTaskStep> steps{{"draft outline"}, {"fill body"}, {"review"}};
    auto t = agenda.createTask("write report", "outline + write + review", steps);
    EXPECT_EQ(t.steps.size(), 3u);
    EXPECT_EQ(t.plan, "outline + write + review");
}

TEST_F(AgentAgendaTest, ListTasksReflectsCreated) {
    agenda.createTask("a");
    agenda.createTask("b");
    auto in_progress = agenda.listTasks(AgendaTaskStatus::IN_PROGRESS);
    EXPECT_EQ(in_progress.size(), 2u);
}

TEST_F(AgentAgendaTest, GetTaskByIdReturnsTask) {
    auto t = agenda.createTask("findme");
    auto got = agenda.getTaskById(t.id);
    EXPECT_EQ(got.id, t.id);
    EXPECT_EQ(got.goal, "findme");
}

TEST_F(AgentAgendaTest, FinishTaskUpdatesStatus) {
    auto t = agenda.createTask("done soon");
    EXPECT_TRUE(agenda.finishTask(t.id));
    auto complete = agenda.listTasks(AgendaTaskStatus::COMPLETE);
    EXPECT_EQ(complete.size(), 1u);
}

TEST_F(AgentAgendaTest, CancelTaskUpdatesStatus) {
    auto t = agenda.createTask("oops");
    EXPECT_TRUE(agenda.cancelTask(t.id));
    auto cancelled = agenda.listTasks(AgendaTaskStatus::CANCELLED);
    EXPECT_EQ(cancelled.size(), 1u);
}

TEST_F(AgentAgendaTest, DeleteTask) {
    auto t = agenda.createTask("ephemeral");
    EXPECT_TRUE(agenda.deleteTask(t.id));
    EXPECT_TRUE(agenda.getTaskById(t.id).id.empty());
}

TEST_F(AgentAgendaTest, SetCurrentTask) {
    auto t = agenda.createTask("focus");
    EXPECT_TRUE(agenda.setCurrentTask(t.id));
    auto cur = agenda.getCurrentTask();
    EXPECT_EQ(cur.id, t.id);
}

TEST_F(AgentAgendaTest, AddAndFinishStep) {
    auto t = agenda.createTask("multi-step");
    EXPECT_TRUE(agenda.addStep(t.id, "step one"));
    EXPECT_TRUE(agenda.addStep(t.id, "step two"));
    EXPECT_TRUE(agenda.finishStep(t.id, "step one"));
    auto fetched = agenda.getTaskById(t.id);
    bool found_completed = false;
    for (const auto& s : fetched.steps) {
        if (s.content == "step one" && s.completed) found_completed = true;
    }
    EXPECT_TRUE(found_completed);
}

TEST_F(AgentAgendaTest, CancelStepRemovesIt) {
    auto t = agenda.createTask("with step");
    agenda.addStep(t.id, "removable");
    EXPECT_TRUE(agenda.cancelStep(t.id, "removable"));
}

TEST_F(AgentAgendaTest, UpdatePlan) {
    auto t = agenda.createTask("p");
    EXPECT_TRUE(agenda.updatePlan(t.id, "new plan"));
    auto fetched = agenda.getTaskById(t.id);
    EXPECT_EQ(fetched.plan, "new plan");
}

TEST_F(AgentAgendaTest, CreatePlanReturnsNonEmpty) {
    auto plan = agenda.createPlan("ship feature");
    EXPECT_FALSE(plan.empty());
}

TEST_F(AgentAgendaTest, CreateStepsReturnsList) {
    auto steps = agenda.createSteps("ship feature", "design then build");
    EXPECT_FALSE(steps.empty());
}

TEST_F(AgentAgendaTest, SearchTasks) {
    agenda.createTask("write the report");
    agenda.createTask("read the report");
    auto found = agenda.searchTasks("report");
    EXPECT_GE(found.size(), 1u);
}

TEST_F(AgentAgendaTest, FormattedTaskString) {
    auto t = agenda.createTask("formatted");
    auto s = agenda.getTaskAsFormattedString(t);
    EXPECT_FALSE(s.empty());
    EXPECT_NE(s.find("formatted"), std::string::npos);
}

TEST_F(AgentAgendaTest, ListTasksFormattedString) {
    agenda.createTask("a");
    agenda.createTask("b");
    auto s = agenda.listTasksAsFormattedString();
    EXPECT_FALSE(s.empty());
}

TEST_F(AgentAgendaTest, LastCreatedAndUpdated) {
    auto a = agenda.createTask("first");
    auto b = agenda.createTask("second");
    auto last_created = agenda.getLastCreatedTask();
    EXPECT_EQ(last_created.id, b.id);
    agenda.updatePlan(a.id, "newplan");
    auto last_updated = agenda.getLastUpdatedTask();
    EXPECT_EQ(last_updated.id, a.id);
}
