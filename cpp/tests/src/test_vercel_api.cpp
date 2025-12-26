#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/vercel_api.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;
using namespace ::testing;

class VercelApiTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_.apiToken = "test-token";
        config_.teamId = "team-123";
        config_.baseUrl = "https://api.vercel.com";
    }

    VercelConfig config_;
};

// ============================================================================
// VercelConfig Tests
// ============================================================================

TEST_F(VercelApiTest, DefaultVercelConfigValues) {
    VercelConfig defaultConfig;

    EXPECT_EQ(defaultConfig.baseUrl, "https://api.vercel.com");
    EXPECT_EQ(defaultConfig.timeout, 30);
    EXPECT_TRUE(defaultConfig.apiToken.empty());
}

TEST_F(VercelApiTest, CustomVercelConfigValues) {
    config_.timeout = 60;
    config_.projectId = "proj-456";

    EXPECT_EQ(config_.apiToken, "test-token");
    EXPECT_EQ(config_.teamId, "team-123");
    EXPECT_EQ(config_.timeout, 60);
    EXPECT_EQ(config_.projectId, "proj-456");
}

// ============================================================================
// VercelProject Tests
// ============================================================================

TEST_F(VercelApiTest, VercelProjectCreation) {
    VercelProject project;
    project.id = "prj-123456";
    project.name = "my-app";
    project.framework = "nextjs";
    project.createdAt = std::chrono::system_clock::now();

    EXPECT_EQ(project.id, "prj-123456");
    EXPECT_EQ(project.name, "my-app");
    EXPECT_EQ(project.framework, "nextjs");
}

TEST_F(VercelApiTest, VercelProjectWithDomains) {
    VercelProject project;
    project.id = "prj-123456";
    project.domains = {"my-app.vercel.app", "custom-domain.com"};

    EXPECT_EQ(project.domains.size(), 2);
    EXPECT_THAT(project.domains, Contains("custom-domain.com"));
}

TEST_F(VercelApiTest, VercelProjectEnvironment) {
    VercelProject project;
    project.id = "prj-123456";
    project.environmentVariables["DATABASE_URL"] = "postgres://...";
    project.environmentVariables["API_KEY"] = "secret-key";

    EXPECT_EQ(project.environmentVariables.size(), 2);
}

// ============================================================================
// VercelDeployment Tests
// ============================================================================

TEST_F(VercelApiTest, VercelDeploymentCreation) {
    VercelDeployment deployment;
    deployment.id = "dpl-789";
    deployment.url = "https://my-app-abc123.vercel.app";
    deployment.state = DeploymentState::READY;
    deployment.target = "production";

    EXPECT_EQ(deployment.id, "dpl-789");
    EXPECT_EQ(deployment.state, DeploymentState::READY);
    EXPECT_EQ(deployment.target, "production");
}

TEST_F(VercelApiTest, VercelDeploymentStates) {
    VercelDeployment deployment;

    deployment.state = DeploymentState::BUILDING;
    EXPECT_EQ(deployment.state, DeploymentState::BUILDING);

    deployment.state = DeploymentState::ERROR;
    EXPECT_EQ(deployment.state, DeploymentState::ERROR);

    deployment.state = DeploymentState::CANCELED;
    EXPECT_EQ(deployment.state, DeploymentState::CANCELED);
}

TEST_F(VercelApiTest, VercelDeploymentMeta) {
    VercelDeployment deployment;
    deployment.id = "dpl-789";
    deployment.meta.gitCommitSha = "abc123def456";
    deployment.meta.gitCommitMessage = "Fix: authentication bug";
    deployment.meta.gitBranch = "main";

    EXPECT_EQ(deployment.meta.gitCommitSha, "abc123def456");
    EXPECT_EQ(deployment.meta.gitBranch, "main");
}

// ============================================================================
// VercelDomain Tests
// ============================================================================

TEST_F(VercelApiTest, VercelDomainCreation) {
    VercelDomain domain;
    domain.name = "example.com";
    domain.projectId = "prj-123456";
    domain.verified = true;
    domain.redirect = "";

    EXPECT_EQ(domain.name, "example.com");
    EXPECT_TRUE(domain.verified);
}

TEST_F(VercelApiTest, VercelDomainDNS) {
    VercelDomain domain;
    domain.name = "example.com";

    VercelDNSRecord record;
    record.type = "A";
    record.name = "@";
    record.value = "76.76.21.21";
    domain.dnsRecords.push_back(record);

    EXPECT_EQ(domain.dnsRecords.size(), 1);
    EXPECT_EQ(domain.dnsRecords[0].type, "A");
}

// ============================================================================
// VercelFunction Tests
// ============================================================================

TEST_F(VercelApiTest, VercelFunctionCreation) {
    VercelFunction func;
    func.id = "fn-001";
    func.name = "api/hello";
    func.runtime = "nodejs18.x";
    func.memory = 1024;
    func.timeout = 10;

    EXPECT_EQ(func.id, "fn-001");
    EXPECT_EQ(func.runtime, "nodejs18.x");
    EXPECT_EQ(func.memory, 1024);
}

TEST_F(VercelApiTest, VercelFunctionInvocation) {
    VercelFunctionInvocation invocation;
    invocation.id = "inv-001";
    invocation.functionId = "fn-001";
    invocation.statusCode = 200;
    invocation.duration = std::chrono::milliseconds(150);

    EXPECT_EQ(invocation.statusCode, 200);
    EXPECT_EQ(invocation.duration.count(), 150);
}

// ============================================================================
// VercelLog Tests
// ============================================================================

TEST_F(VercelApiTest, VercelLogCreation) {
    VercelLog log;
    log.id = "log-001";
    log.deploymentId = "dpl-789";
    log.message = "Build completed successfully";
    log.level = LogLevel::INFO;
    log.source = "build";

    EXPECT_EQ(log.deploymentId, "dpl-789");
    EXPECT_EQ(log.level, LogLevel::INFO);
}

TEST_F(VercelApiTest, VercelLogLevels) {
    VercelLog log;

    log.level = LogLevel::DEBUG;
    EXPECT_EQ(log.level, LogLevel::DEBUG);

    log.level = LogLevel::WARNING;
    EXPECT_EQ(log.level, LogLevel::WARNING);

    log.level = LogLevel::ERROR;
    EXPECT_EQ(log.level, LogLevel::ERROR);
}

// ============================================================================
// VercelTeam Tests
// ============================================================================

TEST_F(VercelApiTest, VercelTeamCreation) {
    VercelTeam team;
    team.id = "team-123";
    team.name = "My Team";
    team.slug = "my-team";

    EXPECT_EQ(team.id, "team-123");
    EXPECT_EQ(team.name, "My Team");
    EXPECT_EQ(team.slug, "my-team");
}

TEST_F(VercelApiTest, VercelTeamMembers) {
    VercelTeam team;
    team.id = "team-123";

    VercelTeamMember member1;
    member1.userId = "user-001";
    member1.role = TeamRole::OWNER;
    team.members.push_back(member1);

    VercelTeamMember member2;
    member2.userId = "user-002";
    member2.role = TeamRole::MEMBER;
    team.members.push_back(member2);

    EXPECT_EQ(team.members.size(), 2);
    EXPECT_EQ(team.members[0].role, TeamRole::OWNER);
}

// ============================================================================
// VercelApiRequest Tests
// ============================================================================

TEST_F(VercelApiTest, VercelApiRequestCreation) {
    VercelApiRequest request;
    request.method = HttpMethod::GET;
    request.endpoint = "/v9/projects";
    request.headers["Authorization"] = "Bearer test-token";

    EXPECT_EQ(request.method, HttpMethod::GET);
    EXPECT_EQ(request.endpoint, "/v9/projects");
}

TEST_F(VercelApiTest, VercelApiRequestWithBody) {
    VercelApiRequest request;
    request.method = HttpMethod::POST;
    request.endpoint = "/v13/deployments";
    request.body = R"({"name": "my-deployment"})";

    EXPECT_EQ(request.method, HttpMethod::POST);
    EXPECT_FALSE(request.body.empty());
}

// ============================================================================
// VercelApiResponse Tests
// ============================================================================

TEST_F(VercelApiTest, VercelApiResponseSuccess) {
    VercelApiResponse response;
    response.statusCode = 200;
    response.body = R"({"id": "prj-123"})";
    response.success = true;

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.statusCode, 200);
}

TEST_F(VercelApiTest, VercelApiResponseError) {
    VercelApiResponse response;
    response.statusCode = 401;
    response.success = false;
    response.error = "Unauthorized: Invalid token";

    EXPECT_FALSE(response.success);
    EXPECT_EQ(response.statusCode, 401);
    EXPECT_NE(response.error.find("Unauthorized"), std::string::npos);
}

// ============================================================================
// VercelWebhook Tests
// ============================================================================

TEST_F(VercelApiTest, VercelWebhookCreation) {
    VercelWebhook webhook;
    webhook.id = "hook-001";
    webhook.url = "https://my-server.com/webhook";
    webhook.events = {"deployment.created", "deployment.succeeded"};

    EXPECT_EQ(webhook.id, "hook-001");
    EXPECT_EQ(webhook.events.size(), 2);
}

TEST_F(VercelApiTest, VercelWebhookPayload) {
    VercelWebhookPayload payload;
    payload.type = "deployment.succeeded";
    payload.deploymentId = "dpl-789";
    payload.projectId = "prj-123";

    EXPECT_EQ(payload.type, "deployment.succeeded");
    EXPECT_EQ(payload.deploymentId, "dpl-789");
}
