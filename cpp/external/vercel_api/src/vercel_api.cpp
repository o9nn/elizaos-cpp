#include "elizaos/vercel_api.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <mutex>
#include <regex>
#include <set>
#include <sstream>
#include <thread>
#include <utility>

namespace elizaos {
namespace {
using json = nlohmann::json;

VercelStatus status(VercelStatusCode code, std::string message = {}, int http_status = 0) {
    VercelStatus value;
    value.code = code;
    value.http_status = http_status;
    value.message = std::move(message);
    return value;
}

template <typename T>
VercelResult<T> failure(VercelStatusCode code, const std::string& message, int http_status = 0) {
    VercelResult<T> result;
    result.status = status(code, message, http_status);
    return result;
}

bool validIdentifier(const std::string& value) {
    static const std::regex pattern("^[A-Za-z0-9][A-Za-z0-9._-]{0,127}$");
    return std::regex_match(value, pattern);
}

bool validProjectName(const std::string& value) {
    static const std::regex pattern("^[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?$");
    return std::regex_match(value, pattern);
}

bool validEnvKey(const std::string& value) {
    static const std::regex pattern("^[A-Za-z_][A-Za-z0-9_]{0,127}$");
    return std::regex_match(value, pattern);
}

bool validTarget(const std::string& value) {
    return value == "PRODUCTION" || value == "PREVIEW";
}

bool validEnvTarget(const std::string& value) {
    return value == "production" || value == "preview" || value == "development";
}

bool validDomain(const std::string& value) {
    if (value.empty() || value.size() > 253 || value.front() == '.' || value.back() == '.') return false;
    static const std::regex pattern(
        "^(?=.{1,253}$)(?:[A-Za-z0-9](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?\\.)+"
        "[A-Za-z](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?$");
    return std::regex_match(value, pattern);
}

bool validUrl(const std::string& value, bool https_only) {
    const std::string prefix = https_only ? "https://" : "http";
    if (https_only) return value.rfind(prefix, 0) == 0 && value.size() > prefix.size();
    return (value.rfind("https://", 0) == 0 && value.size() > 8) ||
           (value.rfind("http://", 0) == 0 && value.size() > 7);
}

bool validGitUrl(const std::string& value) {
    return validUrl(value, true) || value.rfind("ssh://", 0) == 0 || value.rfind("git@", 0) == 0;
}

bool validRelativePath(const std::string& value) {
    if (value.empty() || value.find('\0') != std::string::npos || value.find('\\') != std::string::npos) return false;
    const std::filesystem::path path(value);
    if (path.is_absolute() || path.has_root_name() || path.has_root_directory()) return false;
    for (const auto& component : path) {
        if (component == ".." || component == "." || component.empty()) return false;
    }
    return path.lexically_normal().generic_string() == value;
}

bool hasString(const json& object, const char* key) {
    return object.is_object() && object.contains(key) && object.at(key).is_string() &&
           !object.at(key).get_ref<const std::string&>().empty();
}

std::chrono::system_clock::time_point fromMilliseconds(const json& object, const char* key) {
    if (!object.contains(key) || !object.at(key).is_number_integer()) return {};
    const auto millis = object.at(key).get<std::int64_t>();
    return std::chrono::system_clock::time_point(std::chrono::milliseconds(millis));
}

std::string requestId(const HttpResponse& response) {
    for (const auto& entry : response.headers) {
        std::string key = entry.first;
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (key == "x-vercel-id" || key == "x-request-id") return entry.second;
    }
    return {};
}

VercelStatus responseStatus(const HttpResponse& response, const std::string& operation) {
    if (response.status_code < 200 || response.status_code >= 300 || !response.success) {
        VercelStatus result = status(
            response.status_code == 0 ? VercelStatusCode::transport_error : VercelStatusCode::http_error,
            operation + (response.status_code == 0 ? ": transport rejected request" : ": HTTP " + std::to_string(response.status_code)),
            response.status_code);
        result.request_id = requestId(response);
        return result;
    }
    VercelStatus result;
    result.http_status = response.status_code;
    result.request_id = requestId(response);
    return result;
}

VercelStatus parseObject(const HttpResponse& response, const std::string& operation, json& output) {
    auto mapped = responseStatus(response, operation);
    if (!mapped) return mapped;
    try {
        output = json::parse(response.body);
        if (!output.is_object()) return status(VercelStatusCode::schema_error, operation + ": expected JSON object", response.status_code);
        return mapped;
    } catch (const std::exception&) {
        return status(VercelStatusCode::schema_error, operation + ": malformed JSON response", response.status_code);
    }
}

VercelProject mapProject(const json& source) {
    VercelProject project;
    project.id = source.value("id", "");
    project.name = source.value("name", "");
    project.framework = source.value("framework", "");
    project.account_id = source.value("accountId", "");
    project.created_at = fromMilliseconds(source, "createdAt");
    project.updated_at = fromMilliseconds(source, "updatedAt");
    project.git_repository = source.value("gitRepository", "");
    project.root_directory = source.value("rootDirectory", "");
    project.build_command = source.value("buildCommand", "");
    project.install_command = source.value("installCommand", "");
    project.output_directory = source.value("outputDirectory", "");
    project.node_version = source.value("nodeVersion", "");
    if (source.contains("domains") && source.at("domains").is_array()) {
        for (const auto& domain : source.at("domains")) if (domain.is_string()) project.domains.push_back(domain.get<std::string>());
    }
    return project;
}

VercelDeployment mapDeployment(const json& source) {
    VercelDeployment deployment;
    deployment.id = source.value("id", source.value("uid", ""));
    deployment.url = source.value("url", "");
    deployment.project_id = source.value("projectId", source.value("project", ""));
    deployment.state = source.value("readyState", source.value("state", ""));
    deployment.type = source.value("type", "");
    deployment.target = source.value("target", "");
    deployment.created_at = fromMilliseconds(source, "createdAt");
    deployment.ready_at = fromMilliseconds(source, "readyAt");
    if (source.contains("meta") && source.at("meta").is_object()) {
        const auto& meta = source.at("meta");
        deployment.git_branch = meta.value("githubCommitRef", meta.value("gitBranch", ""));
        deployment.git_commit_sha = meta.value("githubCommitSha", meta.value("gitCommitSha", ""));
        deployment.git_commit_message = meta.value("githubCommitMessage", "");
    }
    return deployment;
}

VercelDomain mapDomain(const json& source) {
    VercelDomain domain;
    domain.name = source.value("name", "");
    domain.apex_name = source.value("apexName", "");
    domain.project_id = source.value("projectId", "");
    domain.git_branch = source.value("gitBranch", "");
    domain.verified = source.value("verified", false);
    domain.created_at = fromMilliseconds(source, "createdAt");
    if (source.contains("verification") && source.at("verification").is_array()) {
        for (const auto& challenge : source.at("verification")) {
            if (challenge.is_string()) domain.verification_challenges.push_back(challenge.get<std::string>());
            else if (challenge.is_object()) domain.verification_challenges.push_back(challenge.value("value", ""));
        }
    }
    return domain;
}

std::string stableHash(const std::string& value) {
    std::uint64_t hash = 14695981039346656037ULL;
    for (unsigned char c : value) { hash ^= c; hash *= 1099511628211ULL; }
    std::ostringstream out;
    out << std::hex << std::setfill('0') << std::setw(16) << hash;
    return out.str();
}

std::string base64(const std::string& value) {
    static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string output;
    output.reserve(((value.size() + 2U) / 3U) * 4U);
    for (std::size_t i = 0; i < value.size(); i += 3U) {
        const auto a = static_cast<unsigned char>(value[i]);
        const auto b = i + 1U < value.size() ? static_cast<unsigned char>(value[i + 1U]) : 0U;
        const auto c = i + 2U < value.size() ? static_cast<unsigned char>(value[i + 2U]) : 0U;
        const unsigned triple = (static_cast<unsigned>(a) << 16U) | (static_cast<unsigned>(b) << 8U) | c;
        output.push_back(alphabet[(triple >> 18U) & 0x3FU]);
        output.push_back(alphabet[(triple >> 12U) & 0x3FU]);
        output.push_back(i + 1U < value.size() ? alphabet[(triple >> 6U) & 0x3FU] : '=');
        output.push_back(i + 2U < value.size() ? alphabet[triple & 0x3FU] : '=');
    }
    return output;
}

bool terminalFailure(const VercelDeployment& deployment) {
    return deployment.state == "ERROR" || deployment.state == "CANCELED";
}
} // namespace

struct HttpClient::Impl {
    mutable std::mutex mutex;
    std::shared_ptr<HttpAdapter> adapter;
    std::string user_agent = "ElizaOS-CPP/1.0";
    int timeout_seconds = 30;
    bool follow_redirects = false;
    int max_retries = 0;
    std::unordered_map<std::string, std::string> default_headers;
};

HttpClient::HttpClient() : pImpl_(std::make_unique<Impl>()) {}
HttpClient::HttpClient(std::shared_ptr<HttpAdapter> adapter) : HttpClient() { setAdapter(std::move(adapter)); }
HttpClient::~HttpClient() = default;
HttpResponse HttpClient::get(const std::string& url, const std::unordered_map<std::string, std::string>& headers) { return performRequest("GET", url, "", headers); }
HttpResponse HttpClient::post(const std::string& url, const std::string& data, const std::unordered_map<std::string, std::string>& headers) { return performRequest("POST", url, data, headers); }
HttpResponse HttpClient::put(const std::string& url, const std::string& data, const std::unordered_map<std::string, std::string>& headers) { return performRequest("PUT", url, data, headers); }
HttpResponse HttpClient::del(const std::string& url, const std::unordered_map<std::string, std::string>& headers) { return performRequest("DELETE", url, "", headers); }
HttpResponse HttpClient::patch(const std::string& url, const std::string& data, const std::unordered_map<std::string, std::string>& headers) { return performRequest("PATCH", url, data, headers); }

HttpResponse HttpClient::performRequest(const std::string& method, const std::string& url, const std::string& data,
                                        const std::unordered_map<std::string, std::string>& headers) {
    HttpRequest request;
    std::shared_ptr<HttpAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        adapter = pImpl_->adapter;
        request.method = method;
        request.url = url;
        request.body = data;
        request.headers = pImpl_->default_headers;
        for (const auto& header : headers) request.headers[header.first] = header.second;
        request.headers["User-Agent"] = pImpl_->user_agent;
        request.timeout_seconds = pImpl_->timeout_seconds;
    }
    if (!adapter) { HttpResponse response; response.error_message = "HTTP adapter not configured"; return response; }
    const auto start = std::chrono::steady_clock::now();
    HttpResponse response;
    try { response = adapter->perform(request); }
    catch (const std::exception&) { response.error_message = "HTTP adapter threw an exception"; }
    catch (...) { response.error_message = "HTTP adapter threw an unknown exception"; }
    response.success = response.status_code >= 200 && response.status_code < 300 && response.success;
    response.response_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    return response;
}

void HttpClient::setAdapter(std::shared_ptr<HttpAdapter> adapter) { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->adapter = std::move(adapter); }
bool HttpClient::hasAdapter() const { std::lock_guard<std::mutex> lock(pImpl_->mutex); return static_cast<bool>(pImpl_->adapter); }
void HttpClient::setTimeout(int seconds) { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->timeout_seconds = std::max(1, seconds); }
void HttpClient::setUserAgent(const std::string& user_agent) { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->user_agent = user_agent; }
void HttpClient::setFollowRedirects(bool follow) { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->follow_redirects = follow; }
void HttpClient::setMaxRetries(int retries) { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->max_retries = std::max(0, retries); }
void HttpClient::setBearerToken(const std::string& token) { addDefaultHeader("Authorization", "Bearer " + token); }
void HttpClient::setBasicAuth(const std::string& username, const std::string& password) { addDefaultHeader("Authorization", "Basic " + base64(username + ":" + password)); }
void HttpClient::addDefaultHeader(const std::string& key, const std::string& value) { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->default_headers[key] = value; }

std::string HttpClient::urlEncode(const std::string& data) const {
    std::ostringstream output;
    output << std::hex << std::uppercase << std::setfill('0');
    for (unsigned char c : data) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') output << static_cast<char>(c);
        else output << '%' << std::setw(2) << static_cast<unsigned>(c);
    }
    return output.str();
}
std::string HttpClient::jsonEscape(const std::string& data) const { return json(data).dump().substr(1, json(data).dump().size() - 2); }

struct VercelAPI::Impl {
    mutable std::mutex mutex;
    VercelStatus last_status;
    ApiError last_error;
    std::set<std::string> webhook_ids;
    std::unordered_map<std::string, std::string> deployment_states;
};

VercelAPI::VercelAPI(const VercelConfig& config) : VercelAPI(config, nullptr) {}
VercelAPI::VercelAPI(const VercelConfig& config, std::shared_ptr<HttpAdapter> adapter)
    : config_(config), http_client_(std::make_shared<HttpClient>(std::move(adapter))), pImpl_(std::make_unique<Impl>()) {
    http_client_->setTimeout(config.timeout_seconds);
    http_client_->setMaxRetries(config.max_retries);
    http_client_->setBearerToken(config.api_token);
    http_client_->addDefaultHeader("Accept", "application/json");
    http_client_->addDefaultHeader("Content-Type", "application/json");
}
VercelAPI::~VercelAPI() = default;

void VercelAPI::setStatus(const VercelStatus& value) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    pImpl_->last_status = value;
    if (value.ok()) pImpl_->last_error = {};
    else {
        const int code = value.http_status != 0 ? value.http_status : static_cast<int>(value.code);
        pImpl_->last_error = ApiError(code, value.message);
        pImpl_->last_error.details = value.request_id;
    }
}
VercelAPI::ApiError VercelAPI::getLastError() const { std::lock_guard<std::mutex> lock(pImpl_->mutex); return pImpl_->last_error; }
VercelStatus VercelAPI::getLastStatus() const { std::lock_guard<std::mutex> lock(pImpl_->mutex); return pImpl_->last_status; }
bool VercelAPI::hasError() const { return !getLastStatus().ok(); }
void VercelAPI::clearError() { setStatus({}); }
VercelConfig VercelAPI::getConfigSnapshot() const { std::lock_guard<std::mutex> lock(pImpl_->mutex); return config_; }
void VercelAPI::updateConfig(const VercelConfig& config) {
    {
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        config_ = config;
    }
    http_client_->setTimeout(config.timeout_seconds);
    http_client_->setMaxRetries(config.max_retries);
    http_client_->setBearerToken(config.api_token);
    clearError();
}
void VercelAPI::setHttpAdapter(std::shared_ptr<HttpAdapter> adapter) { http_client_->setAdapter(std::move(adapter)); }
bool VercelAPI::hasHttpAdapter() const { return http_client_->hasAdapter(); }

std::string VercelAPI::buildApiUrl(const std::string& endpoint) const {
    const auto cfg = getConfigSnapshot();
    std::string base = cfg.api_base_url;
    while (!base.empty() && base.back() == '/') base.pop_back();
    std::string version = cfg.api_version;
    while (!version.empty() && version.front() == '/') version.erase(version.begin());
    std::string path = endpoint.empty() || endpoint.front() == '/' ? endpoint : "/" + endpoint;
    std::string output = base + (version.empty() ? "" : "/" + version) + path;
    if (!cfg.team_id.empty()) output += (output.find('?') == std::string::npos ? "?" : "&") + std::string("teamId=") + http_client_->urlEncode(cfg.team_id);
    return output;
}
std::unordered_map<std::string, std::string> VercelAPI::getDefaultHeaders() const {
    const auto cfg = getConfigSnapshot();
    return {{"Authorization", "Bearer " + cfg.api_token}, {"Accept", "application/json"}, {"Content-Type", "application/json"}, {"User-Agent", "ElizaOS-CPP/1.0"}};
}
bool VercelAPI::handleApiResponse(const HttpResponse& response, const std::string& operation) { auto mapped = responseStatus(response, operation); setStatus(mapped); return mapped.ok(); }
std::string VercelAPI::serializeJson(const std::unordered_map<std::string, std::string>& data) const { return json(data).dump(); }
std::unordered_map<std::string, std::string> VercelAPI::parseJson(const std::string& value) const {
    std::unordered_map<std::string, std::string> output;
    try { const auto object = json::parse(value); if (object.is_object()) for (const auto& entry : object.items()) output[entry.key()] = entry.value().is_string() ? entry.value().get<std::string>() : entry.value().dump(); }
    catch (const std::exception&) {}
    return output;
}
std::string VercelAPI::calculateFileSha(const std::string& content) const { return stableHash(content); }
std::string VercelAPI::encodeBase64(const std::string& data) const { return base64(data); }

VercelStatus VercelAPI::authenticateResult() {
    const auto cfg = getConfigSnapshot();
    if (cfg.api_token.empty()) { auto value = status(VercelStatusCode::invalid_argument, "API token is required"); setStatus(value); return value; }
    if (!validUrl(cfg.api_base_url, true)) { auto value = status(VercelStatusCode::invalid_argument, "API base URL must be HTTPS"); setStatus(value); return value; }
    const auto response = http_client_->get(buildApiUrl("/user"));
    json object;
    auto value = parseObject(response, "Authenticate", object);
    if (value && !hasString(object, "id") && !hasString(object, "uid") && !hasString(object, "username")) value = status(VercelStatusCode::schema_error, "Authenticate: response lacks user identity", response.status_code);
    setStatus(value);
    return value;
}
bool VercelAPI::authenticate() { return authenticateResult().ok(); }
bool VercelAPI::validateCredentials() { return authenticate(); }

VercelResult<VercelPage<VercelProject>> VercelAPI::listProjectsPage(const std::string& cursor, int limit) {
    if (limit < 1 || limit > 100) return failure<VercelPage<VercelProject>>(VercelStatusCode::invalid_argument, "Project page limit must be 1..100");
    std::string url = buildApiUrl("/projects?limit=" + std::to_string(limit));
    if (!cursor.empty()) url += "&until=" + http_client_->urlEncode(cursor);
    const auto response = http_client_->get(url);
    json object;
    auto mapped = parseObject(response, "List projects", object);
    VercelResult<VercelPage<VercelProject>> result; result.status = mapped;
    if (mapped && (!object.contains("projects") || !object.at("projects").is_array())) result.status = status(VercelStatusCode::schema_error, "List projects: missing projects array", response.status_code);
    if (result) {
        for (const auto& entry : object.at("projects")) {
            if (!entry.is_object() || !hasString(entry, "id") || !hasString(entry, "name")) { result.status = status(VercelStatusCode::schema_error, "List projects: invalid project entry", response.status_code); result.value.items.clear(); break; }
            result.value.items.push_back(mapProject(entry));
        }
        if (object.contains("pagination") && object.at("pagination").is_object()) result.value.next_cursor = object.at("pagination").value("next", "");
    }
    setStatus(result.status); return result;
}
std::vector<VercelProject> VercelAPI::listProjects() { auto result = listProjectsPage(); return result ? result.value.items : std::vector<VercelProject>{}; }

VercelResult<VercelProject> VercelAPI::getProjectResult(const std::string& project_id) {
    if (!validIdentifier(project_id)) { auto result = failure<VercelProject>(VercelStatusCode::invalid_argument, "Invalid project ID"); setStatus(result.status); return result; }
    const auto response = http_client_->get(buildApiUrl("/projects/" + http_client_->urlEncode(project_id)));
    json object;
    VercelResult<VercelProject> result; result.status = parseObject(response, "Get project", object);
    if (result && (!hasString(object, "id") || !hasString(object, "name"))) result.status = status(VercelStatusCode::schema_error, "Get project: missing id or name", response.status_code);
    if (result) result.value = mapProject(object);
    setStatus(result.status); return result;
}
VercelProject VercelAPI::getProject(const std::string& id) { auto result = getProjectResult(id); return result ? result.value : VercelProject{}; }

VercelResult<VercelProject> VercelAPI::createProjectResult(const std::string& name, const std::string& framework) {
    if (!validProjectName(name)) { auto result = failure<VercelProject>(VercelStatusCode::invalid_argument, "Invalid project name"); setStatus(result.status); return result; }
    if (framework.size() > 64) { auto result = failure<VercelProject>(VercelStatusCode::invalid_argument, "Invalid framework"); setStatus(result.status); return result; }
    json request{{"name", name}}; if (!framework.empty()) request["framework"] = framework;
    const std::string body = request.dump();
    const auto response = http_client_->post(buildApiUrl("/projects"), body, {{"Idempotency-Key", stableHash(body)}});
    json object;
    VercelResult<VercelProject> result; result.status = parseObject(response, "Create project", object);
    if (result && (!hasString(object, "id") || !hasString(object, "name"))) result.status = status(VercelStatusCode::schema_error, "Create project: missing id or name", response.status_code);
    if (result) result.value = mapProject(object);
    setStatus(result.status); return result;
}
VercelProject VercelAPI::createProject(const std::string& name, const std::string& framework) { auto result = createProjectResult(name, framework); return result ? result.value : VercelProject{}; }

bool VercelAPI::deleteProject(const std::string& id) {
    if (!validIdentifier(id)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid project ID")); return false; }
    const auto response = http_client_->del(buildApiUrl("/projects/" + http_client_->urlEncode(id)));
    return handleApiResponse(response, "Delete project");
}
bool VercelAPI::updateProject(const VercelProject& project) {
    if (!validIdentifier(project.id) || !validProjectName(project.name)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid project update")); return false; }
    json request{{"name", project.name}}; if (!project.framework.empty()) request["framework"] = project.framework;
    if (!project.root_directory.empty()) request["rootDirectory"] = project.root_directory;
    return handleApiResponse(http_client_->patch(buildApiUrl("/projects/" + http_client_->urlEncode(project.id)), request.dump()), "Update project");
}

VercelResult<VercelDeployment> VercelAPI::createDeploymentResult(const DeploymentRequest& request) {
    if (!validProjectName(request.name) || !validTarget(request.target) || request.files.empty()) { auto result = failure<VercelDeployment>(VercelStatusCode::invalid_argument, "Invalid deployment request"); setStatus(result.status); return result; }
    if (!request.project_id.empty() && !validIdentifier(request.project_id)) { auto result = failure<VercelDeployment>(VercelStatusCode::invalid_argument, "Invalid project ID"); setStatus(result.status); return result; }
    const auto cfg = getConfigSnapshot();
    if (request.files.size() > cfg.max_files) { auto result = failure<VercelDeployment>(VercelStatusCode::invalid_argument, "Too many deployment files"); setStatus(result.status); return result; }
    json files = json::array();
    std::set<std::string> paths;
    for (const auto& file : request.files) {
        if (!validRelativePath(file.path) || file.content.size() > cfg.max_file_size || !paths.insert(file.path).second || (file.encoding != "utf-8" && file.encoding != "base64")) {
            auto result = failure<VercelDeployment>(VercelStatusCode::invalid_argument, "Invalid deployment file"); setStatus(result.status); return result;
        }
        files.push_back({{"file", file.path}, {"data", file.encoding == "base64" ? file.content : file.content}, {"encoding", file.encoding}});
    }
    json body{{"name", request.name}, {"target", request.target}, {"files", files}};
    if (!request.project_id.empty()) body["projectId"] = request.project_id;
    if (!request.env_vars.empty()) body["env"] = request.env_vars;
    if (!request.build_env.empty()) body["buildEnv"] = request.build_env;
    if (!request.git_source.empty()) body["gitSource"] = request.git_source;
    if (request.force_new_deployment) body["forceNew"] = true;
    const std::string serialized = body.dump();
    const auto response = http_client_->post(buildApiUrl("/deployments"), serialized, {{"Idempotency-Key", stableHash(serialized)}});
    json object;
    VercelResult<VercelDeployment> result; result.status = parseObject(response, "Create deployment", object);
    if (result && (!hasString(object, "id") && !hasString(object, "uid"))) result.status = status(VercelStatusCode::schema_error, "Create deployment: missing deployment ID", response.status_code);
    if (result && !hasString(object, "url")) result.status = status(VercelStatusCode::schema_error, "Create deployment: missing URL", response.status_code);
    if (result) {
        result.value = mapDeployment(object);
        if (result.value.state.empty()) result.value.state = "BUILDING";
        std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->deployment_states[result.value.id] = result.value.state;
    }
    setStatus(result.status); return result;
}
VercelDeployment VercelAPI::createDeployment(const DeploymentRequest& request) { auto result = createDeploymentResult(request); return result ? result.value : VercelDeployment{}; }

VercelResult<VercelDeployment> VercelAPI::getDeploymentResult(const std::string& id) {
    if (!validIdentifier(id)) { auto result = failure<VercelDeployment>(VercelStatusCode::invalid_argument, "Invalid deployment ID"); setStatus(result.status); return result; }
    const auto response = http_client_->get(buildApiUrl("/deployments/" + http_client_->urlEncode(id)));
    json object;
    VercelResult<VercelDeployment> result; result.status = parseObject(response, "Get deployment", object);
    if (result && (!hasString(object, "id") && !hasString(object, "uid"))) result.status = status(VercelStatusCode::schema_error, "Get deployment: missing deployment ID", response.status_code);
    if (result && !hasString(object, "readyState") && !hasString(object, "state")) result.status = status(VercelStatusCode::schema_error, "Get deployment: missing state", response.status_code);
    if (result) {
        result.value = mapDeployment(object);
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        const auto existing = pImpl_->deployment_states.find(result.value.id);
        if (existing != pImpl_->deployment_states.end() && existing->second == "READY" && result.value.state != "READY") result.status = status(VercelStatusCode::conflict, "Deployment state regressed from READY", response.status_code);
        else pImpl_->deployment_states[result.value.id] = result.value.state;
    }
    setStatus(result.status); return result;
}
VercelDeployment VercelAPI::getDeployment(const std::string& id) { auto result = getDeploymentResult(id); return result ? result.value : VercelDeployment{}; }

VercelResult<VercelPage<VercelDeployment>> VercelAPI::listDeploymentsPage(const std::string& project_id, const std::string& cursor, int limit) {
    if ((!project_id.empty() && !validIdentifier(project_id)) || limit < 1 || limit > 100) return failure<VercelPage<VercelDeployment>>(VercelStatusCode::invalid_argument, "Invalid deployment page request");
    std::string url = buildApiUrl("/deployments?limit=" + std::to_string(limit));
    if (!project_id.empty()) url += "&projectId=" + http_client_->urlEncode(project_id);
    if (!cursor.empty()) url += "&until=" + http_client_->urlEncode(cursor);
    const auto response = http_client_->get(url);
    json object;
    VercelResult<VercelPage<VercelDeployment>> result; result.status = parseObject(response, "List deployments", object);
    if (result && (!object.contains("deployments") || !object.at("deployments").is_array())) result.status = status(VercelStatusCode::schema_error, "List deployments: missing deployments array", response.status_code);
    if (result) {
        for (const auto& entry : object.at("deployments")) {
            if (!entry.is_object() || (!hasString(entry, "id") && !hasString(entry, "uid"))) { result.status = status(VercelStatusCode::schema_error, "List deployments: invalid deployment entry", response.status_code); result.value.items.clear(); break; }
            result.value.items.push_back(mapDeployment(entry));
        }
        if (object.contains("pagination") && object.at("pagination").is_object()) result.value.next_cursor = object.at("pagination").value("next", "");
    }
    setStatus(result.status); return result;
}
std::vector<VercelDeployment> VercelAPI::listDeployments(const std::string& project_id) { auto result = listDeploymentsPage(project_id); return result ? result.value.items : std::vector<VercelDeployment>{}; }

bool VercelAPI::deleteDeployment(const std::string& id) { if (!validIdentifier(id)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid deployment ID")); return false; } return handleApiResponse(http_client_->del(buildApiUrl("/deployments/" + http_client_->urlEncode(id))), "Delete deployment"); }
bool VercelAPI::cancelDeployment(const std::string& id) { if (!validIdentifier(id)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid deployment ID")); return false; } return handleApiResponse(http_client_->patch(buildApiUrl("/deployments/" + http_client_->urlEncode(id)), json{{"action", "cancel"}}.dump()), "Cancel deployment"); }

VercelResult<std::string> VercelAPI::uploadFileResult(const DeploymentFile& file) {
    const auto cfg = getConfigSnapshot();
    if (!validRelativePath(file.path) || file.content.size() > cfg.max_file_size || (file.encoding != "utf-8" && file.encoding != "base64")) { auto result = failure<std::string>(VercelStatusCode::invalid_argument, "Invalid upload file"); setStatus(result.status); return result; }
    const std::string data = file.encoding == "base64" ? file.content : encodeBase64(file.content);
    json body{{"file", file.path}, {"data", data}, {"encoding", "base64"}};
    const std::string serialized = body.dump();
    const auto response = http_client_->post(buildApiUrl("/files"), serialized, {{"Idempotency-Key", stableHash(file.path + file.content)}});
    json object;
    VercelResult<std::string> result; result.status = parseObject(response, "Upload file", object);
    if (result && !hasString(object, "id")) result.status = status(VercelStatusCode::schema_error, "Upload file: missing file ID", response.status_code);
    if (result) result.value = object.at("id").get<std::string>();
    setStatus(result.status); return result;
}
std::string VercelAPI::uploadFile(const std::string& path, const std::string& content) { auto result = uploadFileResult(DeploymentFile(path, content)); return result ? result.value : std::string{}; }
bool VercelAPI::uploadFiles(const std::vector<DeploymentFile>& files) {
    if (files.empty() || files.size() > getConfigSnapshot().max_files) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid upload file set")); return false; }
    for (const auto& file : files) if (!uploadFileResult(file)) return false;
    return true;
}

bool VercelAPI::downloadDeploymentFiles(const std::string& id, const std::string& output_dir) {
    if (!validIdentifier(id) || output_dir.empty()) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid download request")); return false; }
    std::error_code error;
    const auto root = std::filesystem::absolute(output_dir, error).lexically_normal();
    if (error) { setStatus(status(VercelStatusCode::io_error, "Invalid output directory")); return false; }
    const auto response = http_client_->get(buildApiUrl("/deployments/" + http_client_->urlEncode(id) + "/files"));
    json object;
    auto mapped = parseObject(response, "List deployment files", object);
    if (!mapped) { setStatus(mapped); return false; }
    if (!object.contains("files") || !object.at("files").is_array()) { setStatus(status(VercelStatusCode::schema_error, "List deployment files: missing files array", response.status_code)); return false; }
    struct Pending { std::filesystem::path path; std::string content; };
    std::vector<Pending> pending;
    for (const auto& item : object.at("files")) {
        if (!item.is_object() || !hasString(item, "name") || !hasString(item, "url")) { setStatus(status(VercelStatusCode::schema_error, "List deployment files: invalid entry", response.status_code)); return false; }
        const auto name = item.at("name").get<std::string>();
        const auto url = item.at("url").get<std::string>();
        if (!validRelativePath(name) || !validUrl(url, true)) { setStatus(status(VercelStatusCode::schema_error, "List deployment files: unsafe path or URL", response.status_code)); return false; }
        auto file_response = http_client_->get(url);
        auto file_status = responseStatus(file_response, "Download deployment file");
        if (!file_status) { setStatus(file_status); return false; }
        pending.push_back({root / std::filesystem::path(name), std::move(file_response.body)});
    }
    for (const auto& item : pending) {
        const auto normalized = item.path.lexically_normal();
        const auto root_string = root.generic_string() + "/";
        if (normalized.generic_string().rfind(root_string, 0) != 0) { setStatus(status(VercelStatusCode::io_error, "Download path escaped output directory")); return false; }
        std::filesystem::create_directories(normalized.parent_path(), error);
        if (error) { setStatus(status(VercelStatusCode::io_error, "Failed to create output directory")); return false; }
        std::ofstream stream(normalized, std::ios::binary | std::ios::trunc);
        if (!stream || !(stream.write(item.content.data(), static_cast<std::streamsize>(item.content.size())))) { setStatus(status(VercelStatusCode::io_error, "Failed to write deployment file")); return false; }
    }
    clearError(); return true;
}

VercelResult<VercelPage<VercelDomain>> VercelAPI::listDomainsPage(const std::string& cursor, int limit) {
    if (limit < 1 || limit > 100) return failure<VercelPage<VercelDomain>>(VercelStatusCode::invalid_argument, "Domain page limit must be 1..100");
    std::string url = buildApiUrl("/domains?limit=" + std::to_string(limit)); if (!cursor.empty()) url += "&until=" + http_client_->urlEncode(cursor);
    const auto response = http_client_->get(url);
    json object;
    VercelResult<VercelPage<VercelDomain>> result; result.status = parseObject(response, "List domains", object);
    if (result && (!object.contains("domains") || !object.at("domains").is_array())) result.status = status(VercelStatusCode::schema_error, "List domains: missing domains array", response.status_code);
    if (result) {
        for (const auto& entry : object.at("domains")) { if (!entry.is_object() || !hasString(entry, "name")) { result.status = status(VercelStatusCode::schema_error, "List domains: invalid domain entry", response.status_code); result.value.items.clear(); break; } result.value.items.push_back(mapDomain(entry)); }
        if (object.contains("pagination") && object.at("pagination").is_object()) result.value.next_cursor = object.at("pagination").value("next", "");
    }
    setStatus(result.status); return result;
}
std::vector<VercelDomain> VercelAPI::listDomains() { auto result = listDomainsPage(); return result ? result.value.items : std::vector<VercelDomain>{}; }
VercelResult<VercelDomain> VercelAPI::addDomainResult(const std::string& name, const std::string& project_id) {
    if (!validDomain(name) || !validIdentifier(project_id)) { auto result = failure<VercelDomain>(VercelStatusCode::invalid_argument, "Invalid domain request"); setStatus(result.status); return result; }
    json body{{"name", name}, {"projectId", project_id}};
    const auto response = http_client_->post(buildApiUrl("/domains"), body.dump(), {{"Idempotency-Key", stableHash(name + project_id)}});
    json object;
    VercelResult<VercelDomain> result; result.status = parseObject(response, "Add domain", object);
    if (result && !hasString(object, "name")) result.status = status(VercelStatusCode::schema_error, "Add domain: missing name", response.status_code);
    if (result) { result.value = mapDomain(object); if (result.value.project_id.empty()) result.value.project_id = project_id; }
    setStatus(result.status); return result;
}
VercelDomain VercelAPI::addDomain(const std::string& name, const std::string& project_id) { auto result = addDomainResult(name, project_id); return result ? result.value : VercelDomain{}; }
bool VercelAPI::removeDomain(const std::string& name) { if (!validDomain(name)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid domain")); return false; } return handleApiResponse(http_client_->del(buildApiUrl("/domains/" + http_client_->urlEncode(name))), "Remove domain"); }
bool VercelAPI::verifyDomain(const std::string& name) {
    if (!validDomain(name)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid domain")); return false; }
    const auto response = http_client_->post(buildApiUrl("/domains/" + http_client_->urlEncode(name) + "/verify"), json{{"method", "TXT"}}.dump());
    json object; auto mapped = parseObject(response, "Verify domain", object);
    if (mapped && (!object.contains("verified") || !object.at("verified").is_boolean())) mapped = status(VercelStatusCode::schema_error, "Verify domain: missing verified boolean", response.status_code);
    if (mapped && !object.at("verified").get<bool>()) mapped = status(VercelStatusCode::conflict, "Domain verification is pending", response.status_code);
    setStatus(mapped); return mapped.ok();
}

bool VercelAPI::setEnvironmentVariable(const std::string& project_id, const std::string& key, const std::string& value, const std::string& target_name) {
    if (!validIdentifier(project_id) || !validEnvKey(key) || value.size() > 65536U || !validEnvTarget(target_name)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid environment variable")); return false; }
    json body{{"key", key}, {"value", value}, {"type", "encrypted"}, {"target", json::array({target_name})}};
    return handleApiResponse(http_client_->post(buildApiUrl("/projects/" + http_client_->urlEncode(project_id) + "/env"), body.dump(), {{"Idempotency-Key", stableHash(project_id + key + target_name + value)}}), "Set environment variable");
}
bool VercelAPI::removeEnvironmentVariable(const std::string& project_id, const std::string& key) { if (!validIdentifier(project_id) || !validEnvKey(key)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid environment variable")); return false; } return handleApiResponse(http_client_->del(buildApiUrl("/projects/" + http_client_->urlEncode(project_id) + "/env/" + http_client_->urlEncode(key))), "Remove environment variable"); }
std::unordered_map<std::string, std::string> VercelAPI::getEnvironmentVariables(const std::string& project_id) {
    std::unordered_map<std::string, std::string> output;
    if (!validIdentifier(project_id)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid project ID")); return output; }
    const auto response = http_client_->get(buildApiUrl("/projects/" + http_client_->urlEncode(project_id) + "/env"));
    json object; auto mapped = parseObject(response, "Get environment variables", object);
    if (mapped && (!object.contains("envs") || !object.at("envs").is_array())) mapped = status(VercelStatusCode::schema_error, "Get environment variables: missing envs array", response.status_code);
    if (mapped) for (const auto& entry : object.at("envs")) {
        if (!entry.is_object() || !hasString(entry, "key")) { mapped = status(VercelStatusCode::schema_error, "Get environment variables: invalid entry", response.status_code); output.clear(); break; }
        output[entry.at("key").get<std::string>()] = entry.value("value", "");
    }
    setStatus(mapped); return output;
}

std::string VercelAPI::getDeploymentLogs(const std::string& id) {
    if (!validIdentifier(id)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid deployment ID")); return {}; }
    const auto response = http_client_->get(buildApiUrl("/deployments/" + http_client_->urlEncode(id) + "/events"));
    json object; auto mapped = parseObject(response, "Get deployment logs", object);
    if (mapped && (!object.contains("events") || !object.at("events").is_array())) mapped = status(VercelStatusCode::schema_error, "Get deployment logs: missing events array", response.status_code);
    std::ostringstream output;
    if (mapped) for (const auto& event : object.at("events")) if (event.is_object() && event.contains("text") && event.at("text").is_string()) output << '[' << event.value("created", "") << "] " << event.value("type", "") << ": " << event.at("text").get<std::string>() << '\n';
    setStatus(mapped); return mapped ? output.str() : std::string{};
}
std::string VercelAPI::getBuildLogs(const std::string& id) {
    if (!validIdentifier(id)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid deployment ID")); return {}; }
    const auto response = http_client_->get(buildApiUrl("/deployments/" + http_client_->urlEncode(id) + "/events"));
    json object; auto mapped = parseObject(response, "Get build logs", object);
    if (mapped && (!object.contains("events") || !object.at("events").is_array())) mapped = status(VercelStatusCode::schema_error, "Get build logs: missing events array", response.status_code);
    std::ostringstream output;
    if (mapped) for (const auto& event : object.at("events")) if (event.is_object()) { const std::string type = event.value("type", ""); const std::string text = event.value("text", ""); if (type == "build" || type == "stdout" || type == "stderr") output << '[' << event.value("created", "") << "] " << text << '\n'; }
    setStatus(mapped); return mapped ? output.str() : std::string{};
}
bool VercelAPI::waitForDeployment(const std::string& id, int timeout_seconds) {
    if (!validIdentifier(id) || timeout_seconds < 0) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid deployment wait request")); return false; }
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(timeout_seconds);
    do {
        auto result = getDeploymentResult(id);
        if (!result) return false;
        if (result.value.isReady()) return true;
        if (terminalFailure(result.value)) { setStatus(status(VercelStatusCode::conflict, "Deployment reached terminal failure state")); return false; }
        if (std::chrono::steady_clock::now() >= deadline) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(std::max(1, getConfigSnapshot().poll_interval_ms)));
    } while (true);
    setStatus(status(VercelStatusCode::timeout, "Deployment wait timed out")); return false;
}

VercelResult<std::string> VercelAPI::createWebhookResult(const std::string& project_id, const std::string& url, const std::vector<std::string>& events) {
    if ((!project_id.empty() && !validIdentifier(project_id)) || !validUrl(url, false) || events.empty()) { auto result = failure<std::string>(VercelStatusCode::invalid_argument, "Invalid webhook request"); setStatus(result.status); return result; }
    for (const auto& event : events) if (!validIdentifier(event) || event.find('.') == std::string::npos) { auto result = failure<std::string>(VercelStatusCode::invalid_argument, "Invalid webhook event"); setStatus(result.status); return result; }
    json body{{"url", url}, {"events", events}}; if (!project_id.empty()) body["projectId"] = project_id;
    const auto response = http_client_->post(buildApiUrl("/webhooks"), body.dump(), {{"Idempotency-Key", stableHash(body.dump())}});
    json object;
    VercelResult<std::string> result; result.status = parseObject(response, "Create webhook", object);
    if (result && !hasString(object, "id")) result.status = status(VercelStatusCode::schema_error, "Create webhook: missing webhook ID", response.status_code);
    if (result) { result.value = object.at("id").get<std::string>(); if (!validIdentifier(result.value)) result.status = status(VercelStatusCode::schema_error, "Create webhook: invalid webhook ID", response.status_code); }
    if (result) { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->webhook_ids.insert(result.value); }
    setStatus(result.status); return result;
}
bool VercelAPI::createWebhook(const std::string& project_id, const std::string& url, const std::vector<std::string>& events) { return createWebhookResult(project_id, url, events).ok(); }
bool VercelAPI::deleteWebhook(const std::string& id) {
    if (!validIdentifier(id)) { setStatus(status(VercelStatusCode::invalid_argument, "Invalid webhook ID")); return false; }
    const auto response = http_client_->del(buildApiUrl("/webhooks/" + http_client_->urlEncode(id)));
    if (!handleApiResponse(response, "Delete webhook")) return false;
    { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->webhook_ids.erase(id); }
    return true;
}
std::vector<std::string> VercelAPI::getKnownWebhookIds() const { std::lock_guard<std::mutex> lock(pImpl_->mutex); return {pImpl_->webhook_ids.begin(), pImpl_->webhook_ids.end()}; }

struct VercelIntegration::Impl {
    mutable std::mutex mutex;
    bool initialized = false;
    std::shared_ptr<HttpAdapter> http_adapter;
    std::shared_ptr<DeploymentAdapter> deployment_adapter;
    std::shared_ptr<GitAdapter> git_adapter;
    VercelStatus last_status;
};

VercelIntegration::VercelIntegration(const VercelConfig& config)
    : VercelIntegration(config, nullptr, nullptr, nullptr) {}
VercelIntegration::VercelIntegration(const VercelConfig& config, std::shared_ptr<HttpAdapter> http_adapter,
                                     std::shared_ptr<DeploymentAdapter> deployment_adapter,
                                     std::shared_ptr<GitAdapter> git_adapter)
    : config_(config), api_(std::make_shared<VercelAPI>(config, http_adapter)), pImpl_(std::make_unique<Impl>()) {
    pImpl_->http_adapter = std::move(http_adapter);
    pImpl_->deployment_adapter = std::move(deployment_adapter);
    pImpl_->git_adapter = std::move(git_adapter);
}
VercelIntegration::~VercelIntegration() = default;

bool VercelIntegration::validateEnvironment() {
    if (config_.api_token.empty()) { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->last_status = status(VercelStatusCode::invalid_argument, "API token is required"); return false; }
    if (!validUrl(config_.api_base_url, true)) { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->last_status = status(VercelStatusCode::invalid_argument, "API base URL must be HTTPS"); return false; }
    return true;
}
bool VercelIntegration::initialize() {
    if (!validateEnvironment()) return false;
    std::shared_ptr<DeploymentAdapter> deployment_adapter;
    { std::lock_guard<std::mutex> lock(pImpl_->mutex); deployment_adapter = pImpl_->deployment_adapter; }
    const VercelStatus result = deployment_adapter ? deployment_adapter->validateCredentials() : api_->authenticateResult();
    { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->last_status = result; pImpl_->initialized = result.ok(); }
    return result.ok();
}
bool VercelIntegration::isInitialized() const { std::lock_guard<std::mutex> lock(pImpl_->mutex); return pImpl_->initialized; }
std::shared_ptr<VercelAPI> VercelIntegration::getAPI() const { std::lock_guard<std::mutex> lock(pImpl_->mutex); return api_; }
VercelConfig VercelIntegration::getConfigSnapshot() const { std::lock_guard<std::mutex> lock(pImpl_->mutex); return config_; }
VercelStatus VercelIntegration::getLastStatus() const { std::lock_guard<std::mutex> lock(pImpl_->mutex); return pImpl_->last_status; }

VercelResult<std::vector<DeploymentFile>> VercelIntegration::packageDirectory(const std::string& directory_path) const {
    if (directory_path.empty()) return failure<std::vector<DeploymentFile>>(VercelStatusCode::invalid_argument, "Directory path is required");
    std::error_code error;
    const std::filesystem::path root = std::filesystem::canonical(directory_path, error);
    if (error || !std::filesystem::is_directory(root, error)) return failure<std::vector<DeploymentFile>>(VercelStatusCode::io_error, "Deployment directory does not exist");
    const auto cfg = getConfigSnapshot();
    VercelResult<std::vector<DeploymentFile>> result;
    std::filesystem::recursive_directory_iterator iterator(root, std::filesystem::directory_options::skip_permission_denied, error), end;
    for (; iterator != end && !error; iterator.increment(error)) {
        const auto& entry = *iterator;
        const auto symlink_status = entry.symlink_status(error);
        if (error) break;
        if (std::filesystem::is_symlink(symlink_status)) return failure<std::vector<DeploymentFile>>(VercelStatusCode::io_error, "Symbolic links are not allowed in deployment packages");
        if (!entry.is_regular_file(error)) continue;
        if (result.value.size() >= cfg.max_files) return failure<std::vector<DeploymentFile>>(VercelStatusCode::invalid_argument, "Deployment package exceeds file count limit");
        const auto size = entry.file_size(error);
        if (error || size > cfg.max_file_size || size > static_cast<std::uintmax_t>(std::numeric_limits<std::streamsize>::max())) return failure<std::vector<DeploymentFile>>(VercelStatusCode::invalid_argument, "Deployment file exceeds size limit");
        const auto relative = std::filesystem::relative(entry.path(), root, error).generic_string();
        if (error || !validRelativePath(relative)) return failure<std::vector<DeploymentFile>>(VercelStatusCode::io_error, "Unsafe deployment file path");
        std::ifstream stream(entry.path(), std::ios::binary);
        std::string content(static_cast<std::size_t>(size), '\0');
        if (!stream || (size != 0U && !stream.read(content.data(), static_cast<std::streamsize>(size)))) return failure<std::vector<DeploymentFile>>(VercelStatusCode::io_error, "Failed to read deployment file");
        DeploymentFile file(relative, content); file.sha = stableHash(content); file.encoding = "utf-8";
        result.value.push_back(std::move(file));
    }
    if (error) return failure<std::vector<DeploymentFile>>(VercelStatusCode::io_error, "Failed to scan deployment directory");
    if (result.value.empty()) return failure<std::vector<DeploymentFile>>(VercelStatusCode::invalid_argument, "Deployment directory contains no files");
    std::sort(result.value.begin(), result.value.end(), [](const DeploymentFile& left, const DeploymentFile& right) { return left.path < right.path; });
    return result;
}
std::vector<DeploymentFile> VercelIntegration::scanDirectory(const std::string& path) { auto result = packageDirectory(path); return result ? result.value : std::vector<DeploymentFile>{}; }
bool VercelIntegration::isValidProjectName(const std::string& name) const { return validProjectName(name); }
std::string VercelIntegration::generateProjectName(const std::string& base) const {
    std::string output;
    bool previous_hyphen = false;
    for (unsigned char c : base) {
        char mapped = static_cast<char>(std::tolower(c));
        if (std::isalnum(static_cast<unsigned char>(mapped))) { output.push_back(mapped); previous_hyphen = false; }
        else if (!output.empty() && !previous_hyphen) { output.push_back('-'); previous_hyphen = true; }
    }
    while (!output.empty() && output.back() == '-') output.pop_back();
    if (output.empty()) output = "elizaos-deployment";
    if (output.size() > 63U) output.resize(63U);
    while (!output.empty() && output.back() == '-') output.pop_back();
    return output;
}

VercelResult<VercelDeployment> VercelIntegration::deployDirectoryResult(const std::string& path, const std::string& project_name, bool production) {
    if (!isInitialized()) return failure<VercelDeployment>(VercelStatusCode::not_configured, "Vercel integration is not initialized");
    auto package = packageDirectory(path); if (!package) return failure<VercelDeployment>(package.status.code, package.status.message, package.status.http_status);
    const std::string name = project_name.empty() ? generateProjectName(std::filesystem::path(path).filename().string()) : project_name;
    if (!validProjectName(name)) return failure<VercelDeployment>(VercelStatusCode::invalid_argument, "Invalid project name");
    DeploymentRequest request(name); request.files = std::move(package.value); request.target = production ? "PRODUCTION" : "PREVIEW";
    std::shared_ptr<DeploymentAdapter> adapter;
    { std::lock_guard<std::mutex> lock(pImpl_->mutex); adapter = pImpl_->deployment_adapter; }
    auto result = adapter ? adapter->createDeployment(request) : api_->createDeploymentResult(request);
    { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->last_status = result.status; }
    return result;
}
VercelDeployment VercelIntegration::deployDirectory(const std::string& path, const std::string& name, bool production) { auto result = deployDirectoryResult(path, name, production); return result ? result.value : VercelDeployment{}; }

VercelResult<VercelDeployment> VercelIntegration::deployGitRepositoryResult(const std::string& git_url, const std::string& project_name, const std::string& branch) {
    if (!isInitialized()) return failure<VercelDeployment>(VercelStatusCode::not_configured, "Vercel integration is not initialized");
    if (!validGitUrl(git_url) || !validIdentifier(branch) || (!project_name.empty() && !validProjectName(project_name))) return failure<VercelDeployment>(VercelStatusCode::invalid_argument, "Invalid Git deployment request");
    std::shared_ptr<GitAdapter> git;
    std::shared_ptr<DeploymentAdapter> deployment;
    { std::lock_guard<std::mutex> lock(pImpl_->mutex); git = pImpl_->git_adapter; deployment = pImpl_->deployment_adapter; }
    if (!git) return failure<VercelDeployment>(VercelStatusCode::not_configured, "Git adapter not configured");
    auto prepared = git->prepareDeployment(git_url, project_name.empty() ? generateProjectName("git-deployment") : project_name, branch);
    if (!prepared) return failure<VercelDeployment>(prepared.status.code, prepared.status.message, prepared.status.http_status);
    if (prepared.value.name.empty()) prepared.value.name = project_name.empty() ? generateProjectName("git-deployment") : project_name;
    if (prepared.value.git_source.empty()) prepared.value.git_source = git_url + "#" + branch;
    auto result = deployment ? deployment->createDeployment(prepared.value) : api_->createDeploymentResult(prepared.value);
    { std::lock_guard<std::mutex> lock(pImpl_->mutex); pImpl_->last_status = result.status; }
    return result;
}
VercelDeployment VercelIntegration::deployGitRepository(const std::string& url, const std::string& name, const std::string& branch) { auto result = deployGitRepositoryResult(url, name, branch); return result ? result.value : VercelDeployment{}; }

bool VercelIntegration::setupProject(const std::string& name, const std::string& framework) { return isInitialized() && api_->createProjectResult(name, framework).ok(); }
bool VercelIntegration::configureProject(const std::string& project_id, const std::unordered_map<std::string, std::string>& env) {
    if (!isInitialized() || !validIdentifier(project_id)) return false;
    for (const auto& entry : env) if (!api_->setEnvironmentVariable(project_id, entry.first, entry.second)) return false;
    return true;
}
bool VercelIntegration::linkDomain(const std::string& project_id, const std::string& domain) { return isInitialized() && api_->addDomainResult(domain, project_id).ok(); }

bool VercelIntegration::monitorDeployment(const std::string& id, std::function<void(const std::string&)> callback) {
    if (!isInitialized() || !validIdentifier(id)) return false;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(std::max(1, config_.timeout_seconds));
    while (true) {
        std::shared_ptr<DeploymentAdapter> adapter;
        { std::lock_guard<std::mutex> lock(pImpl_->mutex); adapter = pImpl_->deployment_adapter; }
        auto result = adapter ? adapter->getDeployment(id) : api_->getDeploymentResult(id);
        if (!result) return false;
        const std::string message = result.value.isReady() ? "Deployment ready at: " + result.value.url :
                                    terminalFailure(result.value) ? "Deployment failed: " + result.value.state :
                                    "Deployment state: " + result.value.state;
        if (callback) callback(message); // External callback is intentionally outside every lock.
        if (result.value.isReady()) return true;
        if (terminalFailure(result.value)) return false;
        if (std::chrono::steady_clock::now() >= deadline) return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(std::max(1, config_.poll_interval_ms)));
    }
}
std::vector<VercelDeployment> VercelIntegration::getRecentDeployments(const std::string& project_id, int limit) {
    if (!isInitialized() || !validIdentifier(project_id) || limit < 1) return {};
    std::shared_ptr<DeploymentAdapter> adapter;
    { std::lock_guard<std::mutex> lock(pImpl_->mutex); adapter = pImpl_->deployment_adapter; }
    std::vector<VercelDeployment> items;
    if (adapter) { auto result = adapter->listDeployments(project_id, limit); if (!result) return {}; items = std::move(result.value); }
    else { auto result = api_->listDeploymentsPage(project_id, "", std::min(limit, 100)); if (!result) return {}; items = std::move(result.value.items); }
    std::sort(items.begin(), items.end(), [](const VercelDeployment& a, const VercelDeployment& b) { return a.created_at > b.created_at; });
    if (items.size() > static_cast<std::size_t>(limit)) items.resize(static_cast<std::size_t>(limit));
    return items;
}

bool VercelIntegration::enableContinuousDeployment(const std::string& project_id
, const std::string& branch) {
    if (!isInitialized() || !validIdentifier(project_id) || !validIdentifier(branch)) return false;
    std::shared_ptr<DeploymentAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        adapter = pImpl_->deployment_adapter;
    }
    if (!adapter) {
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        pImpl_->last_status = status(VercelStatusCode::not_configured,
                                     "Deployment adapter not configured");
        return false;
    }
    const auto result = adapter->configureContinuousDeployment(project_id, branch, true);
    {
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        pImpl_->last_status = result;
    }
    return result.ok();
}

bool VercelIntegration::disableContinuousDeployment(const std::string& project_id) {
    if (!isInitialized() || !validIdentifier(project_id)) return false;
    std::shared_ptr<DeploymentAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        adapter = pImpl_->deployment_adapter;
    }
    if (!adapter) {
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        pImpl_->last_status = status(VercelStatusCode::not_configured,
                                     "Deployment adapter not configured");
        return false;
    }
    const auto result = adapter->configureContinuousDeployment(project_id, "", false);
    {
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        pImpl_->last_status = result;
    }
    return result.ok();
}

bool VercelIntegration::updateConfig(const VercelConfig& config) {
    bool was_initialized = false;
    std::shared_ptr<HttpAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(pImpl_->mutex);
        was_initialized = pImpl_->initialized;
        pImpl_->initialized = false;
        config_ = config;
        adapter = pImpl_->http_adapter;
        api_ = std::make_shared<VercelAPI>(config, adapter);
        pImpl_->last_status = {};
    }
    return !was_initialized || initialize();
}

void VercelIntegration::setDeploymentAdapter(std::shared_ptr<DeploymentAdapter> adapter) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    pImpl_->deployment_adapter = std::move(adapter);
    pImpl_->initialized = false;
}

void VercelIntegration::setGitAdapter(std::shared_ptr<GitAdapter> adapter) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    pImpl_->git_adapter = std::move(adapter);
}

} // namespace elizaos
