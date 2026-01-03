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
        config_.api_token = "test-token";
        config_.team_id = "team-123";
    }

    VercelConfig config_;
};

// ============================================================================
// VercelConfig Tests
// ============================================================================

TEST_F(VercelApiTest, DefaultVercelConfigValues) {
    VercelConfig defaultConfig;

    EXPECT_EQ(defaultConfig.api_base_url, "https://api.vercel.com");
    EXPECT_EQ(defaultConfig.api_version, "v2");
    EXPECT_EQ(defaultConfig.timeout_seconds, 30);
    EXPECT_EQ(defaultConfig.max_retries, 3);
    EXPECT_TRUE(defaultConfig.enable_logging);
    EXPECT_TRUE(defaultConfig.api_token.empty());
}

TEST_F(VercelApiTest, TokenConstructor) {
    VercelConfig config("my-token");

    EXPECT_EQ(config.api_token, "my-token");
    EXPECT_TRUE(config.team_id.empty());
}

TEST_F(VercelApiTest, TokenAndTeamConstructor) {
    VercelConfig config("my-token", "my-team");

    EXPECT_EQ(config.api_token, "my-token");
    EXPECT_EQ(config.team_id, "my-team");
}

TEST_F(VercelApiTest, CustomVercelConfigValues) {
    config_.timeout_seconds = 60;
    config_.max_retries = 5;
    config_.enable_logging = false;

    EXPECT_EQ(config_.api_token, "test-token");
    EXPECT_EQ(config_.team_id, "team-123");
    EXPECT_EQ(config_.timeout_seconds, 60);
    EXPECT_EQ(config_.max_retries, 5);
    EXPECT_FALSE(config_.enable_logging);
}

// ============================================================================
// HttpResponse Tests
// ============================================================================

TEST_F(VercelApiTest, HttpResponseDefaultConstruction) {
    HttpResponse response;

    EXPECT_EQ(response.status_code, 0);
    EXPECT_TRUE(response.body.empty());
    EXPECT_FALSE(response.success);
}

TEST_F(VercelApiTest, HttpResponseWithCodeAndBody) {
    HttpResponse response(200, R"({"id": "test"})");

    EXPECT_EQ(response.status_code, 200);
    EXPECT_EQ(response.body, R"({"id": "test"})");
    EXPECT_TRUE(response.success);
}

TEST_F(VercelApiTest, HttpResponseSuccess) {
    HttpResponse success(201, "Created");
    EXPECT_TRUE(success.success);

    HttpResponse failure(404, "Not Found");
    EXPECT_FALSE(failure.success);

    HttpResponse serverError(500, "Server Error");
    EXPECT_FALSE(serverError.success);
}

// ============================================================================
// HttpClient Tests
// ============================================================================

TEST_F(VercelApiTest, HttpClientCreation) {
    HttpClient client;

    // Should be able to create without errors
    EXPECT_TRUE(true);
}

TEST_F(VercelApiTest, HttpClientSetTimeout) {
    HttpClient client;
    client.setTimeout(60);

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(VercelApiTest, HttpClientSetUserAgent) {
    HttpClient client;
    client.setUserAgent("ElizaOS/1.0");

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(VercelApiTest, HttpClientSetBearerToken) {
    HttpClient client;
    client.setBearerToken("test-token");

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(VercelApiTest, HttpClientAddDefaultHeader) {
    HttpClient client;
    client.addDefaultHeader("X-Custom-Header", "value");

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(VercelApiTest, HttpClientUrlEncode) {
    HttpClient client;

    std::string encoded = client.urlEncode("hello world");
    EXPECT_FALSE(encoded.empty());
}

TEST_F(VercelApiTest, HttpClientJsonEscape) {
    HttpClient client;

    std::string escaped = client.jsonEscape("hello \"world\"");
    EXPECT_FALSE(escaped.empty());
}

// ============================================================================
// VercelDeployment Tests
// ============================================================================

TEST_F(VercelApiTest, VercelDeploymentCreation) {
    VercelDeployment deployment;
    deployment.id = "dpl-789";
    deployment.url = "https://my-app-abc123.vercel.app";
    deployment.project_id = "prj-123";
    deployment.state = "READY";
    deployment.target = "PRODUCTION";

    EXPECT_EQ(deployment.id, "dpl-789");
    EXPECT_EQ(deployment.state, "READY");
    EXPECT_EQ(deployment.target, "PRODUCTION");
}

TEST_F(VercelApiTest, VercelDeploymentConstructor) {
    VercelDeployment deployment("dpl-123", "https://app.vercel.app");

    EXPECT_EQ(deployment.id, "dpl-123");
    EXPECT_EQ(deployment.url, "https://app.vercel.app");
}

TEST_F(VercelApiTest, VercelDeploymentStates) {
    VercelDeployment ready;
    ready.state = "READY";
    EXPECT_TRUE(ready.isReady());
    EXPECT_FALSE(ready.hasError());
    EXPECT_FALSE(ready.isBuilding());

    VercelDeployment building;
    building.state = "BUILDING";
    EXPECT_FALSE(building.isReady());
    EXPECT_FALSE(building.hasError());
    EXPECT_TRUE(building.isBuilding());

    VercelDeployment error;
    error.state = "ERROR";
    EXPECT_FALSE(error.isReady());
    EXPECT_TRUE(error.hasError());
}

TEST_F(VercelApiTest, VercelDeploymentGitInfo) {
    VercelDeployment deployment;
    deployment.id = "dpl-789";
    deployment.git_branch = "main";
    deployment.git_commit_sha = "abc123def456";
    deployment.git_commit_message = "Fix: authentication bug";

    EXPECT_EQ(deployment.git_commit_sha, "abc123def456");
    EXPECT_EQ(deployment.git_branch, "main");
}

TEST_F(VercelApiTest, VercelDeploymentDomains) {
    VercelDeployment deployment;
    deployment.id = "dpl-789";
    deployment.domains = {"my-app.vercel.app", "custom-domain.com"};

    EXPECT_EQ(deployment.domains.size(), 2);
    EXPECT_THAT(deployment.domains, Contains("custom-domain.com"));
}

// ============================================================================
// VercelProject Tests
// ============================================================================

TEST_F(VercelApiTest, VercelProjectCreation) {
    VercelProject project;
    project.id = "prj-123456";
    project.name = "my-app";
    project.framework = "nextjs";
    project.account_id = "acc-789";

    EXPECT_EQ(project.id, "prj-123456");
    EXPECT_EQ(project.name, "my-app");
    EXPECT_EQ(project.framework, "nextjs");
}

TEST_F(VercelApiTest, VercelProjectConstructor) {
    VercelProject project("prj-123", "my-project");

    EXPECT_EQ(project.id, "prj-123");
    EXPECT_EQ(project.name, "my-project");
}

TEST_F(VercelApiTest, VercelProjectDomains) {
    VercelProject project;
    project.id = "prj-123456";
    project.domains = {"my-app.vercel.app", "custom-domain.com"};

    EXPECT_EQ(project.domains.size(), 2);
    EXPECT_THAT(project.domains, Contains("custom-domain.com"));
}

TEST_F(VercelApiTest, VercelProjectEnvVars) {
    VercelProject project;
    project.id = "prj-123456";
    project.env_vars["DATABASE_URL"] = "postgres://...";
    project.env_vars["API_KEY"] = "secret-key";

    EXPECT_EQ(project.env_vars.size(), 2);
}

TEST_F(VercelApiTest, VercelProjectBuildConfig) {
    VercelProject project;
    project.id = "prj-123";
    project.build_command = "npm run build";
    project.install_command = "npm install";
    project.output_directory = "dist";
    project.root_directory = "./";
    project.node_version = "18.x";

    EXPECT_EQ(project.build_command, "npm run build");
    EXPECT_EQ(project.node_version, "18.x");
}

// ============================================================================
// DeploymentFile Tests
// ============================================================================

TEST_F(VercelApiTest, DeploymentFileCreation) {
    DeploymentFile file;
    file.path = "/src/index.js";
    file.content = "console.log('hello');";
    file.encoding = "utf-8";

    EXPECT_EQ(file.path, "/src/index.js");
    EXPECT_EQ(file.encoding, "utf-8");
}

TEST_F(VercelApiTest, DeploymentFileConstructor) {
    DeploymentFile file("/index.html", "<html></html>");

    EXPECT_EQ(file.path, "/index.html");
    EXPECT_EQ(file.content, "<html></html>");
    EXPECT_EQ(file.size, 13);  // strlen("<html></html>")
}

// ============================================================================
// DeploymentRequest Tests
// ============================================================================

TEST_F(VercelApiTest, DeploymentRequestCreation) {
    DeploymentRequest request;
    request.name = "my-deployment";
    request.target = "PRODUCTION";
    request.project_id = "prj-123";

    EXPECT_EQ(request.name, "my-deployment");
    EXPECT_EQ(request.target, "PRODUCTION");
}

TEST_F(VercelApiTest, DeploymentRequestConstructor) {
    DeploymentRequest request("test-deployment");

    EXPECT_EQ(request.name, "test-deployment");
}

TEST_F(VercelApiTest, DeploymentRequestWithFiles) {
    DeploymentRequest request("my-app");

    DeploymentFile file1("/index.html", "<html></html>");
    DeploymentFile file2("/style.css", "body {}");
    request.files.push_back(file1);
    request.files.push_back(file2);

    EXPECT_EQ(request.files.size(), 2);
}

TEST_F(VercelApiTest, DeploymentRequestEnvVars) {
    DeploymentRequest request("my-app");
    request.env_vars["NODE_ENV"] = "production";
    request.build_env["CI"] = "true";

    EXPECT_EQ(request.env_vars.size(), 1);
    EXPECT_EQ(request.build_env.size(), 1);
}

// ============================================================================
// VercelDomain Tests
// ============================================================================

TEST_F(VercelApiTest, VercelDomainCreation) {
    VercelDomain domain;
    domain.name = "example.com";
    domain.apex_name = "example.com";
    domain.project_id = "prj-123456";
    domain.verified = true;

    EXPECT_EQ(domain.name, "example.com");
    EXPECT_TRUE(domain.verified);
}

TEST_F(VercelApiTest, VercelDomainConstructor) {
    VercelDomain domain("mydomain.com");

    EXPECT_EQ(domain.name, "mydomain.com");
}

TEST_F(VercelApiTest, VercelDomainVerificationChallenges) {
    VercelDomain domain;
    domain.name = "example.com";
    domain.verification_challenges = {"challenge1", "challenge2"};

    EXPECT_EQ(domain.verification_challenges.size(), 2);
}

// ============================================================================
// VercelAPI Tests
// ============================================================================

TEST_F(VercelApiTest, ApiCreation) {
    VercelAPI api(config_);

    EXPECT_FALSE(api.hasError());
}

TEST_F(VercelApiTest, ApiGetConfig) {
    VercelAPI api(config_);

    const VercelConfig& cfg = api.getConfig();
    EXPECT_EQ(cfg.api_token, "test-token");
}

TEST_F(VercelApiTest, ApiUpdateConfig) {
    VercelAPI api(config_);

    VercelConfig newConfig;
    newConfig.api_token = "new-token";
    api.updateConfig(newConfig);

    EXPECT_EQ(api.getConfig().api_token, "new-token");
}

TEST_F(VercelApiTest, ApiClearError) {
    VercelAPI api(config_);

    api.clearError();
    EXPECT_FALSE(api.hasError());
}

TEST_F(VercelApiTest, ApiError) {
    VercelAPI::ApiError error(401, "Unauthorized");

    EXPECT_EQ(error.code, 401);
    EXPECT_EQ(error.message, "Unauthorized");
}

// ============================================================================
// VercelIntegration Tests
// ============================================================================

TEST_F(VercelApiTest, IntegrationCreation) {
    VercelIntegration integration(config_);

    EXPECT_FALSE(integration.isInitialized());
}

TEST_F(VercelApiTest, IntegrationGetConfig) {
    VercelIntegration integration(config_);

    const VercelConfig& cfg = integration.getConfig();
    EXPECT_EQ(cfg.api_token, "test-token");
}

TEST_F(VercelApiTest, IntegrationGetAPI) {
    VercelIntegration integration(config_);
    integration.initialize();

    auto api = integration.getAPI();
    EXPECT_NE(api, nullptr);
}

TEST_F(VercelApiTest, IntegrationUpdateConfig) {
    VercelIntegration integration(config_);

    VercelConfig newConfig;
    newConfig.api_token = "updated-token";
    integration.updateConfig(newConfig);

    EXPECT_EQ(integration.getConfig().api_token, "updated-token");
}

