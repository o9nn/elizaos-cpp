#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace elizaos {

/**
 * Vercel API Module - Integration with Vercel platform for serverless deployments
 * Provides deployment automation and management for Vercel projects
 */

class HttpClient;
struct VercelDeployment;
struct VercelProject;

/**
 * Vercel API configuration
 */
struct VercelConfig {
    std::string api_token;
    std::string team_id;
    std::string api_base_url = "https://api.vercel.com";
    std::string api_version = "v2";
    int timeout_seconds = 30;
    int max_retries = 3;
    bool enable_logging = true;
    
    VercelConfig() = default;
    VercelConfig(const std::string& token) : api_token(token) {}
    VercelConfig(const std::string& token, const std::string& team) 
        : api_token(token), team_id(team) {}
};

/**
 * HTTP response structure
 */
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

/**
 * HTTP client for REST API operations
 */
class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    // HTTP methods
    HttpResponse get(const std::string& url, const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse post(const std::string& url, const std::string& data, 
                     const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse put(const std::string& url, const std::string& data,
                    const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse del(const std::string& url, const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse patch(const std::string& url, const std::string& data,
                      const std::unordered_map<std::string, std::string>& headers = {});
    
    // Configuration
    void setTimeout(int seconds);
    void setUserAgent(const std::string& user_agent);
    void setFollowRedirects(bool follow);
    void setMaxRetries(int retries);
    
    // Authentication
    void setBearerToken(const std::string& token);
    void setBasicAuth(const std::string& username, const std::string& password);
    void addDefaultHeader(const std::string& key, const std::string& value);
    
    // Utility
    std::string urlEncode(const std::string& data) const;
    std::string jsonEscape(const std::string& data) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Helper method
    HttpResponse performRequest(const std::string& method, const std::string& url, 
                               const std::string& data, 
                               const std::unordered_map<std::string, std::string>& headers);
};

/**
 * Vercel deployment information
 */
struct VercelDeployment {
    std::string id;
    std::string url;
    std::string project_id;
    std::string state;  // BUILDING, READY, ERROR, CANCELED
    std::string type;   // LAMBDAS
    std::string target; // PRODUCTION, PREVIEW
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point ready_at;
    std::string git_branch;
    std::string git_commit_sha;
    std::string git_commit_message;
    std::vector<std::string> domains;
    std::unordered_map<std::string, std::string> env_vars;
    std::unordered_map<std::string, std::string> build_env;
    
    VercelDeployment() = default;
    VercelDeployment(const std::string& deployment_id, const std::string& deployment_url)
        : id(deployment_id), url(deployment_url), created_at(std::chrono::system_clock::now()) {}
        
    bool isReady() const { return state == "READY"; }
    bool hasError() const { return state == "ERROR"; }
    bool isBuilding() const { return state == "BUILDING"; }
};

/**
 * Vercel project information
 */
struct VercelProject {
    std::string id;
    std::string name;
    std::string framework;
    std::string account_id;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
    std::vector<std::string> domains;
    std::unordered_map<std::string, std::string> env_vars;
    std::string git_repository;
    std::string root_directory;
    std::string build_command;
    std::string install_command;
    std::string output_directory;
    std::string node_version;
    
    VercelProject() = default;
    VercelProject(const std::string& project_id, const std::string& project_name)
        : id(project_id), name(project_name), created_at(std::chrono::system_clock::now()) {}
};

/**
 * File entry for deployment uploads
 */
struct DeploymentFile {
    std::string path;
    std::string content;
    std::string encoding = "utf-8"; // or "base64"
    std::string sha;
    size_t size = 0;
    
    DeploymentFile() = default;
    DeploymentFile(const std::string& file_path, const std::string& file_content)
        : path(file_path), content(file_content), size(file_content.size()) {}
};

/**
 * Deployment request structure
 */
struct DeploymentRequest {
    std::string name;
    std::vector<DeploymentFile> files;
    std::string target = "PRODUCTION"; // or "PREVIEW"
    std::string project_id;
    std::unordered_map<std::string, std::string> env_vars;
    std::unordered_map<std::string, std::string> build_env;
    std::string git_source;
    bool force_new_deployment = false;
    
    DeploymentRequest() = default;
    DeploymentRequest(const std::string& deployment_name) : name(deployment_name) {}
};

/**
 * Domain configuration
 */
struct VercelDomain {
    std::string name;
    std::string apex_name;
    std::string project_id;
    std::string git_branch;
    bool verified = false;
    std::chrono::system_clock::time_point created_at;
    std::vector<std::string> verification_challenges;
    
    VercelDomain() = default;
    VercelDomain(const std::string& domain_name) : name(domain_name) {}
};

/**
 * Main Vercel API client
 */
class VercelAPI {
public:
    VercelAPI(const VercelConfig& config);
    ~VercelAPI();
    
    // Authentication and setup
    bool authenticate();
    bool validateCredentials();
    
    // Project management
    std::vector<VercelProject> listProjects();
    VercelProject getProject(const std::string& project_id);
    VercelProject createProject(const std::string& name, const std::string& framework = "");
    bool deleteProject(const std::string& project_id);
    bool updateProject(const VercelProject& project);
    
    // Deployment operations
    VercelDeployment createDeployment(const DeploymentRequest& request);
    VercelDeployment getDeployment(const std::string& deployment_id);
    std::vector<VercelDeployment> listDeployments(const std::string& project_id = "");
    bool deleteDeployment(const std::string& deployment_id);
    bool cancelDeployment(const std::string& deployment_id);
    
    // File operations
    bool uploadFiles(const std::vector<DeploymentFile>& files);
    std::string uploadFile(const std::string& file_path, const std::string& content);
    bool downloadDeploymentFiles(const std::string& deployment_id, const std::string& output_dir);
    
    // Domain management
    std::vector<VercelDomain> listDomains();
    VercelDomain addDomain(const std::string& domain_name, const std::string& project_id);
    bool removeDomain(const std::string& domain_name);
    bool verifyDomain(const std::string& domain_name);
    
    // Environment variables
    bool setEnvironmentVariable(const std::string& project_id, const std::string& key, 
                               const std::string& value, const std::string& target = "production");
    bool removeEnvironmentVariable(const std::string& project_id, const std::string& key);
    std::unordered_map<std::string, std::string> getEnvironmentVariables(const std::string& project_id);
    
    // Monitoring and logs
    std::string getDeploymentLogs(const std::string& deployment_id);
    std::string getBuildLogs(const std::string& deployment_id);
    bool waitForDeployment(const std::string& deployment_id, int timeout_seconds = 300);
    
    // Webhook management
    bool createWebhook(const std::string& project_id, const std::string& url, 
                      const std::vector<std::string>& events);
    bool deleteWebhook(const std::string& webhook_id);
    
    // Configuration
    const VercelConfig& getConfig() const { return config_; }
    void updateConfig(const VercelConfig& config) { config_ = config; }
    
    // Status and error handling
    struct ApiError {
        int code = 0;
        std::string message;
        std::string details;
        std::chrono::system_clock::time_point timestamp;
        
        ApiError() = default;
        ApiError(int error_code, const std::string& error_message)
            : code(error_code), message(error_message), timestamp(std::chrono::system_clock::now()) {}
    };
    
    ApiError getLastError() const { return last_error_; }
    bool hasError() const { return last_error_.code != 0; }
    void clearError() { last_error_ = ApiError{}; }
    
private:
    VercelConfig config_;
    std::shared_ptr<HttpClient> http_client_;
    ApiError last_error_;
    
    // Helper methods
    std::string buildApiUrl(const std::string& endpoint) const;
    std::unordered_map<std::string, std::string> getDefaultHeaders() const;
    bool handleApiResponse(const HttpResponse& response, const std::string& operation);
    std::string serializeJson(const std::unordered_map<std::string, std::string>& data) const;
    std::unordered_map<std::string, std::string> parseJson(const std::string& json) const;
    std::string calculateFileSha(const std::string& content) const;
    std::string encodeBase64(const std::string& data) const;
};

/**
 * High-level Vercel integration interface
 */
class VercelIntegration {
public:
    VercelIntegration(const VercelConfig& config);
    ~VercelIntegration();
    
    // Initialization
    bool initialize();
    bool isInitialized() const { return initialized_; }
    
    // Quick deployment
    VercelDeployment deployDirectory(const std::string& directory_path, 
                                   const std::string& project_name = "",
                                   bool production = true);
    VercelDeployment deployGitRepository(const std::string& git_url, 
                                       const std::string& project_name = "",
                                       const std::string& branch = "main");
    
    // Project workflow
    bool setupProject(const std::string& project_name, const std::string& framework = "static");
    bool configureProject(const std::string& project_id, 
                         const std::unordered_map<std::string, std::string>& env_vars);
    bool linkDomain(const std::string& project_id, const std::string& domain);
    
    // Monitoring
    bool monitorDeployment(const std::string& deployment_id, 
                          std::function<void(const std::string&)> progress_callback = nullptr);
    std::vector<VercelDeployment> getRecentDeployments(const std::string& project_id, int limit = 10);
    
    // Automation
    bool enableContinuousDeployment(const std::string& project_id, const std::string& git_branch);
    bool disableContinuousDeployment(const std::string& project_id);
    
    // Component access
    std::shared_ptr<VercelAPI> getAPI() const { return api_; }
    
    // Configuration
    const VercelConfig& getConfig() const { return config_; }
    bool updateConfig(const VercelConfig& config);
    
private:
    VercelConfig config_;
    std::shared_ptr<VercelAPI> api_;
    bool initialized_ = false;
    
    bool validateEnvironment();
    std::vector<DeploymentFile> scanDirectory(const std::string& directory_path);
    bool isValidProjectName(const std::string& name) const;
    std::string generateProjectName(const std::string& base_name) const;
};

} // namespace elizaos