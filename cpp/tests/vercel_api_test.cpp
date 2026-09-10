#include <gtest/gtest.h>
#include "elizaos/vercel_api.hpp"

#include <nlohmann/json.hpp>

#include <atomic>
#include <deque>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace elizaos;
using json = nlohmann::json;

namespace {
class FakeHttp final : public HttpAdapter {
public:
    HttpResponse perform(const HttpRequest& request) override {
        std::lock_guard<std::mutex> lock(mutex);
        requests.push_back(request);
        if (responses.empty()) { HttpResponse response; response.error_message = "no fake response"; return response; }
        auto response = responses.front();
        responses.pop_front();
        return response;
    }
    void push(int code, const std::string& body, std::unordered_map<std::string, std::string> headers = {}) {
        HttpResponse response(code, body); response.headers = std::move(headers);
        std::lock_guard<std::mutex> lock(mutex); responses.push_back(std::move(response));
    }
    std::vector<HttpRequest> snapshot() const { std::lock_guard<std::mutex> lock(mutex); return requests; }
private:
    mutable std::mutex mutex;
    std::deque<HttpResponse> responses;
    std::vector<HttpRequest> requests;
};

VercelConfig config() {
    VercelConfig value("token-DO-NOT-LEAK", "team /alpha");
    value.api_base_url = "https://unit.invalid";
    value.poll_interval_ms = 1;
    value.timeout_seconds = 1;
    return value;
}
}

TEST(VercelTruthBoundary, DefaultClientsRejectWithoutContactOrFabricatedIds) {
    HttpClient transport;
    const auto response = transport.get("https://never.invalid");
    EXPECT_FALSE(response.success);
    EXPECT_EQ(response.status_code, 0);
    EXPECT_EQ(response.error_message, "HTTP adapter not configured");

    VercelAPI api(config());
    EXPECT_FALSE(api.authenticate());
    EXPECT_EQ(api.getLastStatus().code, VercelStatusCode::transport_error);
    EXPECT_EQ(api.getLastError().message.find("token-DO-NOT-LEAK"), std::string::npos);
    EXPECT_TRUE(api.createWebhook("project-1", "https://hook.invalid/event", {"deployment.ready"}) == false);
    EXPECT_TRUE(api.getKnownWebhookIds().empty());

    VercelIntegration integration(config());
    EXPECT_FALSE(integration.initialize());
    EXPECT_FALSE(integration.isInitialized());
    EXPECT_TRUE(integration.deployGitRepository("https://git.invalid/repo.git").id.empty());
}

TEST(VercelHttpAdapter, CapturesExactAuthHeadersUrlAndResponseIdentity) {
    auto fake = std::make_shared<FakeHttp>();
    fake->push(200, R"({"id":"user-1"})", {{"X-Vercel-Id", "request-9"}});
    VercelAPI api(config(), fake);
    EXPECT_TRUE(api.authenticate());
    const auto requests = fake->snapshot();
    ASSERT_EQ(requests.size(), 1U);
    EXPECT_EQ(requests[0].method, "GET");
    EXPECT_EQ(requests[0].url, "https://unit.invalid/v2/user?teamId=team%20%2Falpha");
    EXPECT_EQ(requests[0].headers.at("Authorization"), "Bearer token-DO-NOT-LEAK");
    EXPECT_EQ(requests[0].headers.at("Accept"), "application/json");
    EXPECT_EQ(requests[0].headers.at("User-Agent"), "ElizaOS-CPP/1.0");
    EXPECT_EQ(api.getLastStatus().request_id, "request-9");
}

TEST(VercelProjects, MapsPagesAndCapturesExactCreateJsonAndStableIdempotency) {
    auto fake = std::make_shared<FakeHttp>();
    fake->push(200, R"({"projects":[{"id":"prj-1","name":"one","framework":"nextjs","createdAt":1000}],"pagination":{"next":"cursor 2"}})");
    fake->push(201, R"({"id":"prj-2","name":"two","framework":"static"})");
    fake->push(201, R"({"id":"prj-2","name":"two","framework":"static"})");
    VercelAPI api(config(), fake);
    auto page = api.listProjectsPage("cursor 1", 2);
    ASSERT_TRUE(page);
    ASSERT_EQ(page.value.items.size(), 1U);
    EXPECT_EQ(page.value.items[0].framework, "nextjs");
    EXPECT_EQ(page.value.next_cursor, "cursor 2");
    auto created = api.createProjectResult("two", "static");
    auto repeated = api.createProjectResult("two", "static");
    ASSERT_TRUE(created);
    ASSERT_TRUE(repeated);
    const auto requests = fake->snapshot();
    EXPECT_EQ(requests[0].url, "https://unit.invalid/v2/projects?limit=2&teamId=team%20%2Falpha&until=cursor%201");
    EXPECT_EQ(json::parse(requests[1].body), json({{"framework", "static"}, {"name", "two"}}));
    EXPECT_FALSE(requests[1].headers.at("Idempotency-Key").empty());
    EXPECT_EQ(requests[1].headers.at("Idempotency-Key"), requests[2].headers.at("Idempotency-Key"));
}

TEST(VercelProjects, RejectsInvalidInputsHttpFailuresAndMalformedSchemasWithoutSecrets) {
    auto fake = std::make_shared<FakeHttp>();
    VercelAPI api(config(), fake);
    EXPECT_FALSE(api.createProjectResult("Bad Name", "static"));
    EXPECT_TRUE(fake->snapshot().empty());
    fake->push(403, R"({"error":{"message":"denied"}})", {{"x-request-id", "r-403"}});
    EXPECT_TRUE(api.getProject("prj-1").id.empty());
    EXPECT_EQ(api.getLastStatus().http_status, 403);
    EXPECT_EQ(api.getLastStatus().request_id, "r-403");
    EXPECT_EQ(api.getLastError().message.find("token-DO-NOT-LEAK"), std::string::npos);
    fake->push(200, R"({"name":"missing-id"})");
    EXPECT_TRUE(api.getProject("prj-1").id.empty());
    EXPECT_EQ(api.getLastStatus().code, VercelStatusCode::schema_error);
}

TEST(VercelDeployments, ValidatesFilesMapsResponseAndRejectsReadyRegression) {
    auto fake = std::make_shared<FakeHttp>();
    fake->push(201, R"({"id":"dpl-1","url":"app.vercel.app","readyState":"BUILDING","projectId":"prj-1","meta":{"githubCommitSha":"abc"}})");
    fake->push(200, R"({"id":"dpl-1","url":"app.vercel.app","readyState":"READY"})");
    fake->push(200, R"({"id":"dpl-1","url":"app.vercel.app","readyState":"BUILDING"})");
    VercelAPI api(config(), fake);
    DeploymentRequest request("valid-app");
    request.files.emplace_back("src/index.js", "console.log(1)");
    request.project_id = "prj-1";
    request.build_env["CI"] = "1";
    auto created = api.createDeploymentResult(request);
    ASSERT_TRUE(created);
    EXPECT_EQ(created.value.git_commit_sha, "abc");
    EXPECT_TRUE(api.getDeploymentResult("dpl-1"));
    auto regressed = api.getDeploymentResult("dpl-1");
    EXPECT_FALSE(regressed);
    EXPECT_EQ(regressed.status.code, VercelStatusCode::conflict);
    const auto body = json::parse(fake->snapshot()[0].body);
    EXPECT_EQ(body.at("files")[0].at("file"), "src/index.js");
    EXPECT_EQ(body.at("buildEnv").at("CI"), "1");

    request.files[0].path = "../secret";
    EXPECT_FALSE(api.createDeploymentResult(request));
    request.files[0].path = "/absolute";
    EXPECT_FALSE(api.createDeploymentResult(request));
    request.files[0].path = "src\\evil";
    EXPECT_FALSE(api.createDeploymentResult(request));
}

TEST(VercelFilesDomainsEnvironmentLogs, CoversEveryOperationFamily) {
    auto fake = std::make_shared<FakeHttp>();
    fake->push(201, R"({"id":"file-1"})");
    fake->push(200, R"({"domains":[{"name":"example.com","verified":true}],"pagination":{"next":"c2"}})");
    fake->push(201, R"({"name":"example.com","projectId":"prj-1","verified":false})");
    fake->push(200, R"({"verified":true})");
    fake->push(204, "{}");
    fake->push(201, "{}");
    fake->push(200, R"({"envs":[{"key":"A","value":"B"}]})");
    fake->push(204, "{}");
    fake->push(200, R"({"events":[{"created":"1","type":"build","text":"Building"},{"created":"2","type":"info","text":"Ready"}]})");
    fake->push(200, R"({"events":[{"created":"1","type":"stdout","text":"compiled"},{"created":"2","type":"info","text":"ignore"}]})");
    VercelAPI api(config(), fake);
    EXPECT_EQ(api.uploadFile("index.html", "abc"), "file-1");
    auto domains = api.listDomainsPage("c 1", 4);
    ASSERT_TRUE(domains); EXPECT_EQ(domains.value.items[0].name, "example.com"); EXPECT_EQ(domains.value.next_cursor, "c2");
    EXPECT_TRUE(api.addDomainResult("example.com", "prj-1"));
    EXPECT_TRUE(api.verifyDomain("example.com"));
    EXPECT_TRUE(api.removeDomain("example.com"));
    EXPECT_TRUE(api.setEnvironmentVariable("prj-1", "A", "secret-value", "preview"));
    EXPECT_EQ(api.getEnvironmentVariables("prj-1").at("A"), "B");
    EXPECT_TRUE(api.removeEnvironmentVariable("prj-1", "A"));
    EXPECT_NE(api.getDeploymentLogs("dpl-1").find("Ready"), std::string::npos);
    const auto build_logs = api.getBuildLogs("dpl-1");
    EXPECT_NE(build_logs.find("compiled"), std::string::npos);
    EXPECT_EQ(build_logs.find("ignore"), std::string::npos);
    const auto requests = fake->snapshot();
    EXPECT_EQ(json::parse(requests[0].body).at("data"), "YWJj");
    EXPECT_EQ(json::parse(requests[5].body).at("target")[0], "preview");
    EXPECT_EQ(api.getLastError().message.find("secret-value"), std::string::npos);
}

TEST(VercelWebhooks, RequiresReturnedIdAndMaintainsThreadSafeRegistry) {
    auto fake = std::make_shared<FakeHttp>();
    fake->push(201, R"({"id":"hook-1"})");
    fake->push(201, R"({})");
    fake->push(204, "{}");
    VercelAPI api(config(), fake);
    auto created = api.createWebhookResult("prj-1", "https://hook.invalid/callback", {"deployment.ready"});
    ASSERT_TRUE(created);
    EXPECT_EQ(created.value, "hook-1");
    EXPECT_EQ(api.getKnownWebhookIds(), std::vector<std::string>({"hook-1"}));
    EXPECT_FALSE(api.createWebhook("prj-1", "https://hook.invalid/callback", {"deployment.ready"}));
    EXPECT_EQ(api.getKnownWebhookIds(), std::vector<std::string>({"hook-1"}));
    EXPECT_TRUE(api.deleteWebhook("hook-1"));
    EXPECT_TRUE(api.getKnownWebhookIds().empty());
}

TEST(VercelConcurrency, ConcurrentReadWriteStatusAndRegistryAccessCompletes) {
    auto fake = std::make_shared<FakeHttp>();
    for (int i = 0; i < 80; ++i) fake->push(200, R"({"projects":[]})");
    VercelAPI api(config(), fake);
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t) threads.emplace_back([&api] {
        for (int i = 0; i < 10; ++i) {
            (void)api.listProjects();
            (void)api.getLastStatus();
            (void)api.getKnownWebhookIds();
        }
    });
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(fake->snapshot().size(), 80U);
}
