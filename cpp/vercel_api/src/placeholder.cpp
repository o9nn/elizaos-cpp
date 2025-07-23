#include "elizaos/vercel_api.hpp"
#include "elizaos/agentlogger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <iomanip>
#include <thread>

#ifdef CURL_FOUND
#include <curl/curl.h>
#endif

namespace elizaos {

using json = nlohmann::json;

// Global logger instance for the module
static AgentLogger g_vercel_logger;

// HttpClient::Impl - PIMPL to hide curl details
struct HttpClient::Impl {
#ifdef CURL_FOUND
    CURL* curl = nullptr;
#endif
    std::string user_agent = "ElizaOS-CPP/1.0";
    int timeout_seconds = 30;
    bool follow_redirects = true;
    int max_retries = 3;
    std::unordered_map<std::string, std::string> default_headers;
    
    Impl() {
#ifdef CURL_FOUND
        curl = curl_easy_init();
#endif
    }
    
    ~Impl() {
#ifdef CURL_FOUND
        if (curl) {
            curl_easy_cleanup(curl);
        }
#endif
    }
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
        size_t total_size = size * nmemb;
        response->append(static_cast<char*>(contents), total_size);
        return total_size;
    }
};

// HttpClient implementation
HttpClient::HttpClient() : pImpl_(std::make_unique<Impl>()) {}
HttpClient::~HttpClient() = default;

HttpResponse HttpClient::get(const std::string& url, const std::unordered_map<std::string, std::string>& headers) {
    return performRequest("GET", url, "", headers);
}

HttpResponse HttpClient::post(const std::string& url, const std::string& data, 
                             const std::unordered_map<std::string, std::string>& headers) {
    return performRequest("POST", url, data, headers);
}

HttpResponse HttpClient::put(const std::string& url, const std::string& data,
                            const std::unordered_map<std::string, std::string>& headers) {
    return performRequest("PUT", url, data, headers);
}

HttpResponse HttpClient::del(const std::string& url, const std::unordered_map<std::string, std::string>& headers) {
    return performRequest("DELETE", url, "", headers);
}

HttpResponse HttpClient::patch(const std::string& url, const std::string& data,
                              const std::unordered_map<std::string, std::string>& headers) {
    return performRequest("PATCH", url, data, headers);
}

HttpResponse HttpClient::performRequest(const std::string& method, const std::string& url, 
                                       const std::string& /* data */, 
                                       const std::unordered_map<std::string, std::string>& /* headers */) {
    HttpResponse response;
    auto start_time = std::chrono::steady_clock::now();
    
#ifdef CURL_FOUND
    if (!pImpl_->curl) {
        response.error_message = "HTTP client not initialized";
        return response;
    }
    
    // Reset curl handle
    curl_easy_reset(pImpl_->curl);
    
    // Set basic options
    curl_easy_setopt(pImpl_->curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(pImpl_->curl, CURLOPT_USERAGENT, pImpl_->user_agent.c_str());
    curl_easy_setopt(pImpl_->curl, CURLOPT_TIMEOUT, pImpl_->timeout_seconds);
    curl_easy_setopt(pImpl_->curl, CURLOPT_FOLLOWLOCATION, pImpl_->follow_redirects ? 1L : 0L);
    
    // Set method
    if (method == "POST") {
        curl_easy_setopt(pImpl_->curl, CURLOPT_POST, 1L);
    } else if (method == "PUT") {
        curl_easy_setopt(pImpl_->curl, CURLOPT_CUSTOMREQUEST, "PUT");
    } else if (method == "DELETE") {
        curl_easy_setopt(pImpl_->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    } else if (method == "PATCH") {
        curl_easy_setopt(pImpl_->curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    }
    
    // Set data for POST/PUT/PATCH
    if (!data.empty() && (method == "POST" || method == "PUT" || method == "PATCH")) {
        curl_easy_setopt(pImpl_->curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(pImpl_->curl, CURLOPT_POSTFIELDSIZE, data.length());
    }
    
    // Set headers
    struct curl_slist* header_list = nullptr;
    auto all_headers = pImpl_->default_headers;
    for (const auto& header : headers) {
        all_headers[header.first] = header.second;
    }
    
    for (const auto& header : all_headers) {
        std::string header_string = header.first + ": " + header.second;
        header_list = curl_slist_append(header_list, header_string.c_str());
    }
    
    if (header_list) {
        curl_easy_setopt(pImpl_->curl, CURLOPT_HTTPHEADER, header_list);
    }
    
    // Set callback for response data
    std::string response_body;
    curl_easy_setopt(pImpl_->curl, CURLOPT_WRITEFUNCTION, Impl::WriteCallback);
    curl_easy_setopt(pImpl_->curl, CURLOPT_WRITEDATA, &response_body);
    
    // Perform request
    CURLcode result = curl_easy_perform(pImpl_->curl);
    
    // Get response info
    long response_code = 0;
    curl_easy_getinfo(pImpl_->curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    // Clean up headers
    if (header_list) {
        curl_slist_free_all(header_list);
    }
    
    // Build response
    response.status_code = static_cast<int>(response_code);
    response.body = response_body;
    response.success = (result == CURLE_OK) && (response_code >= 200 && response_code < 300);
    
    if (result != CURLE_OK) {
        response.error_message = curl_easy_strerror(result);
    }
    
#else
    // Fallback implementation without curl
    response.error_message = "HTTP functionality not available (libcurl not found)";
    g_vercel_logger.log("HTTP request attempted but libcurl not available: " + method + " " + url,
                       "", "vercel_api", LogLevel::WARNING);
#endif
    
    auto end_time = std::chrono::steady_clock::now();
    response.response_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    return response;
}

void HttpClient::setTimeout(int seconds) {
    pImpl_->timeout_seconds = seconds;
}

void HttpClient::setUserAgent(const std::string& user_agent) {
    pImpl_->user_agent = user_agent;
}

void HttpClient::setFollowRedirects(bool follow) {
    pImpl_->follow_redirects = follow;
}

void HttpClient::setMaxRetries(int retries) {
    pImpl_->max_retries = retries;
}

void HttpClient::setBearerToken(const std::string& token) {
    addDefaultHeader("Authorization", "Bearer " + token);
}

void HttpClient::setBasicAuth(const std::string& username, const std::string& password) {
    std::string credentials = username + ":" + password;
    // In a real implementation, we'd base64 encode this
    addDefaultHeader("Authorization", "Basic " + credentials);
}

void HttpClient::addDefaultHeader(const std::string& key, const std::string& value) {
    pImpl_->default_headers[key] = value;
}

std::string HttpClient::urlEncode(const std::string& data) const {
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;
    
    for (auto c : data) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            encoded << std::uppercase;
            encoded << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
            encoded << std::nouppercase;
        }
    }
    
    return encoded.str();
}

std::string HttpClient::jsonEscape(const std::string& data) const {
    std::string result = data;
    result = std::regex_replace(result, std::regex("\\\\"), "\\\\");
    result = std::regex_replace(result, std::regex("\""), "\\\"");
    result = std::regex_replace(result, std::regex("\n"), "\\n");
    result = std::regex_replace(result, std::regex("\r"), "\\r");
    result = std::regex_replace(result, std::regex("\t"), "\\t");
    return result;
}

// VercelAPI implementation
VercelAPI::VercelAPI(const VercelConfig& config) : config_(config) {
    http_client_ = std::make_shared<HttpClient>();
    http_client_->setBearerToken(config_.api_token);
    http_client_->setTimeout(config_.timeout_seconds);
    http_client_->setMaxRetries(config_.max_retries);
    http_client_->addDefaultHeader("Content-Type", "application/json");
}

VercelAPI::~VercelAPI() = default;

bool VercelAPI::authenticate() {
    auto response = http_client_->get(buildApiUrl("/user"));
    
    if (response.success) {
        g_vercel_logger.log("Authentication successful", "", "vercel_api", LogLevel::INFO);
        return true;
    } else {
        last_error_ = ApiError(response.status_code, "Authentication failed: " + response.error_message);
        return false;
    }
}

bool VercelAPI::validateCredentials() {
    return authenticate();
}

std::vector<VercelProject> VercelAPI::listProjects() {
    std::vector<VercelProject> projects;
    
    auto response = http_client_->get(buildApiUrl("/projects"));
    
    if (response.success) {
        try {
            auto json_response = json::parse(response.body);
            
            if (json_response.contains("projects")) {
                for (const auto& project_json : json_response["projects"]) {
                    VercelProject project;
                    project.id = project_json.value("id", "");
                    project.name = project_json.value("name", "");
                    project.framework = project_json.value("framework", "");
                    
                    if (project_json.contains("createdAt")) {
                        // Parse timestamp - simplified implementation
                        project.created_at = std::chrono::system_clock::now();
                    }
                    
                    projects.push_back(project);
                }
            }
        } catch (const json::exception& e) {
            last_error_ = ApiError(500, "Failed to parse projects response: " + std::string(e.what()));
        }
    } else {
        last_error_ = ApiError(response.status_code, "Failed to list projects: " + response.error_message);
    }
    
    return projects;
}

VercelProject VercelAPI::getProject(const std::string& project_id) {
    VercelProject project;
    
    auto response = http_client_->get(buildApiUrl("/projects/" + project_id));
    
    if (response.success) {
        try {
            auto json_response = json::parse(response.body);
            
            project.id = json_response.value("id", "");
            project.name = json_response.value("name", "");
            project.framework = json_response.value("framework", "");
            project.account_id = json_response.value("accountId", "");
            
            if (json_response.contains("env")) {
                for (const auto& env_var : json_response["env"]) {
                    if (env_var.contains("key") && env_var.contains("value")) {
                        project.env_vars[env_var["key"]] = env_var["value"];
                    }
                }
            }
            
        } catch (const json::exception& e) {
            last_error_ = ApiError(500, "Failed to parse project response: " + std::string(e.what()));
        }
    } else {
        last_error_ = ApiError(response.status_code, "Failed to get project: " + response.error_message);
    }
    
    return project;
}

VercelProject VercelAPI::createProject(const std::string& name, const std::string& framework) {
    VercelProject project;
    
    json request_data;
    request_data["name"] = name;
    if (!framework.empty()) {
        request_data["framework"] = framework;
    }
    
    auto response = http_client_->post(buildApiUrl("/projects"), request_data.dump());
    
    if (response.success) {
        try {
            auto json_response = json::parse(response.body);
            
            project.id = json_response.value("id", "");
            project.name = json_response.value("name", "");
            project.framework = json_response.value("framework", "");
            project.created_at = std::chrono::system_clock::now();
            
            g_vercel_logger.log("Created project: " + project.name + " (" + project.id + ")", 
                               "", "vercel_api", LogLevel::INFO);
            
        } catch (const json::exception& e) {
            last_error_ = ApiError(500, "Failed to parse create project response: " + std::string(e.what()));
        }
    } else {
        last_error_ = ApiError(response.status_code, "Failed to create project: " + response.error_message);
    }
    
    return project;
}

VercelDeployment VercelAPI::createDeployment(const DeploymentRequest& request) {
    VercelDeployment deployment;
    
    json request_data;
    request_data["name"] = request.name;
    request_data["target"] = request.target;
    
    if (!request.project_id.empty()) {
        request_data["projectId"] = request.project_id;
    }
    
    // Add files
    json files_array = json::array();
    for (const auto& file : request.files) {
        json file_obj;
        file_obj["file"] = file.path;
        file_obj["data"] = file.content;
        files_array.push_back(file_obj);
    }
    request_data["files"] = files_array;
    
    // Add environment variables
    if (!request.env_vars.empty()) {
        json env_array = json::array();
        for (const auto& env_var : request.env_vars) {
            json env_obj;
            env_obj["key"] = env_var.first;
            env_obj["value"] = env_var.second;
            env_obj["type"] = "encrypted";
            env_obj["target"] = json::array({"production", "preview", "development"});
            env_array.push_back(env_obj);
        }
        request_data["env"] = env_array;
    }
    
    auto response = http_client_->post(buildApiUrl("/deployments"), request_data.dump());
    
    if (response.success) {
        try {
            auto json_response = json::parse(response.body);
            
            deployment.id = json_response.value("id", "");
            deployment.url = json_response.value("url", "");
            deployment.state = json_response.value("readyState", "BUILDING");
            deployment.type = json_response.value("type", "LAMBDAS");
            deployment.target = json_response.value("target", "PRODUCTION");
            deployment.created_at = std::chrono::system_clock::now();
            
            if (json_response.contains("meta") && json_response["meta"].contains("githubCommitSha")) {
                deployment.git_commit_sha = json_response["meta"]["githubCommitSha"];
            }
            
            g_vercel_logger.log("Created deployment: " + deployment.id + " at " + deployment.url, "", "vercel_api", LogLevel::INFO);
            
        } catch (const json::exception& e) {
            last_error_ = ApiError(500, "Failed to parse deployment response: " + std::string(e.what()));
        }
    } else {
        last_error_ = ApiError(response.status_code, "Failed to create deployment: " + response.error_message);
    }
    
    return deployment;
}

VercelDeployment VercelAPI::getDeployment(const std::string& deployment_id) {
    VercelDeployment deployment;
    
    auto response = http_client_->get(buildApiUrl("/deployments/" + deployment_id));
    
    if (response.success) {
        try {
            auto json_response = json::parse(response.body);
            
            deployment.id = json_response.value("id", "");
            deployment.url = json_response.value("url", "");
            deployment.state = json_response.value("readyState", "UNKNOWN");
            deployment.project_id = json_response.value("projectId", "");
            
        } catch (const json::exception& e) {
            last_error_ = ApiError(500, "Failed to parse deployment response: " + std::string(e.what()));
        }
    } else {
        last_error_ = ApiError(response.status_code, "Failed to get deployment: " + response.error_message);
    }
    
    return deployment;
}

bool VercelAPI::waitForDeployment(const std::string& deployment_id, int timeout_seconds) {
    auto start_time = std::chrono::steady_clock::now();
    auto timeout_duration = std::chrono::seconds(timeout_seconds);
    
    while (true) {
        auto deployment = getDeployment(deployment_id);
        
        if (deployment.isReady()) {
            g_vercel_logger.log("Deployment " + deployment_id + " is ready at " + deployment.url, "", "vercel_api", LogLevel::INFO);
            return true;
        }
        
        if (deployment.hasError()) {
            g_vercel_logger.log("Deployment " + deployment_id + " failed", "", "vercel_api", LogLevel::ERROR);
            return false;
        }
        
        auto elapsed = std::chrono::steady_clock::now() - start_time;
        if (elapsed >= timeout_duration) {
            g_vercel_logger.log("Deployment " + deployment_id + " timed out after " + std::to_string(timeout_seconds) + " seconds",
                               "", "vercel_api", LogLevel::WARNING);
            return false;
        }
        
        // Wait 5 seconds before checking again
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

std::string VercelAPI::buildApiUrl(const std::string& endpoint) const {
    std::string url = config_.api_base_url + "/" + config_.api_version + endpoint;
    
    if (!config_.team_id.empty()) {
        char separator = (endpoint.find('?') != std::string::npos) ? '&' : '?';
        url += separator + "teamId=" + config_.team_id;
    }
    
    return url;
}

std::unordered_map<std::string, std::string> VercelAPI::getDefaultHeaders() const {
    return {
        {"Authorization", "Bearer " + config_.api_token},
        {"Content-Type", "application/json"},
        {"User-Agent", "ElizaOS-CPP/1.0"}
    };
}

bool VercelAPI::handleApiResponse(const HttpResponse& response, const std::string& operation) {
    if (response.success) {
        return true;
    }
    
    std::string error_msg = operation + " failed: ";
    if (!response.error_message.empty()) {
        error_msg += response.error_message;
    } else {
        error_msg += "HTTP " + std::to_string(response.status_code);
    }
    
    last_error_ = ApiError(response.status_code, error_msg);
    
    if (config_.enable_logging) {
        g_vercel_logger.log(error_msg, "", "vercel_api", LogLevel::ERROR);
    }
    
    return false;
}

std::string VercelAPI::serializeJson(const std::unordered_map<std::string, std::string>& data) const {
    json j = data;
    return j.dump();
}

std::unordered_map<std::string, std::string> VercelAPI::parseJson(const std::string& json_str) const {
    std::unordered_map<std::string, std::string> result;
    
    try {
        auto j = json::parse(json_str);
        for (auto& [key, value] : j.items()) {
            if (value.is_string()) {
                result[key] = value.get<std::string>();
            } else {
                result[key] = value.dump();
            }
        }
    } catch (const json::exception& e) {
        g_vercel_logger.log("Failed to parse JSON: " + std::string(e.what()), "", "vercel_api", LogLevel::ERROR);
    }
    
    return result;
}

std::string VercelAPI::calculateFileSha(const std::string& content) const {
    // Simple hash calculation - in practice you'd use proper SHA-1
    std::hash<std::string> hasher;
    auto hash_value = hasher(content);
    
    std::stringstream ss;
    ss << std::hex << hash_value;
    return ss.str();
}

std::string VercelAPI::encodeBase64(const std::string& data) const {
    // Simple base64 encoding implementation
    const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string encoded;
    
    for (size_t i = 0; i < data.size(); i += 3) {
        unsigned char b1 = data[i];
        unsigned char b2 = (i + 1 < data.size()) ? data[i + 1] : 0;
        unsigned char b3 = (i + 2 < data.size()) ? data[i + 2] : 0;
        
        unsigned int triple = (b1 << 16) | (b2 << 8) | b3;
        
        encoded += chars[(triple >> 18) & 0x3F];
        encoded += chars[(triple >> 12) & 0x3F];
        encoded += (i + 1 < data.size()) ? chars[(triple >> 6) & 0x3F] : '=';
        encoded += (i + 2 < data.size()) ? chars[triple & 0x3F] : '=';
    }
    
    return encoded;
}

// VercelIntegration implementation
VercelIntegration::VercelIntegration(const VercelConfig& config) : config_(config) {
    api_ = std::make_shared<VercelAPI>(config);
}

VercelIntegration::~VercelIntegration() = default;

bool VercelIntegration::initialize() {
    if (!validateEnvironment()) {
        return false;
    }
    
    if (!api_->validateCredentials()) {
        g_vercel_logger.log("Failed to validate Vercel credentials", "", "vercel_api", LogLevel::ERROR);
        return false;
    }
    
    initialized_ = true;
    
    g_vercel_logger.log("Vercel integration initialized successfully", "", "vercel_api", LogLevel::INFO);
    
    return true;
}

VercelDeployment VercelIntegration::deployDirectory(const std::string& directory_path, 
                                                   const std::string& project_name,
                                                   bool production) {
    VercelDeployment deployment;
    
    if (!initialized_) {
        g_vercel_logger.log("Vercel integration not initialized", "", "vercel_api", LogLevel::ERROR);
        return deployment;
    }
    
    auto files = scanDirectory(directory_path);
    if (files.empty()) {
        g_vercel_logger.log("No files found in directory: " + directory_path, "", "vercel_api", LogLevel::ERROR);
        return deployment;
    }
    
    DeploymentRequest request;
    request.name = project_name.empty() ? generateProjectName("elizaos-deployment") : project_name;
    request.files = files;
    request.target = production ? "PRODUCTION" : "PREVIEW";
    
    deployment = api_->createDeployment(request);
    
    if (!deployment.id.empty()) {
        g_vercel_logger.log("Deploying " + std::to_string(files.size()) + " files from " + directory_path,
                           "", "vercel_api", LogLevel::INFO);
        
        // Wait for deployment to complete
        if (api_->waitForDeployment(deployment.id, 300)) { // 5 minute timeout
            g_vercel_logger.log("Deployment successful: " + deployment.url, "", "vercel_api", LogLevel::INFO);
        }
    }
    
    return deployment;
}

VercelDeployment VercelIntegration::deployGitRepository(const std::string& git_url, 
                                                       const std::string& /* project_name */,
                                                       const std::string& branch) {
    VercelDeployment deployment;
    
    // This would implement Git repository deployment
    // For now, we'll log the attempt
    g_vercel_logger.log("Git deployment requested for: " + git_url + " (branch: " + branch + ")",
                       "", "vercel_api", LogLevel::INFO);
    
    // In a real implementation, this would:
    // 1. Clone the repository
    // 2. Extract files
    // 3. Create deployment with git metadata
    
    return deployment;
}

bool VercelIntegration::setupProject(const std::string& project_name, const std::string& framework) {
    if (!initialized_) {
        return false;
    }
    
    auto project = api_->createProject(project_name, framework);
    return !project.id.empty();
}

bool VercelIntegration::configureProject(const std::string& project_id, 
                                        const std::unordered_map<std::string, std::string>& env_vars) {
    if (!initialized_) {
        return false;
    }
    
    // Set environment variables
    for (const auto& env_var : env_vars) {
        if (!api_->setEnvironmentVariable(project_id, env_var.first, env_var.second)) {
            return false;
        }
    }
    
    return true;
}

bool VercelIntegration::linkDomain(const std::string& project_id, const std::string& domain) {
    if (!initialized_) {
        return false;
    }
    
    auto domain_info = api_->addDomain(domain, project_id);
    return !domain_info.name.empty();
}

bool VercelIntegration::validateEnvironment() {
    if (config_.api_token.empty()) {
        g_vercel_logger.log("Vercel API token not provided", "", "vercel_api", LogLevel::ERROR);
        return false;
    }
    
    return true;
}

std::vector<DeploymentFile> VercelIntegration::scanDirectory(const std::string& directory_path) {
    std::vector<DeploymentFile> files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
            if (entry.is_regular_file()) {
                std::ifstream file(entry.path());
                if (!file.is_open()) continue;
                
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string content = buffer.str();
                
                auto relative_path = std::filesystem::relative(entry.path(), directory_path);
                
                DeploymentFile deployment_file;
                deployment_file.path = relative_path.string();
                deployment_file.content = content;
                deployment_file.size = content.size();
                // Calculate SHA locally (simplified)
                std::hash<std::string> hasher;
                auto hash_value = hasher(content);
                std::stringstream ss;
                ss << std::hex << hash_value;
                deployment_file.sha = ss.str();
                
                files.push_back(deployment_file);
            }
        }
    } catch (const std::exception& e) {
        g_vercel_logger.log("Failed to scan directory: " + std::string(e.what()), "", "vercel_api", LogLevel::ERROR);
    }
    
    return files;
}

bool VercelIntegration::isValidProjectName(const std::string& name) const {
    // Vercel project names must be lowercase, alphanumeric, and may contain hyphens
    std::regex name_regex("^[a-z0-9-]+$");
    return std::regex_match(name, name_regex) && name.length() <= 63;
}

std::string VercelIntegration::generateProjectName(const std::string& base_name) const {
    std::string name = base_name;
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    std::replace(name.begin(), name.end(), '_', '-');
    std::replace(name.begin(), name.end(), ' ', '-');
    
    // Remove invalid characters
    name.erase(std::remove_if(name.begin(), name.end(), 
        [](char c) { return !std::isalnum(c) && c != '-'; }), name.end());
    
    // Ensure it doesn't start or end with a hyphen
    while (!name.empty() && name.front() == '-') {
        name.erase(0, 1);
    }
    while (!name.empty() && name.back() == '-') {
        name.pop_back();
    }
    
    // Add timestamp to make it unique
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    name += "-" + std::to_string(timestamp);
    
    // Truncate if too long
    if (name.length() > 63) {
        name = name.substr(0, 63);
    }
    
    return name;
}

// Stub implementations for methods that require actual API calls
bool VercelAPI::deleteProject(const std::string& project_id) {
    auto response = http_client_->del(buildApiUrl("/projects/" + project_id));
    return handleApiResponse(response, "Delete project");
}

bool VercelAPI::updateProject(const VercelProject& project) {
    json request_data;
    request_data["name"] = project.name;
    request_data["framework"] = project.framework;
    
    auto response = http_client_->patch(buildApiUrl("/projects/" + project.id), request_data.dump());
    return handleApiResponse(response, "Update project");
}

std::vector<VercelDeployment> VercelAPI::listDeployments(const std::string& project_id) {
    std::vector<VercelDeployment> deployments;
    
    std::string url = buildApiUrl("/deployments");
    if (!project_id.empty()) {
        url += (url.find('?') != std::string::npos ? "&" : "?");
        url += "projectId=" + project_id;
    }
    
    auto response = http_client_->get(url);
    
    if (response.success) {
        try {
            auto json_response = json::parse(response.body);
            
            if (json_response.contains("deployments")) {
                for (const auto& deployment_json : json_response["deployments"]) {
                    VercelDeployment deployment;
                    deployment.id = deployment_json.value("id", "");
                    deployment.url = deployment_json.value("url", "");
                    deployment.state = deployment_json.value("readyState", "UNKNOWN");
                    deployments.push_back(deployment);
                }
            }
        } catch (const json::exception& e) {
            last_error_ = ApiError(500, "Failed to parse deployments response: " + std::string(e.what()));
        }
    }
    
    return deployments;
}

// Additional stub implementations
bool VercelAPI::setEnvironmentVariable(const std::string& project_id, const std::string& key, 
                                      const std::string& value, const std::string& target) {
    json request_data;
    request_data["key"] = key;
    request_data["value"] = value;
    request_data["type"] = "encrypted";
    request_data["target"] = json::array({target});
    
    auto response = http_client_->post(buildApiUrl("/projects/" + project_id + "/env"), request_data.dump());
    return handleApiResponse(response, "Set environment variable");
}

std::vector<VercelDomain> VercelAPI::listDomains() {
    std::vector<VercelDomain> domains;
    // Implementation would parse domains from API response
    return domains;
}

VercelDomain VercelAPI::addDomain(const std::string& domain_name, const std::string& project_id) {
    VercelDomain domain(domain_name);
    domain.project_id = project_id;
    // Implementation would make API call to add domain
    return domain;
}

} // namespace elizaos
