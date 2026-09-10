#include "elizaos/livevideochat_server.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cctype>
#include <cstring>
#include <limits>
#include <map>
#include <mutex>
#include <optional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#ifndef _WIN32
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace elizaos {
namespace {

using Json = nlohmann::json;

constexpr std::size_t kMaxRequestLineBytes = 4096;
constexpr std::size_t kMaxHeaderBytes = 16384;
constexpr std::size_t kMaxHeaderCount = 64;
constexpr std::size_t kMaxBodyBytes = 1024 * 1024;
constexpr std::size_t kMaxOutboundMessageBytes = 1024 * 1024;
constexpr std::chrono::milliseconds kClientIoTimeout{1500};

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string upper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t");
    return value.substr(first, last - first + 1);
}

bool validMethod(const std::string& method) {
    if (method.empty() || method.size() > 16) return false;
    return std::all_of(method.begin(), method.end(), [](unsigned char ch) {
        return std::isupper(ch) != 0;
    });
}

bool validPath(const std::string& path) {
    if (path.empty() || path.front() != '/' || path.size() > kMaxRequestLineBytes) return false;
    return std::none_of(path.begin(), path.end(), [](unsigned char ch) {
        return ch < 0x20 || ch == 0x7f || ch == '\\';
    });
}

bool validIdentifier(const std::string& value) {
    if (value.empty() || value.size() > 128) return false;
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return std::isalnum(ch) != 0 || ch == '-' || ch == '_' || ch == '.';
    });
}

std::string jsonError(const std::string& error) {
    return Json{{"error", error}}.dump();
}

HttpResponse response(int status, const std::string& body) {
    HttpResponse result;
    result.status_code = status;
    result.body = body;
    return result;
}

HttpResponse errorResponse(int status, const std::string& error) {
    return response(status, jsonError(error));
}

bool validStatus(int status) {
    return status >= 100 && status <= 599;
}

bool validHandlerResponse(const HttpResponse& candidate) {
    if (!validStatus(candidate.status_code) || candidate.body.size() > kMaxBodyBytes) return false;
    const auto type = std::find_if(
        candidate.headers.begin(), candidate.headers.end(), [](const auto& item) {
            return lower(item.first) == "content-type";
        });
    if (type != candidate.headers.end() &&
        lower(type->second).find("application/json") != std::string::npos &&
        !candidate.body.empty()) {
        return Json::accept(candidate.body);
    }
    return true;
}

bool contentTypeIsJson(const HttpRequest& request) {
    const auto it = std::find_if(request.headers.begin(), request.headers.end(), [](const auto& item) {
        return lower(item.first) == "content-type";
    });
    if (it == request.headers.end()) return false;
    return lower(it->second).find("application/json") == 0;
}

std::optional<Json> parseJsonObject(const HttpRequest& request) {
    if (request.body.empty() || request.body.size() > kMaxBodyBytes || !contentTypeIsJson(request)) {
        return std::nullopt;
    }
    Json parsed = Json::parse(request.body, nullptr, false);
    if (parsed.is_discarded() || !parsed.is_object()) return std::nullopt;
    return parsed;
}

std::string pathWithoutQuery(const std::string& target) {
    const auto position = target.find('?');
    return position == std::string::npos ? target : target.substr(0, position);
}

int fromHex(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

std::optional<std::string> percentDecode(const std::string& value) {
    std::string result;
    result.reserve(value.size());
    for (std::size_t index = 0; index < value.size(); ++index) {
        if (value[index] == '+') {
            result.push_back(' ');
        } else if (value[index] == '%') {
            if (index + 2 >= value.size()) return std::nullopt;
            const int high = fromHex(value[index + 1]);
            const int low = fromHex(value[index + 2]);
            if (high < 0 || low < 0) return std::nullopt;
            const char decoded = static_cast<char>((high << 4) | low);
            if (decoded == '\0' || decoded == '\r' || decoded == '\n') return std::nullopt;
            result.push_back(decoded);
            index += 2;
        } else {
            result.push_back(value[index]);
        }
    }
    return result;
}

bool parseQuery(const std::string& target, std::map<std::string, std::string>* output) {
    output->clear();
    const auto question = target.find('?');
    if (question == std::string::npos) return true;
    std::size_t start = question + 1;
    while (start <= target.size()) {
        const auto ampersand = target.find('&', start);
        const std::string item = target.substr(
            start, ampersand == std::string::npos ? std::string::npos : ampersand - start);
        if (!item.empty()) {
            const auto equals = item.find('=');
            auto key = percentDecode(item.substr(0, equals));
            auto value = percentDecode(
                equals == std::string::npos ? std::string{} : item.substr(equals + 1));
            if (!key || !value || key->empty() || key->size() > 256 || value->size() > 4096) {
                return false;
            }
            (*output)[*key] = *value;
        }
        if (ampersand == std::string::npos) break;
        start = ampersand + 1;
    }
    return true;
}

std::string reasonPhrase(int status) {
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 411: return "Length Required";
        case 413: return "Payload Too Large";
        case 415: return "Unsupported Media Type";
        case 422: return "Unprocessable Content";
        case 431: return "Request Header Fields Too Large";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 503: return "Service Unavailable";
        case 505: return "HTTP Version Not Supported";
        default: return "Error";
    }
}

#ifndef _WIN32
void closeSocket(int fd) {
    if (fd >= 0) ::close(fd);
}

bool sendAll(int fd, const std::string& value) {
    std::size_t sent = 0;
    while (sent < value.size()) {
        const ssize_t written = ::send(fd, value.data() + sent, value.size() - sent, MSG_NOSIGNAL);
        if (written > 0) {
            sent += static_cast<std::size_t>(written);
            continue;
        }
        if (written < 0 && errno == EINTR) continue;
        return false;
    }
    return true;
}

void setClientTimeouts(int fd) {
    const timeval timeout{
        static_cast<time_t>(kClientIoTimeout.count() / 1000),
        static_cast<suseconds_t>((kClientIoTimeout.count() % 1000) * 1000)};
    ::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    ::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}
#endif

struct Route {
    std::string method;
    std::string path_pattern;
    std::regex compiled_path;
    HttpHandler handler;
};

struct ClientEntry {
    std::shared_ptr<WebSocketClientAdapter> adapter;
    bool acknowledged = false;
};

} // namespace

class LiveVideoChatServer::Impl {
public:
    Impl() { setupDefaultRoutes(); }
    ~Impl() { stop(); }

    bool initialize(int port) {
        if (port < 0 || port > 65535) return false;
        std::lock_guard<std::mutex> lock(lifecycle_mutex_);
        if (running_.load()) return false;
        requested_port_ = port;
        bound_port_ = port;
        initialized_ = true;
        return true;
    }

    void setupDefaultRoutes() {
        registerRoute("OPTIONS", ".*", [](const HttpRequest&) {
            HttpResponse result;
            result.status_code = 204;
            result.body.clear();
            return result;
        });
        registerRoute("GET", "/health", [](const HttpRequest&) {
            return response(200, Json{{"status", "healthy"}, {"service", "LiveVideoChatServer"}}.dump());
        });
        registerRoute("GET", "/api/info", [this](const HttpRequest&) {
            Json endpoints = Json::array({
                "/health", "/api/info", "/:agent_id/message", "/:agent_id/whisper",
                "/webrtc/signaling", "/sessions"});
            return response(200, Json{{"service", "LiveVideoChatServer"},
                                      {"version", "2.0.0"},
                                      {"port", getPort()},
                                      {"endpoints", std::move(endpoints)}}.dump());
        });
    }

    void registerRoute(const std::string& method, const std::string& path, HttpHandler handler) {
        const std::string normalized_method = upper(method);
        if (!validMethod(normalized_method) || path.empty() || path.size() > kMaxRequestLineBytes ||
            !handler) {
            return;
        }
        std::regex compiled;
        try {
            compiled = std::regex(path, std::regex::ECMAScript | std::regex::optimize);
        } catch (const std::regex_error&) {
            return;
        }
        std::lock_guard<std::mutex> lock(routes_mutex_);
        const auto existing = std::find_if(routes_.begin(), routes_.end(), [&](const Route& route) {
            return route.method == normalized_method && route.path_pattern == path;
        });
        Route route{normalized_method, path, std::move(compiled), std::move(handler)};
        if (existing == routes_.end()) {
            routes_.push_back(std::move(route));
        } else {
            *existing = std::move(route);
        }
    }

    HttpResponse handleRequest(const HttpRequest& request) const {
        if (!validMethod(request.method) || !validPath(request.path) ||
            request.body.size() > kMaxBodyBytes || request.headers.size() > kMaxHeaderCount) {
            return errorResponse(request.body.size() > kMaxBodyBytes ? 413 : 400, "Invalid request");
        }
        std::size_t header_bytes = 0;
        for (const auto& header : request.headers) {
            header_bytes += header.first.size() + header.second.size() + 4;
            if (header.first.empty() || header.first.find_first_of("\r\n:") != std::string::npos ||
                header.second.find_first_of("\r\n") != std::string::npos ||
                header_bytes > kMaxHeaderBytes) {
                return errorResponse(400, "Invalid request headers");
            }
        }

        HttpHandler handler;
        bool path_exists = false;
        {
            std::lock_guard<std::mutex> lock(routes_mutex_);
            for (const auto& route : routes_) {
                bool matches = false;
                try {
                    matches = std::regex_match(request.path, route.compiled_path);
                } catch (const std::regex_error&) {
                    continue;
                }
                if (!matches) continue;
                path_exists = true;
                if (route.method == request.method) {
                    handler = route.handler;
                    break;
                }
            }
        }
        if (!handler) {
            return errorResponse(path_exists ? 405 : 404,
                                 path_exists ? "Method not allowed" : "Route not found");
        }
        try {
            HttpResponse result = handler(request);
            if (!validHandlerResponse(result)) {
                return errorResponse(500, "Invalid handler response");
            }
            return result;
        } catch (...) {
            return errorResponse(500, "Internal server error");
        }
    }

    bool start() {
#ifndef _WIN32
        std::lock_guard<std::mutex> lock(lifecycle_mutex_);
        if (!initialized_ || running_.load() || server_thread_.joinable()) return false;

        const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) return false;
        const int enabled = 1;
        ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled));

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.sin_port = htons(static_cast<std::uint16_t>(requested_port_));
        if (::bind(fd, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0 ||
            ::listen(fd, 32) != 0) {
            closeSocket(fd);
            return false;
        }
        sockaddr_in bound{};
        socklen_t bound_size = sizeof(bound);
        if (::getsockname(fd, reinterpret_cast<sockaddr*>(&bound), &bound_size) != 0) {
            closeSocket(fd);
            return false;
        }
        listen_fd_ = fd;
        bound_port_ = ntohs(bound.sin_port);
        running_.store(true);
        server_thread_ = std::thread([this] { acceptLoop(); });
        return true;
#else
        return false;
#endif
    }

    void stop() {
#ifndef _WIN32
        std::thread thread;
        bool called_from_server_thread = false;
        {
            std::lock_guard<std::mutex> lock(lifecycle_mutex_);
            running_.store(false);
            if (listen_fd_ >= 0) {
                ::shutdown(listen_fd_, SHUT_RDWR);
                closeSocket(listen_fd_);
                listen_fd_ = -1;
            }
            called_from_server_thread =
                server_thread_.joinable() &&
                server_thread_.get_id() == std::this_thread::get_id();
            if (!called_from_server_thread && active_client_fd_ >= 0) {
                ::shutdown(active_client_fd_, SHUT_RDWR);
            }
            if (!called_from_server_thread) thread = std::move(server_thread_);
        }
        if (called_from_server_thread) return;
        if (thread.joinable()) thread.join();
        std::unique_lock<std::mutex> lock(active_mutex_);
        active_condition_.wait(lock, [this] { return active_connections_ == 0; });
#else
        running_.store(false);
#endif
    }

    void setVideoChatInstance(std::shared_ptr<LiveVideoChat> video_chat) {
        std::lock_guard<std::mutex> lock(video_chat_mutex_);
        video_chat_ = std::move(video_chat);
    }

    void setWebSocketHandler(WebSocketHandler handler) {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        websocket_handler_ = std::move(handler);
    }

    bool registerWebSocketClient(
        const std::string& client_id,
        std::shared_ptr<WebSocketClientAdapter> adapter) {
        if (!validIdentifier(client_id) || !adapter) return false;
        std::lock_guard<std::mutex> lock(clients_mutex_);
        return clients_.emplace(client_id, ClientEntry{std::move(adapter), false}).second;
    }

    bool acknowledgeWebSocketClient(const std::string& client_id) {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        const auto it = clients_.find(client_id);
        if (it == clients_.end() || !it->second.adapter) return false;
        it->second.acknowledged = true;
        return true;
    }

    void unregisterWebSocketClient(const std::string& client_id) {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_.erase(client_id);
    }

    bool handleWebSocketMessage(const std::string& client_id, const std::string& message) {
        if (message.empty() || message.size() > kMaxOutboundMessageBytes) return false;
        WebSocketHandler handler;
        {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            const auto it = clients_.find(client_id);
            if (it == clients_.end() || !it->second.acknowledged || !websocket_handler_) return false;
            handler = websocket_handler_;
        }
        try {
            handler(client_id, message);
            return true;
        } catch (...) {
            return false;
        }
    }

    bool sendWebSocketMessage(const std::string& client_id, const std::string& message) {
        if (message.empty() || message.size() > kMaxOutboundMessageBytes) return false;
        std::shared_ptr<WebSocketClientAdapter> adapter;
        {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            const auto it = clients_.find(client_id);
            if (it == clients_.end() || !it->second.acknowledged || !it->second.adapter) return false;
            adapter = it->second.adapter;
        }
        try {
            return adapter->send(message);
        } catch (...) {
            return false;
        }
    }

    void broadcastMessage(const std::string& message) {
        if (message.empty() || message.size() > kMaxOutboundMessageBytes) return;
        std::vector<std::shared_ptr<WebSocketClientAdapter>> adapters;
        {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            for (const auto& client : clients_) {
                if (client.second.acknowledged && client.second.adapter) {
                    adapters.push_back(client.second.adapter);
                }
            }
        }
        for (const auto& adapter : adapters) {
            try { static_cast<void>(adapter->send(message)); } catch (...) {}
        }
    }

    bool isRunning() const { return running_.load(); }

    int getPort() const {
        std::lock_guard<std::mutex> lock(lifecycle_mutex_);
        return bound_port_;
    }

private:
#ifndef _WIN32
    struct ActiveConnection {
        explicit ActiveConnection(Impl* owner) : owner_(owner) {
            std::lock_guard<std::mutex> lock(owner_->active_mutex_);
            ++owner_->active_connections_;
        }
        ~ActiveConnection() {
            std::lock_guard<std::mutex> lock(owner_->active_mutex_);
            --owner_->active_connections_;
            owner_->active_condition_.notify_all();
        }
        Impl* owner_;
    };

    void acceptLoop() {
        while (running_.load()) {
            int fd = -1;
            {
                std::lock_guard<std::mutex> lock(lifecycle_mutex_);
                fd = listen_fd_;
            }
            if (fd < 0) break;
            fd_set read_set;
            FD_ZERO(&read_set);
            FD_SET(fd, &read_set);
            timeval timeout{0, 100000};
            const int ready = ::select(fd + 1, &read_set, nullptr, nullptr, &timeout);
            if (ready < 0) {
                if (errno == EINTR) continue;
                if (!running_.load()) break;
                continue;
            }
            if (ready == 0) continue;
            const int client = ::accept(fd, nullptr, nullptr);
            if (client < 0) {
                if (errno == EINTR) continue;
                if (!running_.load()) break;
                continue;
            }
            {
                std::lock_guard<std::mutex> lock(lifecycle_mutex_);
                if (!running_.load()) {
                    closeSocket(client);
                    break;
                }
                active_client_fd_ = client;
            }
            ActiveConnection active(this);
            setClientTimeouts(client);
            serviceClient(client);
            {
                std::lock_guard<std::mutex> lock(lifecycle_mutex_);
                if (active_client_fd_ == client) active_client_fd_ = -1;
            }
            closeSocket(client);
        }
    }

    static void sendResponse(int client, HttpResponse result) {
        if (!validHandlerResponse(result)) result = errorResponse(500, "Invalid handler response");
        result.headers.erase("Content-Length");
        result.headers.erase("Connection");
        std::ostringstream wire;
        wire << "HTTP/1.1 " << result.status_code << ' ' << reasonPhrase(result.status_code) << "\r\n";
        for (const auto& header : result.headers) {
            if (header.first.find_first_of("\r\n:") == std::string::npos &&
                header.second.find_first_of("\r\n") == std::string::npos) {
                wire << header.first << ": " << header.second << "\r\n";
            }
        }
        wire << "Content-Length: " << result.body.size() << "\r\n"
             << "Connection: close\r\n\r\n" << result.body;
        static_cast<void>(sendAll(client, wire.str()));
    }

    static std::optional<std::size_t> parseContentLength(
        const std::map<std::string, std::string>& headers,
        bool* invalid) {
        *invalid = false;
        const auto it = headers.find("content-length");
        if (it == headers.end()) return std::nullopt;
        if (it->second.empty() || !std::all_of(it->second.begin(), it->second.end(), [](unsigned char ch) {
                return std::isdigit(ch) != 0;
            })) {
            *invalid = true;
            return std::nullopt;
        }
        try {
            const unsigned long long value = std::stoull(it->second);
            if (value > std::numeric_limits<std::size_t>::max()) {
                *invalid = true;
                return std::nullopt;
            }
            return static_cast<std::size_t>(value);
        } catch (...) {
            *invalid = true;
            return std::nullopt;
        }
    }

    void serviceClient(int client) {
        std::string bytes;
        bytes.reserve(2048);
        std::array<char, 2048> buffer{};
        std::size_t header_end = std::string::npos;
        while ((header_end = bytes.find("\r\n\r\n")) == std::string::npos) {
            if (bytes.size() > kMaxHeaderBytes) {
                sendResponse(client, errorResponse(431, "Headers too large"));
                return;
            }
            const ssize_t received = ::recv(client, buffer.data(), buffer.size(), 0);
            if (received > 0) {
                bytes.append(buffer.data(), static_cast<std::size_t>(received));
                continue;
            }
            sendResponse(client, errorResponse(received == 0 ? 400 : 408, "Incomplete request"));
            return;
        }
        if (header_end > kMaxHeaderBytes) {
            sendResponse(client, errorResponse(431, "Headers too large"));
            return;
        }

        const std::string head = bytes.substr(0, header_end);
        const auto first_line_end = head.find("\r\n");
        const std::string request_line = head.substr(0, first_line_end);
        if (request_line.size() > kMaxRequestLineBytes) {
            sendResponse(client, errorResponse(400, "Request line too large"));
            return;
        }
        std::istringstream request_stream(request_line);
        std::string method;
        std::string target;
        std::string version;
        std::string trailing;
        if (!(request_stream >> method >> target >> version) || (request_stream >> trailing) ||
            version != "HTTP/1.1" || !validMethod(method) || !validPath(pathWithoutQuery(target))) {
            sendResponse(client, errorResponse(version.empty() || version == "HTTP/1.1" ? 400 : 505,
                                               "Invalid request line"));
            return;
        }

        HttpRequest request;
        request.method = method;
        request.path = pathWithoutQuery(target);
        if (!parseQuery(target, &request.query_params)) {
            sendResponse(client, errorResponse(400, "Invalid query string"));
            return;
        }

        std::size_t line_start = first_line_end == std::string::npos ? head.size() : first_line_end + 2;
        while (line_start < head.size()) {
            const auto line_end = head.find("\r\n", line_start);
            const std::string line = head.substr(
                line_start, line_end == std::string::npos ? std::string::npos : line_end - line_start);
            const auto colon = line.find(':');
            if (colon == std::string::npos || colon == 0 || request.headers.size() >= kMaxHeaderCount) {
                sendResponse(client, errorResponse(400, "Invalid request headers"));
                return;
            }
            const std::string name = lower(trim(line.substr(0, colon)));
            const std::string value = trim(line.substr(colon + 1));
            if (name.empty() || name.find_first_of(" \t\r\n:") != std::string::npos ||
                value.find_first_of("\r\n") != std::string::npos || request.headers.count(name) != 0) {
                sendResponse(client, errorResponse(400, "Invalid request headers"));
                return;
            }
            request.headers.emplace(name, value);
            if (line_end == std::string::npos) break;
            line_start = line_end + 2;
        }
        if (request.headers.count("transfer-encoding") != 0) {
            sendResponse(client, errorResponse(400, "Transfer-Encoding is unsupported"));
            return;
        }
        if (request.headers.count("host") == 0) {
            sendResponse(client, errorResponse(400, "Host header required"));
            return;
        }

        bool invalid_length = false;
        const auto content_length = parseContentLength(request.headers, &invalid_length);
        if (invalid_length) {
            sendResponse(client, errorResponse(400, "Invalid Content-Length"));
            return;
        }
        if (content_length && *content_length > kMaxBodyBytes) {
            sendResponse(client, errorResponse(413, "Payload too large"));
            return;
        }
        const std::size_t expected = content_length.value_or(0);
        request.body = bytes.substr(header_end + 4);
        if (request.body.size() > expected) request.body.resize(expected);
        while (request.body.size() < expected) {
            const std::size_t remaining = expected - request.body.size();
            const ssize_t received = ::recv(client, buffer.data(), std::min(buffer.size(), remaining), 0);
            if (received <= 0) {
                sendResponse(client, errorResponse(408, "Incomplete request body"));
                return;
            }
            request.body.append(buffer.data(), static_cast<std::size_t>(received));
        }
        sendResponse(client, handleRequest(request));
    }
#endif

    mutable std::mutex lifecycle_mutex_;
    mutable std::mutex routes_mutex_;
    mutable std::mutex video_chat_mutex_;
    mutable std::mutex clients_mutex_;
    std::vector<Route> routes_;
    std::weak_ptr<LiveVideoChat> video_chat_;
    WebSocketHandler websocket_handler_;
    std::map<std::string, ClientEntry> clients_;
    std::atomic<bool> running_{false};
    bool initialized_ = false;
    int requested_port_ = 3000;
    int bound_port_ = 3000;
#ifndef _WIN32
    int listen_fd_ = -1;
    int active_client_fd_ = -1;
    std::thread server_thread_;
    std::mutex active_mutex_;
    std::condition_variable active_condition_;
    std::size_t active_connections_ = 0;
#endif
};

LiveVideoChatServer::LiveVideoChatServer() : impl_(std::make_unique<Impl>()) {}
LiveVideoChatServer::~LiveVideoChatServer() = default;
bool LiveVideoChatServer::initialize(int port) { return impl_->initialize(port); }
bool LiveVideoChatServer::start() { return impl_->start(); }
void LiveVideoChatServer::stop() { impl_->stop(); }
void LiveVideoChatServer::setVideoChatInstance(std::shared_ptr<LiveVideoChat> video_chat) {
    impl_->setVideoChatInstance(std::move(video_chat));
}
void LiveVideoChatServer::registerRoute(
    const std::string& method, const std::string& path, HttpHandler handler) {
    impl_->registerRoute(method, path, std::move(handler));
}
HttpResponse LiveVideoChatServer::handleRequest(const HttpRequest& request) const {
    return impl_->handleRequest(request);
}
void LiveVideoChatServer::setWebSocketHandler(WebSocketHandler handler) {
    impl_->setWebSocketHandler(std::move(handler));
}
bool LiveVideoChatServer::registerWebSocketClient(
    const std::string& client_id, std::shared_ptr<WebSocketClientAdapter> adapter) {
    return impl_->registerWebSocketClient(client_id, std::move(adapter));
}
bool LiveVideoChatServer::acknowledgeWebSocketClient(const std::string& client_id) {
    return impl_->acknowledgeWebSocketClient(client_id);
}
void LiveVideoChatServer::unregisterWebSocketClient(const std::string& client_id) {
    impl_->unregisterWebSocketClient(client_id);
}
bool LiveVideoChatServer::handleWebSocketMessage(
    const std::string& client_id, const std::string& message) {
    return impl_->handleWebSocketMessage(client_id, message);
}
bool LiveVideoChatServer::sendWebSocketMessage(
    const std::string& client_id, const std::string& message) {
    return impl_->sendWebSocketMessage(client_id, message);
}
void LiveVideoChatServer::broadcastMessage(const std::string& message) {
    impl_->broadcastMessage(message);
}
bool LiveVideoChatServer::isRunning() const { return impl_->isRunning(); }
int LiveVideoChatServer::getPort() const { return impl_->getPort(); }

class LiveVideoChatWithServer::Impl {
public:
    Impl() : server_(std::make_shared<LiveVideoChatServer>()), callbacks_(std::make_shared<Callbacks>()) {}

    ~Impl() {
        callbacks_->enabled.store(false);
        server_->stop();
    }

    bool initialize(int server_port) {
        if (!server_->initialize(server_port)) return false;
        setupRoutes();
        return true;
    }

    void setupRoutes() {
        const std::weak_ptr<Callbacks> weak_callbacks = callbacks_;
        server_->registerRoute("POST", "/([^/]+)/message", [weak_callbacks](const HttpRequest& request) {
            return invoke(weak_callbacks, Kind::Agent, request);
        });
        server_->registerRoute("POST", "/([^/]+)/whisper", [weak_callbacks](const HttpRequest& request) {
            return invoke(weak_callbacks, Kind::Transcription, request);
        });
        server_->registerRoute("POST", "/webrtc/signaling", [weak_callbacks](const HttpRequest& request) {
            return invoke(weak_callbacks, Kind::Signaling, request);
        });
        server_->registerRoute("GET", "/sessions", [weak_callbacks](const HttpRequest& request) {
            return sessionRequest(weak_callbacks, request);
        });
        server_->registerRoute("POST", "/sessions", [weak_callbacks](const HttpRequest& request) {
            return sessionRequest(weak_callbacks, request);
        });
        server_->registerRoute("DELETE", "/sessions/([^/]+)", [weak_callbacks](const HttpRequest& request) {
            return sessionRequest(weak_callbacks, request);
        });
    }

    void setAgentMessageHandler(AgentMessageHandler handler) {
        std::lock_guard<std::mutex> lock(callbacks_->mutex);
        callbacks_->agent = std::move(handler);
    }

    void setTranscriptionHandler(TranscriptionHandler handler) {
        std::lock_guard<std::mutex> lock(callbacks_->mutex);
        callbacks_->transcription = std::move(handler);
    }

    void setSignalingHandler(SignalingHandler handler) {
        std::lock_guard<std::mutex> lock(callbacks_->mutex);
        callbacks_->signaling = std::move(handler);
    }

    HttpResponse handleAgentMessage(const HttpRequest& request) const {
        return invoke(callbacks_, Kind::Agent, request);
    }

    HttpResponse handleWhisperTranscription(const HttpRequest& request) const {
        return invoke(callbacks_, Kind::Transcription, request);
    }

    HttpResponse handleWebRTCSignaling(const HttpRequest& request) const {
        return invoke(callbacks_, Kind::Signaling, request);
    }

    HttpResponse handleSessionManagement(const HttpRequest& request) const {
        return sessionRequest(callbacks_, request);
    }

    std::shared_ptr<LiveVideoChatServer> server_;

private:
    enum class Kind { Agent, Transcription, Signaling };

    struct SessionRecord {
        std::string id;
        std::string status = "active";
        std::vector<std::string> participants;
    };

    struct Callbacks {
        std::mutex mutex;
        AgentMessageHandler agent;
        TranscriptionHandler transcription;
        SignalingHandler signaling;
        std::map<std::string, SessionRecord> sessions;
        std::atomic<bool> enabled{true};
    };

    static HttpResponse invoke(
        const std::weak_ptr<Callbacks>& weak_callbacks,
        Kind kind,
        const HttpRequest& request) {
        const auto callbacks = weak_callbacks.lock();
        if (!callbacks || !callbacks->enabled.load()) return errorResponse(503, "Service unavailable");
        if (request.method != "POST") return errorResponse(405, "Method not allowed");
        const std::regex agent_path{"/[^/]+/message"};
        const std::regex transcription_path{"/[^/]+/whisper"};
        const bool path_matches =
            kind == Kind::Agent
                ? std::regex_match(request.path, agent_path)
                : kind == Kind::Transcription
                      ? std::regex_match(request.path, transcription_path)
                      : request.path == "/webrtc/signaling";
        if (!path_matches) return errorResponse(404, "Route not found");
        if (request.body.size() > kMaxBodyBytes) return errorResponse(413, "Payload too large");
        if (!contentTypeIsJson(request)) return errorResponse(415, "Content-Type must be application/json");
        const auto parsed = parseJsonObject(request);
        if (!parsed) return errorResponse(400, "Body must be a JSON object");

        if (kind == Kind::Agent) {
            if (!parsed->contains("message") || !(*parsed)["message"].is_string() ||
                (*parsed)["message"].get_ref<const std::string&>().empty()) {
                return errorResponse(422, "A non-empty message is required");
            }
        } else if (kind == Kind::Transcription) {
            if (!parsed->contains("audio") || !(*parsed)["audio"].is_string() ||
                (*parsed)["audio"].get_ref<const std::string&>().empty()) {
                return errorResponse(422, "A non-empty audio value is required");
            }
        } else if (!parsed->contains("type") || !(*parsed)["type"].is_string() ||
                   !parsed->contains("sdp") || !(*parsed)["sdp"].is_string() ||
                   (*parsed)["sdp"].get_ref<const std::string&>().empty()) {
            return errorResponse(422, "Signaling type and non-empty SDP are required");
        }

        HttpHandler handler;
        {
            std::lock_guard<std::mutex> lock(callbacks->mutex);
            if (kind == Kind::Agent) handler = callbacks->agent;
            else if (kind == Kind::Transcription) handler = callbacks->transcription;
            else handler = callbacks->signaling;
        }
        if (!handler) return errorResponse(501, "Handler not configured");
        try {
            HttpResponse result = handler(request);
            if (!validHandlerResponse(result)) return errorResponse(503, "Handler failed");
            return result;
        } catch (...) {
            return errorResponse(503, "Handler failed");
        }
    }

    static std::string sessionIdFromPath(const std::string& path) {
        constexpr const char* prefix = "/sessions/";
        if (path.find(prefix) != 0) return {};
        return path.substr(std::strlen(prefix));
    }

    static HttpResponse sessionRequest(
        const std::weak_ptr<Callbacks>& weak_callbacks,
        const HttpRequest& request) {
        const auto callbacks = weak_callbacks.lock();
        if (!callbacks || !callbacks->enabled.load()) return errorResponse(503, "Service unavailable");

        if (request.method == "GET" && request.path == "/sessions") {
            Json sessions = Json::array();
            {
                std::lock_guard<std::mutex> lock(callbacks->mutex);
                for (const auto& item : callbacks->sessions) {
                    sessions.push_back(Json{{"id", item.second.id},
                                            {"status", item.second.status},
                                            {"participants", item.second.participants}});
                }
            }
            return response(200, Json{{"sessions", sessions}, {"total", sessions.size()}}.dump());
        }

        if (request.method == "POST" && request.path == "/sessions") {
            if (!contentTypeIsJson(request)) {
                return errorResponse(415, "Content-Type must be application/json");
            }
            const auto parsed = parseJsonObject(request);
            if (!parsed || !parsed->contains("session_id") || !(*parsed)["session_id"].is_string()) {
                return errorResponse(400, "session_id is required");
            }
            const std::string id = (*parsed)["session_id"].get<std::string>();
            if (!validIdentifier(id)) return errorResponse(422, "Invalid session_id");

            SessionRecord record;
            record.id = id;
            if (parsed->contains("participants")) {
                if (!(*parsed)["participants"].is_array() || (*parsed)["participants"].size() > 64) {
                    return errorResponse(422, "participants must be a bounded array");
                }
                for (const auto& participant : (*parsed)["participants"]) {
                    if (!participant.is_string() || !validIdentifier(participant.get<std::string>())) {
                        return errorResponse(422, "Invalid participant");
                    }
                    record.participants.push_back(participant.get<std::string>());
                }
            }
            {
                std::lock_guard<std::mutex> lock(callbacks->mutex);
                if (!callbacks->sessions.emplace(id, record).second) {
                    return errorResponse(409, "Session already exists");
                }
            }
            return response(201, Json{{"session_id", id}, {"status", "created"}}.dump());
        }

        if (request.method == "DELETE") {
            const std::string id = sessionIdFromPath(request.path);
            if (!validIdentifier(id)) return errorResponse(400, "Invalid session path");
            bool erased = false;
            {
                std::lock_guard<std::mutex> lock(callbacks->mutex);
                erased = callbacks->sessions.erase(id) != 0;
            }
            if (!erased) return errorResponse(404, "Session not found");
            return response(200, Json{{"session_id", id}, {"status", "deleted"}}.dump());
        }

        return errorResponse(405, "Method not allowed");
    }

    std::shared_ptr<Callbacks> callbacks_;
};

LiveVideoChatWithServer::LiveVideoChatWithServer() : impl_(std::make_unique<Impl>()) {}
LiveVideoChatWithServer::~LiveVideoChatWithServer() {
    stopServer();
    shutdown();
}

bool LiveVideoChatWithServer::initialize(const VideoChatConfig& config, int server_port) {
    bool video_initialized = LiveVideoChat::initialize(config);
    if (!video_initialized && (!config.enable_video && !config.enable_audio)) {
        LiveVideoChatDependencies dependencies;
        dependencies.peer_transport_factory = [](const std::string&, const std::string&) {
            return std::shared_ptr<PeerTransport>{};
        };
        video_initialized = LiveVideoChat::initialize(config, dependencies);
    }
    if (!video_initialized) return false;
    if (!impl_->initialize(server_port)) {
        shutdown();
        return false;
    }
    return true;
}

bool LiveVideoChatWithServer::startServer() { return impl_->server_->start(); }
void LiveVideoChatWithServer::stopServer() { impl_->server_->stop(); }
std::shared_ptr<LiveVideoChatServer> LiveVideoChatWithServer::getServer() { return impl_->server_; }
void LiveVideoChatWithServer::setAgentMessageHandler(AgentMessageHandler handler) {
    impl_->setAgentMessageHandler(std::move(handler));
}
void LiveVideoChatWithServer::setTranscriptionHandler(TranscriptionHandler handler) {
    impl_->setTranscriptionHandler(std::move(handler));
}
void LiveVideoChatWithServer::setSignalingHandler(SignalingHandler handler) {
    impl_->setSignalingHandler(std::move(handler));
}
HttpResponse LiveVideoChatWithServer::handleAgentMessage(const HttpRequest& request) {
    return impl_->handleAgentMessage(request);
}
HttpResponse LiveVideoChatWithServer::handleWhisperTranscription(const HttpRequest& request) {
    return impl_->handleWhisperTranscription(request);
}
HttpResponse LiveVideoChatWithServer::handleWebRTCSignaling(const HttpRequest& request) {
    return impl_->handleWebRTCSignaling(request);
}
HttpResponse LiveVideoChatWithServer::handleSessionManagement(const HttpRequest& request) {
    return impl_->handleSessionManagement(request);
}

} // namespace elizaos
