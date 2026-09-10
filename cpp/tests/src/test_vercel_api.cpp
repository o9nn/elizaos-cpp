#include <gtest/gtest.h>
#include "elizaos/vercel_api.hpp"

#include <atomic>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

using namespace elizaos;
namespace fs = std::filesystem;

namespace {
VercelStatus failed(VercelStatusCode code, const std::string& message) { VercelStatus value; value.code = code; value.message = message; return value; }

template <typename T> VercelResult<T> okResult(T value) { VercelResult<T> result; result.value = std::move(value); return result; }
template <typename T> VercelResult<T> badResult(VercelStatusCode code, const std::string& message) { VercelResult<T> result; result.status = failed(code, message); return result; }

class FakeDeployment final : public DeploymentAdapter {
public:
    VercelStatus validateCredentials() override { ++validate_calls; return validation; }
    VercelResult<VercelDeployment> createDeployment(const DeploymentRequest& request) override {
        std::lock_guard<std::mutex> lock(mutex);
        captured.push_back(request);
        return create_result;
    }
    VercelResult<VercelDeployment> getDeployment(const std::string& id) override {
        std::lock_guard<std::mutex> lock(mutex);
        queried_ids.push_back(id);
        if (states.empty()) return badResult<VercelDeployment>(VercelStatusCode::transport_error, "no state");
        VercelDeployment value = states.front(); states.erase(states.begin());
        return okResult(value);
    }
    VercelResult<std::vector<VercelDeployment>> listDeployments(const std::string& project, int limit) override {
        std::lock_guard<std::mutex> lock(mutex);
        listed_project = project; listed_limit = limit;
        return list_result;
    }
    VercelStatus configureContinuousDeployment(const std::string& project, const std::string& branch, bool enabled) override {
        std::lock_guard<std::mutex> lock(mutex);
        continuous_calls.push_back({project, branch, enabled});
        return continuous_result;
    }
    struct ContinuousCall { std::string project; std::string branch; bool enabled; };
    VercelStatus validation;
    VercelResult<VercelDeployment> create_result;
    VercelResult<std::vector<VercelDeployment>> list_result;
    VercelStatus continuous_result;
    std::vector<VercelDeployment> states;
    std::atomic<int> validate_calls{0};
    std::mutex mutex;
    std::vector<DeploymentRequest> captured;
    std::vector<std::string> queried_ids;
    std::string listed_project;
    int listed_limit = 0;
    std::vector<ContinuousCall> continuous_calls;
};

class FakeGit final : public GitAdapter {
public:
    VercelResult<DeploymentRequest> prepareDeployment(const std::string& url, const std::string& project,
                                                       const std::string& branch) override {
        ++calls; captured_url = url; captured_project = project; captured_branch = branch;
        return result;
    }
    int calls = 0;
    std::string captured_url, captured_project, captured_branch;
    VercelResult<DeploymentRequest> result;
};

class TempDirectory {
public:
    TempDirectory() {
        path = fs::temp_directory_path() / ("eliza-vercel-" + std::to_string(counter.fetch_add(1)));
        fs::create_directories(path);
    }
    ~TempDirectory() { std::error_code error; fs::remove_all(path, error); }
    fs::path path;
private:
    static std::atomic<unsigned> counter;
};
std::atomic<unsigned> TempDirectory::counter{1};

VercelConfig config() {
    VercelConfig value("integration-token");
    value.api_base_url = "https://unit.invalid";
    value.timeout_seconds = 1;
    value.poll_interval_ms = 1;
    return value;
}
VercelDeployment deployment(std::string state, int created = 0) {
    VercelDeployment value("dpl-1", "app.vercel.app");
    value.state = std::move(state);
    value.created_at = std::chrono::system_clock::time_point(std::chrono::seconds(created));
    return value;
}
}

TEST(VercelIntegration, DeploymentAdapterAuthenticatesAndDirectoryPackagingIsDeterministic) {
    TempDirectory root;
    fs::create_directories(root.path / "nested");
    std::ofstream(root.path / "z.txt", std::ios::binary) << "z";
    std::ofstream(root.path / "nested" / "a.txt", std::ios::binary) << "a\0b";

    auto backend = std::make_shared<FakeDeployment>();
    backend->create_result = okResult(deployment("BUILDING"));
    VercelIntegration integration(config(), nullptr, backend);
    ASSERT_TRUE(integration.initialize());
    EXPECT_TRUE(integration.isInitialized());
    auto packaged = integration.packageDirectory(root.path.string());
    ASSERT_TRUE(packaged);
    ASSERT_EQ(packaged.value.size(), 2U);
    EXPECT_EQ(packaged.value[0].path, "nested/a.txt");
    EXPECT_EQ(packaged.value[1].path, "z.txt");
    EXPECT_FALSE(packaged.value[0].sha.empty());

    auto deployed = integration.deployDirectoryResult(root.path.string(), "valid-app", false);
    ASSERT_TRUE(deployed);
    std::lock_guard<std::mutex> lock(backend->mutex);
    ASSERT_EQ(backend->captured.size(), 1U);
    EXPECT_EQ(backend->captured[0].name, "valid-app");
    EXPECT_EQ(backend->captured[0].target, "PREVIEW");
    EXPECT_EQ(backend->captured[0].files[0].path, "nested/a.txt");
}

TEST(VercelIntegration, PackagingRejectsMissingEmptyOversizeAndSymlinkBoundaries) {
    TempDirectory root;
    VercelIntegration integration(config());
    EXPECT_EQ(integration.packageDirectory((root.path / "missing").string()).status.code, VercelStatusCode::io_error);
    EXPECT_EQ(integration.packageDirectory(root.path.string()).status.code, VercelStatusCode::invalid_argument);

    std::ofstream(root.path / "big") << "12345";
    auto constrained = config(); constrained.max_file_size = 4;
    VercelIntegration small(constrained);
    EXPECT_EQ(small.packageDirectory(root.path.string()).status.code, VercelStatusCode::invalid_argument);

    fs::remove(root.path / "big");
    std::ofstream(root.path / "real") << "ok";
    std::error_code error;
    fs::create_symlink(root.path / "real", root.path / "link", error);
    if (!error) {
        EXPECT_EQ(integration.packageDirectory(root.path.string()).status.code,
                  VercelStatusCode::io_error);
    }
}

TEST(VercelIntegration, GitDeploymentRequiresGitAdapterAndForwardsExactPreparedRequest) {
    auto backend = std::make_shared<FakeDeployment>();
    backend->create_result = okResult(deployment("BUILDING"));
    VercelIntegration without_git(config(), nullptr, backend);
    ASSERT_TRUE(without_git.initialize());
    auto rejected = without_git.deployGitRepositoryResult("https://git.invalid/repo.git", "valid-app", "main");
    EXPECT_EQ(rejected.status.code, VercelStatusCode::not_configured);
    EXPECT_TRUE(backend->captured.empty());

    auto git = std::make_shared<FakeGit>();
    DeploymentRequest prepared("valid-app");
    prepared.files.emplace_back("index.js", "ok");
    prepared.git_source = "receipt:commit-abc";
    git->result = okResult(prepared);
    VercelIntegration integration(config(), nullptr, backend, git);
    ASSERT_TRUE(integration.initialize());
    auto deployed = integration.deployGitRepositoryResult("https://git.invalid/repo.git", "valid-app", "feature-1");
    ASSERT_TRUE(deployed);
    EXPECT_EQ(git->captured_url, "https://git.invalid/repo.git");
    EXPECT_EQ(git->captured_project, "valid-app");
    EXPECT_EQ(git->captured_branch, "feature-1");
    std::lock_guard<std::mutex> lock(backend->mutex);
    EXPECT_EQ(backend->captured.back().git_source, "receipt:commit-abc");
}

TEST(VercelIntegration, AdapterFailuresPropagateAndNeverFabricateSuccess) {
    auto backend = std::make_shared<FakeDeployment>();
    backend->validation = failed(VercelStatusCode::transport_error, "backend unavailable");
    VercelIntegration integration(config(), nullptr, backend);
    EXPECT_FALSE(integration.initialize());
    EXPECT_FALSE(integration.isInitialized());
    EXPECT_EQ(integration.getLastStatus().message, "backend unavailable");

    backend->validation = {};
    backend->create_result = badResult<VercelDeployment>(VercelStatusCode::http_error, "rejected");
    ASSERT_TRUE(integration.initialize());
    TempDirectory root; std::ofstream(root.path / "index") << "ok";
    auto result = integration.deployDirectoryResult(root.path.string(), "valid-app");
    EXPECT_FALSE(result);
    EXPECT_TRUE(result.value.id.empty());
    EXPECT_EQ(result.status.message, "rejected");
}

TEST(VercelIntegration, MonitorCallbackIsOutsideLocksAndSeesOrderedStateTransitions) {
    auto backend = std::make_shared<FakeDeployment>();
    backend->states = {deployment("BUILDING"), deployment("READY")};
    VercelIntegration integration(config(), nullptr, backend);
    ASSERT_TRUE(integration.initialize());
    std::vector<std::string> messages;
    EXPECT_TRUE(integration.monitorDeployment("dpl-1", [&](const std::string& message) {
        messages.push_back(message);
        EXPECT_TRUE(integration.isInitialized());
        EXPECT_NE(integration.getAPI(), nullptr);
    }));
    ASSERT_EQ(messages.size(), 2U);
    EXPECT_EQ(messages[0], "Deployment state: BUILDING");
    EXPECT_EQ(messages[1], "Deployment ready at: app.vercel.app");
}

TEST(VercelIntegration, ContinuousDeploymentIsAdapterAcknowledgedAndDefaultRejects) {
    auto backend = std::make_shared<FakeDeployment>();
    VercelIntegration integration(config(), nullptr, backend);
    ASSERT_TRUE(integration.initialize());
    EXPECT_TRUE(integration.enableContinuousDeployment("prj-1", "main"));
    EXPECT_TRUE(integration.disableContinuousDeployment("prj-1"));
    {
        std::lock_guard<std::mutex> lock(backend->mutex);
        ASSERT_EQ(backend->continuous_calls.size(), 2U);
        EXPECT_TRUE(backend->continuous_calls[0].enabled);
        EXPECT_EQ(backend->continuous_calls[0].branch, "main");
        EXPECT_FALSE(backend->continuous_calls[1].enabled);
    }

    backend->continuous_result = failed(VercelStatusCode::transport_error, "not acknowledged");
    EXPECT_FALSE(integration.enableContinuousDeployment("prj-1", "main"));

    class AuthHttp final : public HttpAdapter {
        HttpResponse perform(const HttpRequest&) override { return HttpResponse(200, R"({"id":"user"})"); }
    };
    VercelIntegration http_only(config(), std::make_shared<AuthHttp>());
    ASSERT_TRUE(http_only.initialize());
    EXPECT_FALSE(http_only.enableContinuousDeployment("prj-1", "main"));
    EXPECT_EQ(http_only.getLastStatus().code, VercelStatusCode::not_configured);
}

TEST(VercelIntegration, RecentDeploymentsAreSortedLimitedAndArgumentsCaptured) {
    auto backend = std::make_shared<FakeDeployment>();
    backend->list_result = okResult(std::vector<VercelDeployment>{deployment("READY", 1), deployment("READY", 3), deployment("READY", 2)});
    VercelIntegration integration(config(), nullptr, backend);
    ASSERT_TRUE(integration.initialize());
    const auto recent = integration.getRecentDeployments("prj-1", 2);
    ASSERT_EQ(recent.size(), 2U);
    EXPECT_GT(recent[0].created_at, recent[1].created_at);
    EXPECT_EQ(backend->listed_project, "prj-1");
    EXPECT_EQ(backend->listed_limit, 2);
}

TEST(VercelIntegration, ConcurrentAccessAndAdapterSwapsCompleteWithoutDeadlock) {
    auto backend = std::make_shared<FakeDeployment>();
    VercelIntegration integration(config(), nullptr, backend);
    ASSERT_TRUE(integration.initialize());
    std::atomic<int> reads{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 6; ++i) threads.emplace_back([&] {
        for (int n = 0; n < 100; ++n) {
            if (integration.getAPI() && integration.getConfigSnapshot().api_token == "integration-token") ++reads;
            (void)integration.getLastStatus();
        }
    });
    for (int i = 0; i < 20; ++i) integration.setGitAdapter(std::make_shared<FakeGit>());
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(reads.load(), 600);
}
