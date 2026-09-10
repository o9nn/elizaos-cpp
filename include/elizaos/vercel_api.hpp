#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

struct VercelConfig {
    std::string api_token;
    std::string team_id;
    std::string api_base_url = "https://api.vercel.com";
    std::string api_version = "v2";
    int timeout_seconds = 30;
    int max_retries = 3;
    bool enable_logging = true;
    std::size_t max_file_size = 10U * 1024U * 1024U;
    std::size_t max_files = 1000U;
    int poll_interval_ms = 1000;

    VercelConfig() = default;
    explicit VercelConfig(const std::string& token) : api_token(token) {}
    VercelConfig(const std::string& token, const std::string& team)
        : api_token(token), team_id(team) {}
};

struct HttpResponse {
    int status_code = 0;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
    std::chrono::milliseconds response_time{0};
    bool success = false;
    std::string error_message;
    HttpResponse() = default;
    HttpResponse(int code, const std::string& response_body)
        : status_code(code), body(response_body), success(code >= 200 && code < 300) {}
};

struct HttpRequest {
    std::string method;
    std::string url;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
    int timeout_seconds = 30;
};

/** Explicit transport truth boundary. No adapter is installed by default. */
class HttpAdapter {
public:
    virtual ~HttpAdapter() = default;
    virtual HttpResponse perform(const HttpRequest& request) = 0;
};

class HttpClient {
public:
    HttpClient();
    explicit HttpClient(std::shared_ptr<HttpAdapter> adapter);
    ~HttpClient();
    HttpResponse get(const std::string& url, const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse post(const std::string& url, const std::string& data, const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse put(const std::string& url, const std::string& data, const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse del(const std::string& url, const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse patch(const std::string& url, const std::string& data, const std::unordered_map<std::string, std::string>& headers = {});
    void setAdapter(std::shared_ptr<HttpAdapter> adapter);
    bool hasAdapter() const;
    void setTimeout(int seconds);
    void setUserAgent(const std::string& user_agent);
    void setFollowRedirects(bool follow);
    void setMaxRetries(int retries);
    void setBearerToken(const std::string& token);
    void setBasicAuth(const std::string& username, const std::string& password);
    void addDefaultHeader(const std::string& key, const std::string& value);
    std::string urlEncode(const std::string& data) const;
    std::string jsonEscape(const std::string& data) const;
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    HttpResponse performRequest(const std::string& method, const std::string& url, const std::string& data,
                                const std::unordered_map<std::string, std::string>& headers);
};

struct VercelDeployment {
    std::string id, url, project_id, state, type, target;
    std::chrono::system_clock::time_point created_at, ready_at;
    std::string git_branch, git_commit_sha, git_commit_message;
    std::vector<std::string> domains;
    std::unordered_map<std::string, std::string> env_vars, build_env;
    VercelDeployment() = default;
    VercelDeployment(const std::string& deployment_id, const std::string& deployment_url)
        : id(deployment_id), url(deployment_url), created_at(std::chrono::system_clock::now()) {}
    bool isReady() const { return state == "READY"; }
    bool hasError() const { return state == "ERROR" || state == "CANCELED"; }
    bool isBuilding() const { return state == "BUILDING" || state == "QUEUED" || state == "INITIALIZING"; }
};

struct VercelProject {
    std::string id, name, framework, account_id;
    std::chrono::system_clock::time_point created_at, updated_at;
    std::vector<std::string> domains;
    std::unordered_map<std::string, std::string> env_vars;
    std::string git_repository, root_directory, build_command, install_command, output_directory, node_version;
    VercelProject() = default;
    VercelProject(const std::string& project_id, const std::string& project_name)
        : id(project_id), name(project_name), created_at(std::chrono::system_clock::now()) {}
};

struct DeploymentFile {
    std::string path, content;
    std::string encoding = "utf-8";
    std::string sha;
    std::size_t size = 0;
    DeploymentFile() = default;
    DeploymentFile(const std::string& file_path, const std::string& file_content)
        : path(file_path), content(file_content), size(file_content.size()) {}
};

struct DeploymentRequest {
    std::string name;
    std::vector<DeploymentFile> files;
    std::string target = "PRODUCTION";
    std::string project_id;
    std::unordered_map<std::string, std::string> env_vars, build_env;
    std::string git_source;
    bool force_new_deployment = false;
    DeploymentRequest() = default;
    explicit DeploymentRequest(const std::string& deployment_name) : name(deployment_name) {}
};

struct VercelDomain {
    std::string name, apex_name, project_id, git_branch;
    bool verified = false;
    std::chrono::system_clock::time_point created_at;
    std::vector<std::string> verification_challenges;
    VercelDomain() = default;
    explicit VercelDomain(const std::string& domain_name) : name(domain_name) {}
};

enum class VercelStatusCode { ok, invalid_argument, not_configured, transport_error, http_error,
                              schema_error, conflict, timeout, io_error, unsupported };
struct VercelStatus {
    VercelStatusCode code = VercelStatusCode::ok;
    int http_status = 0;
    std::string message, request_id;
    bool ok() const { return code == VercelStatusCode::ok; }
    explicit operator bool() const { return ok(); }
};
template <typename T> struct VercelResult {
    VercelStatus status;
    T value{};
    bool ok() const { return status.ok(); }
    explicit operator bool() const { return ok(); }
};
template <typename T> struct VercelPage { std::vector<T> items; std::string next_cursor; };

class DeploymentAdapter {
public:
    virtual ~DeploymentAdapter() = default;
    virtual VercelStatus validateCredentials() = 0;
    virtual VercelResult<VercelDeployment> createDeployment(const DeploymentRequest& request) = 0;
    virtual VercelResult<VercelDeployment> getDeployment(const std::string& deployment_id) = 0;
    virtual VercelResult<std::vector<VercelDeployment>> listDeployments(const std::string& project_id, int limit) = 0;
    virtual VercelStatus configureContinuousDeployment(const std::string& project_id, const std::string& git_branch, bool enabled) = 0;
};
class GitAdapter {
public:
    virtual ~GitAdapter() = default;
    virtual VercelResult<DeploymentRequest> prepareDeployment(const std::string& git_url,
        const std::string& project_name, const std::string& branch) = 0;
};

class VercelAPI {
public:
    explicit VercelAPI(const VercelConfig& config);
    VercelAPI(const VercelConfig& config, std::shared_ptr<HttpAdapter> http_adapter);
    ~VercelAPI();
    bool authenticate();
    bool validateCredentials();
    VercelStatus authenticateResult();
    std::vector<VercelProject> listProjects();
    VercelResult<VercelPage<VercelProject>> listProjectsPage(const std::string& cursor = "", int limit = 100);
    VercelProject getProject(const std::string& project_id);
    VercelResult<VercelProject> getProjectResult(const std::string& project_id);
    VercelProject createProject(const std::string& name, const std::string& framework = "");
    VercelResult<VercelProject> createProjectResult(const std::string& name, const std::string& framework = "");
    bool deleteProject(const std::string& project_id);
    bool updateProject(const VercelProject& project);
    VercelDeployment createDeployment(const DeploymentRequest& request);
    VercelResult<VercelDeployment> createDeploymentResult(const DeploymentRequest& request);
    VercelDeployment getDeployment(const std::string& deployment_id);
    VercelResult<VercelDeployment> getDeploymentResult(const std::string& deployment_id);
    std::vector<VercelDeployment> listDeployments(const std::string& project_id = "");
    VercelResult<VercelPage<VercelDeployment>> listDeploymentsPage(const std::string& project_id = "", const std::string& cursor = "", int limit = 100);
    bool deleteDeployment(const std::string& deployment_id);
    bool cancelDeployment(const std::string& deployment_id);
    bool uploadFiles(const std::vector<DeploymentFile>& files);
    std::string uploadFile(const std::string& file_path, const std::string& content);
    VercelResult<std::string> uploadFileResult(const DeploymentFile& file);
    bool downloadDeploymentFiles(const std::string& deployment_id, const std::string& output_dir);
    std::vector<VercelDomain> listDomains();
    VercelResult<VercelPage<VercelDomain>> listDomainsPage(const std::string& cursor = "", int limit = 100);
    VercelDomain addDomain(const std::string& domain_name, const std::string& project_id);
    VercelResult<VercelDomain> addDomainResult(const std::string& domain_name, const std::string& project_id);
    bool removeDomain(const std::string& domain_name);
    bool verifyDomain(const std::string& domain_name);
    bool setEnvironmentVariable(const std::string& project_id, const std::string& key, const std::string& value, const std::string& target = "production");
    bool removeEnvironmentVariable(const std::string& project_id, const std::string& key);
    std::unordered_map<std::string, std::string> getEnvironmentVariables(const std::string& project_id);
    std::string getDeploymentLogs(const std::string& deployment_id);
    std::string getBuildLogs(const std::string& deployment_id);
    bool waitForDeployment(const std::string& deployment_id, int timeout_seconds = 300);
    bool createWebhook(const std::string& project_id, const std::string& url, const std::vector<std::string>& events);
    VercelResult<std::string> createWebhookResult(const std::string& project_id, const std::string& url, const std::vector<std::string>& events);
    bool deleteWebhook(const std::string& webhook_id);
    std::vector<std::string> getKnownWebhookIds() const;
    const VercelConfig& getConfig() const { return config_; }
    VercelConfig getConfigSnapshot() const;
    void updateConfig(const VercelConfig& config);
    void setHttpAdapter(std::shared_ptr<HttpAdapter> adapter);
    bool hasHttpAdapter() const;
    struct ApiError {
        int code = 0;
        std::string message, details;
        std::chrono::system_clock::time_point timestamp;
        ApiError() = default;
        ApiError(int error_code, const std::string& error_message)
            : code(error_code), message(error_message), timestamp(std::chrono::system_clock::now()) {}
    };
    ApiError getLastError() const;
    VercelStatus getLastStatus() const;
    bool hasError() const;
    void clearError();
private:
    struct Impl;
    VercelConfig config_;
    std::shared_ptr<HttpClient> http_client_;
    std::unique_ptr<Impl> pImpl_;
    std::string buildApiUrl(const std::string& endpoint) const;
    std::unordered_map<std::string, std::string> getDefaultHeaders() const;
    bool handleApiResponse(const HttpResponse& response, const std::string& operation);
    std::string serializeJson(const std::unordered_map<std::string, std::string>& data) const;
    std::unordered_map<std::string, std::string> parseJson(const std::string& json) const;
    std::string calculateFileSha(const std::string& content) const;
    std::string encodeBase64(const std::string& data) const;
    void setStatus(const VercelStatus& status);
};

class VercelIntegration {
public:
    explicit VercelIntegration(const VercelConfig& config);
    VercelIntegration(const VercelConfig& config, std::shared_ptr<HttpAdapter> http_adapter,
                      std::shared_ptr<DeploymentAdapter> deployment_adapter = nullptr,
                      std::shared_ptr<GitAdapter> git_adapter = nullptr);
    ~VercelIntegration();
    bool initialize();
    bool isInitialized() const;
    VercelDeployment deployDirectory(const std::string& directory_path, const std::string& project_name = "", bool production = true);
    VercelResult<VercelDeployment> deployDirectoryResult(const std::string& directory_path, const std::string& project_name = "", bool production = true);
    VercelDeployment deployGitRepository(const std::string& git_url, const std::string& project_name = "", const std::string& branch = "main");
    VercelResult<VercelDeployment> deployGitRepositoryResult(const std::string& git_url, const std::string& project_name = "", const std::string& branch = "main");
    VercelResult<std::vector<DeploymentFile>> packageDirectory(const std::string& directory_path) const;
    bool setupProject(const std::string& project_name, const std::string& framework = "static");
    bool configureProject(const std::string& project_id, const std::unordered_map<std::string, std::string>& env_vars);
    bool linkDomain(const std::string& project_id, const std::string& domain);
    bool monitorDeployment(const std::string& deployment_id, std::function<void(const std::string&)> progress_callback = nullptr);
    std::vector<VercelDeployment> getRecentDeployments(const std::string& project_id, int limit = 10);
    bool enableContinuousDeployment(const std::string& project_id, const std::string& git_branch);
    bool disableContinuousDeployment(const std::string& project_id);
    std::shared_ptr<VercelAPI> getAPI() const;
    const VercelConfig& getConfig() const { return config_; }
    VercelConfig getConfigSnapshot() const;
    VercelStatus getLastStatus() const;
    bool updateConfig(const VercelConfig& config);
    void setDeploymentAdapter(std::shared_ptr<DeploymentAdapter> adapter);
    void setGitAdapter(std::shared_ptr<GitAdapter> adapter);
private:
    struct Impl;
    VercelConfig config_;
    std::shared_ptr<VercelAPI> api_;
    std::unique_ptr<Impl> pImpl_;
    bool validateEnvironment();
    std::vector<DeploymentFile> scanDirectory(const std::string& directory_path);
    bool isValidProjectName(const std::string& name) const;
    std::string generateProjectName(const std::string& base_name) const;
};

} // namespace elizaos
