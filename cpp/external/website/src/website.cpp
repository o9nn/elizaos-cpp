#include "elizaos/website.hpp"
#include "elizaos/agentlogger.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cerrno>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <fstream>
#include <limits>
#include <mutex>
#include <regex>
#include <sstream>
#include <system_error>
#include <thread>
#include <unordered_set>
#include <utility>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace elizaos {
namespace {

AgentLogger g_website_logger;

#ifdef _WIN32
using SocketHandle = SOCKET;
constexpr SocketHandle kInvalidSocket = INVALID_SOCKET;
#else
using SocketHandle = int;
constexpr SocketHandle kInvalidSocket = -1;
#endif

constexpr std::size_t kMaximumRequestHeaderBytes = 16U * 1024U;
constexpr std::size_t kMaximumRequestTargetBytes = 4U * 1024U;
constexpr std::uintmax_t kMaximumServedFileBytes = 64U * 1024U * 1024U;
constexpr std::size_t kWorkerCount = 4U;
constexpr std::size_t kMaximumQueuedClients = 64U;

thread_local bool g_in_website_server_thread = false;

class ServerThreadScope {
public:
    ServerThreadScope() noexcept { g_in_website_server_thread = true; }
    ~ServerThreadScope() { g_in_website_server_thread = false; }

    ServerThreadScope(const ServerThreadScope&) = delete;
    ServerThreadScope& operator=(const ServerThreadScope&) = delete;
};

class ThreadReaper {
public:
    static ThreadReaper& instance() {
        static ThreadReaper reaper;
        return reaper;
    }

    void reap(std::thread thread, std::function<void()> completion) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pending_.push_back(PendingJoin{std::move(thread), std::move(completion)});
        }
        changed_.notify_one();
    }

    ThreadReaper(const ThreadReaper&) = delete;
    ThreadReaper& operator=(const ThreadReaper&) = delete;

private:
    struct PendingJoin {
        std::thread thread;
        std::function<void()> completion;
    };

    ThreadReaper() : worker_([this]() { run(); }) {}

    ~ThreadReaper() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
        }
        changed_.notify_one();
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    void run() noexcept {
        for (;;) {
            PendingJoin pending;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                changed_.wait(lock, [this]() { return stopping_ || !pending_.empty(); });
                if (pending_.empty()) {
                    if (stopping_) {
                        return;
                    }
                    continue;
                }
                pending = std::move(pending_.front());
                pending_.pop_front();
            }

            if (pending.thread.joinable()) {
                pending.thread.join();
            }
            if (pending.completion) {
                try {
                    pending.completion();
                } catch (...) {
                    // Lifecycle cleanup callbacks are noexcept in practice;
                    // never allow one to kill the process-wide join reaper.
                }
            }
        }
    }

    std::mutex mutex_;
    std::condition_variable changed_;
    std::deque<PendingJoin> pending_;
    bool stopping_ = false;
    std::thread worker_;
};

void closeSocket(SocketHandle socket_handle) noexcept {
    if (socket_handle == kInvalidSocket) {
        return;
    }
#ifdef _WIN32
    ::closesocket(socket_handle);
#else
    ::close(socket_handle);
#endif
}

void shutdownSocket(SocketHandle socket_handle) noexcept {
    if (socket_handle == kInvalidSocket) {
        return;
    }
#ifdef _WIN32
    ::shutdown(socket_handle, SD_BOTH);
#else
    ::shutdown(socket_handle, SHUT_RDWR);
#endif
}

int lastSocketError() noexcept {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

bool isInterruptedSocketError(int error_code) noexcept {
#ifdef _WIN32
    return error_code == WSAEINTR;
#else
    return error_code == EINTR;
#endif
}

std::string socketErrorMessage(const std::string& operation, int error_code) {
    return operation + " failed (socket error " + std::to_string(error_code) + ")";
}

bool configureClientTimeouts(SocketHandle socket_handle) noexcept {
#ifdef _WIN32
    const DWORD timeout_ms = 1000U;
    const auto* timeout = reinterpret_cast<const char*>(&timeout_ms);
    return ::setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, timeout,
                        static_cast<int>(sizeof(timeout_ms))) == 0 &&
           ::setsockopt(socket_handle, SOL_SOCKET, SO_SNDTIMEO, timeout,
                        static_cast<int>(sizeof(timeout_ms))) == 0;
#else
    const timeval timeout{1, 0};
    return ::setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                        static_cast<socklen_t>(sizeof(timeout))) == 0 &&
           ::setsockopt(socket_handle, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                        static_cast<socklen_t>(sizeof(timeout))) == 0;
#endif
}

bool sendAll(SocketHandle socket_handle, const char* data, std::size_t size) noexcept {
    std::size_t sent = 0;
    while (sent < size) {
        const std::size_t remaining = size - sent;
        const int chunk_size = static_cast<int>(std::min<std::size_t>(
            remaining, static_cast<std::size_t>(std::numeric_limits<int>::max())));
#ifdef _WIN32
        const int result = ::send(socket_handle, data + sent, chunk_size, 0);
#else
        const int result = static_cast<int>(
            ::send(socket_handle, data + sent, static_cast<std::size_t>(chunk_size), MSG_NOSIGNAL));
#endif
        if (result <= 0) {
            return false;
        }
        sent += static_cast<std::size_t>(result);
    }
    return true;
}

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1U);
}

bool hasPathPrefix(const std::filesystem::path& path,
                   const std::filesystem::path& prefix) noexcept {
    auto path_it = path.begin();
    auto prefix_it = prefix.begin();
    for (; prefix_it != prefix.end(); ++prefix_it, ++path_it) {
        if (path_it == path.end() || *path_it != *prefix_it) {
            return false;
        }
    }
    return true;
}

bool isSafeDirectoryPath(const std::filesystem::path& path) {
    if (path.empty()) {
        return false;
    }
    std::error_code error;
    const auto absolute = std::filesystem::absolute(path, error).lexically_normal();
    if (error || absolute.empty()) {
        return false;
    }
    return absolute != absolute.root_path();
}

bool ensureDirectory(const std::filesystem::path& path) {
    if (!isSafeDirectoryPath(path)) {
        return false;
    }
    std::error_code error;
    if (std::filesystem::exists(path, error)) {
        return !error && std::filesystem::is_directory(path, error) && !error;
    }
    if (error) {
        return false;
    }
    return std::filesystem::create_directories(path, error) && !error;
}

bool isHeaderNameCharacter(unsigned char character) noexcept {
    if (std::isalnum(character) != 0) {
        return true;
    }
    switch (character) {
        case '!': case '#': case '$': case '%': case '&': case '\'': case '*':
        case '+': case '-': case '.': case '^': case '_': case '`': case '|': case '~':
            return true;
        default:
            return false;
    }
}

bool percentDecodePath(const std::string& encoded, std::string& decoded) {
    decoded.clear();
    decoded.reserve(encoded.size());
    const auto hexValue = [](char character) -> int {
        if (character >= '0' && character <= '9') {
            return character - '0';
        }
        if (character >= 'a' && character <= 'f') {
            return character - 'a' + 10;
        }
        if (character >= 'A' && character <= 'F') {
            return character - 'A' + 10;
        }
        return -1;
    };

    for (std::size_t index = 0; index < encoded.size(); ++index) {
        const char character = encoded[index];
        if (character != '%') {
            if (character == '\0' || character == '\\') {
                return false;
            }
            decoded.push_back(character);
            continue;
        }
        if (index + 2U >= encoded.size()) {
            return false;
        }
        const int high = hexValue(encoded[index + 1U]);
        const int low = hexValue(encoded[index + 2U]);
        if (high < 0 || low < 0) {
            return false;
        }
        const char value = static_cast<char>((high << 4) | low);
        if (value == '\0' || value == '\\') {
            return false;
        }
        decoded.push_back(value);
        index += 2U;
    }
    return true;
}

std::string mimeTypeForPath(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    if (extension == ".html" || extension == ".htm") return "text/html; charset=utf-8";
    if (extension == ".css") return "text/css; charset=utf-8";
    if (extension == ".js" || extension == ".mjs") return "text/javascript; charset=utf-8";
    if (extension == ".json") return "application/json; charset=utf-8";
    if (extension == ".txt" || extension == ".md") return "text/plain; charset=utf-8";
    if (extension == ".xml") return "application/xml; charset=utf-8";
    if (extension == ".svg") return "image/svg+xml";
    if (extension == ".png") return "image/png";
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    if (extension == ".gif") return "image/gif";
    if (extension == ".webp") return "image/webp";
    if (extension == ".ico") return "image/x-icon";
    if (extension == ".wasm") return "application/wasm";
    if (extension == ".pdf") return "application/pdf";
    if (extension == ".woff") return "font/woff";
    if (extension == ".woff2") return "font/woff2";
    return "application/octet-stream";
}

struct HttpResponse {
    int status = 500;
    const char* reason = "Internal Server Error";
    std::string content_type = "text/plain; charset=utf-8";
    std::string body = "Internal Server Error\n";
    std::string extra_headers;
};

HttpResponse errorResponse(int status) {
    switch (status) {
        case 400:
            return {400, "Bad Request", "text/plain; charset=utf-8", "Bad Request\n", {}};
        case 404:
            return {404, "Not Found", "text/plain; charset=utf-8", "Not Found\n", {}};
        case 405:
            return {405, "Method Not Allowed", "text/plain; charset=utf-8",
                    "Method Not Allowed\n", "Allow: GET, HEAD\r\n"};
        case 413:
            return {413, "Content Too Large", "text/plain; charset=utf-8",
                    "Content Too Large\n", {}};
        case 431:
            return {431, "Request Header Fields Too Large", "text/plain; charset=utf-8",
                    "Request Header Fields Too Large\n", {}};
        case 503:
            return {503, "Service Unavailable", "text/plain; charset=utf-8",
                    "Service Unavailable\n", {}};
        default:
            return {};
    }
}

bool transmitResponse(SocketHandle socket_handle, const HttpResponse& response, bool head_only) {
    std::ostringstream header;
    header << "HTTP/1.1 " << response.status << ' ' << response.reason << "\r\n"
           << "Content-Type: " << response.content_type << "\r\n"
           << "Content-Length: " << response.body.size() << "\r\n"
           << response.extra_headers
           << "Connection: close\r\n"
           << "X-Content-Type-Options: nosniff\r\n\r\n";
    const std::string header_text = header.str();
    if (!sendAll(socket_handle, header_text.data(), header_text.size())) {
        return false;
    }
    return head_only || response.body.empty() ||
           sendAll(socket_handle, response.body.data(), response.body.size());
}

HttpResponse resolveFileResponse(const std::filesystem::path& document_root,
                                 const std::string& request_target) {
    const auto query_position = request_target.find('?');
    const std::string encoded_path = request_target.substr(0, query_position);
    if (encoded_path.empty() || encoded_path.front() != '/' ||
        encoded_path.find('#') != std::string::npos) {
        return errorResponse(400);
    }

    std::string decoded_path;
    if (!percentDecodePath(encoded_path, decoded_path)) {
        return errorResponse(400);
    }

    std::filesystem::path relative_path;
    std::size_t position = 1U;
    while (position <= decoded_path.size()) {
        const auto slash = decoded_path.find('/', position);
        const std::string segment = decoded_path.substr(
            position, slash == std::string::npos ? std::string::npos : slash - position);
        if (segment == "..") {
            return errorResponse(404);
        }
        if (!segment.empty() && segment != ".") {
            relative_path /= segment;
        }
        if (slash == std::string::npos) {
            break;
        }
        position = slash + 1U;
    }

    if (relative_path.empty() || decoded_path.back() == '/') {
        relative_path /= "index.html";
    }

    std::error_code error;
    const auto candidate = std::filesystem::canonical(document_root / relative_path, error);
    if (error || !hasPathPrefix(candidate, document_root)) {
        return errorResponse(404);
    }
    if (!std::filesystem::is_regular_file(candidate, error) || error) {
        return errorResponse(404);
    }

    const auto file_size = std::filesystem::file_size(candidate, error);
    if (error || file_size > kMaximumServedFileBytes ||
        file_size > static_cast<std::uintmax_t>(std::numeric_limits<std::streamsize>::max())) {
        return errorResponse(413);
    }

    std::ifstream file(candidate, std::ios::binary);
    if (!file.is_open()) {
        return errorResponse(404);
    }
    std::string body(static_cast<std::size_t>(file_size), '\0');
    if (!body.empty()) {
        file.read(body.data(), static_cast<std::streamsize>(body.size()));
        if (!file || static_cast<std::size_t>(file.gcount()) != body.size()) {
            return errorResponse(404);
        }
    }
    return {200, "OK", mimeTypeForPath(candidate), std::move(body), {}};
}

void serveClient(SocketHandle socket_handle, const std::filesystem::path& document_root) noexcept {
    try {
        std::string request;
        request.reserve(2048U);
        std::array<char, 2048U> buffer{};
        std::size_t header_end = std::string::npos;

        while (request.size() <= kMaximumRequestHeaderBytes) {
#ifdef _WIN32
            const int received = ::recv(socket_handle, buffer.data(),
                                        static_cast<int>(buffer.size()), 0);
#else
            const int received = static_cast<int>(
                ::recv(socket_handle, buffer.data(), buffer.size(), 0));
#endif
            if (received <= 0) {
                if (!request.empty()) {
                    (void)transmitResponse(socket_handle, errorResponse(400), false);
                }
                return;
            }
            request.append(buffer.data(), static_cast<std::size_t>(received));
            header_end = request.find("\r\n\r\n");
            if (header_end != std::string::npos) {
                break;
            }
        }

        if (header_end == std::string::npos) {
            (void)transmitResponse(socket_handle, errorResponse(431), false);
            return;
        }
        if (header_end + 4U > kMaximumRequestHeaderBytes) {
            (void)transmitResponse(socket_handle, errorResponse(431), false);
            return;
        }

        const auto first_line_end = request.find("\r\n");
        if (first_line_end == std::string::npos || first_line_end == 0U) {
            (void)transmitResponse(socket_handle, errorResponse(400), false);
            return;
        }

        std::string method;
        std::string target;
        std::string version;
        std::string extra;
        std::istringstream request_line(request.substr(0, first_line_end));
        if (!(request_line >> method >> target >> version) || (request_line >> extra) ||
            target.size() > kMaximumRequestTargetBytes ||
            (version != "HTTP/1.0" && version != "HTTP/1.1")) {
            (void)transmitResponse(socket_handle, errorResponse(400), false);
            return;
        }

        bool malformed_headers = false;
        bool has_transfer_encoding = false;
        std::size_t line_start = first_line_end + 2U;
        while (line_start < header_end) {
            const auto line_end = request.find("\r\n", line_start);
            if (line_end == std::string::npos || line_end > header_end || line_end == line_start) {
                malformed_headers = true;
                break;
            }
            const std::string line = request.substr(line_start, line_end - line_start);
            const auto colon = line.find(':');
            if (colon == std::string::npos || colon == 0U ||
                !std::all_of(line.begin(), line.begin() + static_cast<std::ptrdiff_t>(colon),
                             [](unsigned char character) {
                                 return isHeaderNameCharacter(character);
                             })) {
                malformed_headers = true;
                break;
            }
            for (std::size_t index = colon + 1U; index < line.size(); ++index) {
                const unsigned char character = static_cast<unsigned char>(line[index]);
                if ((character < 32U && character != '\t') || character == 127U) {
                    malformed_headers = true;
                    break;
                }
            }
            if (malformed_headers) {
                break;
            }
            std::string header_name = line.substr(0, colon);
            std::transform(header_name.begin(), header_name.end(), header_name.begin(),
                           [](unsigned char character) {
                               return static_cast<char>(std::tolower(character));
                           });
            if (header_name == "transfer-encoding") {
                has_transfer_encoding = true;
            }
            line_start = line_end + 2U;
        }

        if (malformed_headers || has_transfer_encoding) {
            (void)transmitResponse(socket_handle, errorResponse(400), method == "HEAD");
            return;
        }

        const bool head_only = method == "HEAD";
        if (method != "GET" && !head_only) {
            (void)transmitResponse(socket_handle, errorResponse(405), false);
            return;
        }

        (void)transmitResponse(socket_handle,
                               resolveFileResponse(document_root, target), head_only);
    } catch (...) {
        (void)transmitResponse(socket_handle, errorResponse(400), false);
    }
}

} // namespace

struct Website::DevelopmentServerState
    : std::enable_shared_from_this<Website::DevelopmentServerState> {
    mutable std::mutex mutex;
    std::mutex lifecycle_mutex;
    std::condition_variable lifecycle_changed;
    std::condition_variable queue_changed;
    bool lifecycle_transition = false;
    std::size_t deferred_joins = 0U;
    bool running = false;
    bool stopping = false;
    SocketHandle listen_socket = kInvalidSocket;
    std::uint16_t port = 0;
    std::filesystem::path document_root;
    std::string last_error;
    DevelopmentServerErrorCallback error_callback;
    std::deque<SocketHandle> pending_clients;
    std::unordered_set<SocketHandle> active_clients;
    std::thread accept_thread;
    std::vector<std::thread> workers;
#ifdef _WIN32
    bool winsock_started = false;
#endif

    bool beginLifecycle() noexcept {
        std::unique_lock<std::mutex> lock(lifecycle_mutex);
        while (lifecycle_transition || deferred_joins != 0U) {
            // A server callback can be the thread that the active lifecycle
            // transition is joining (or that the reaper is about to join). It
            // must publish stop and return rather than wait for itself.
            if (g_in_website_server_thread) {
                return false;
            }
            lifecycle_changed.wait(lock, [this]() {
                return !lifecycle_transition && deferred_joins == 0U;
            });
        }
        lifecycle_transition = true;
        return true;
    }

    void finishLifecycle() noexcept {
        {
            std::lock_guard<std::mutex> lock(lifecycle_mutex);
            lifecycle_transition = false;
        }
        lifecycle_changed.notify_all();
    }

    void reportError(const std::string& message) {
        DevelopmentServerErrorCallback callback;
        {
            std::lock_guard<std::mutex> lock(mutex);
            last_error = message;
            callback = error_callback;
        }
        g_website_logger.log(message, "", "website", LogLevel::ERROR);
        if (callback) {
            try {
                callback(message);
            } catch (...) {
                g_website_logger.log("Development server error callback threw an exception",
                                     "", "website", LogLevel::WARNING);
            }
        }
    }

    bool ownsCurrentThread() const noexcept {
        const std::thread::id caller_id = std::this_thread::get_id();
        if (accept_thread.joinable() && accept_thread.get_id() == caller_id) {
            return true;
        }
        return std::any_of(workers.begin(), workers.end(),
                           [caller_id](const std::thread& worker) {
                               return worker.joinable() && worker.get_id() == caller_id;
                           });
    }

    void publishStop() noexcept {
        SocketHandle socket_to_close = kInvalidSocket;
        {
            // Publish the queue wait predicate under the mutex used by
            // queue_changed.wait(). Identifying active descriptors under the
            // same lock also prevents fd reuse before shutdown.
            std::lock_guard<std::mutex> lock(mutex);
            running = false;
            stopping = true;
            socket_to_close = listen_socket;
            listen_socket = kInvalidSocket;
            port = 0;
            document_root.clear();
            for (SocketHandle client : pending_clients) {
                shutdownSocket(client);
                closeSocket(client);
            }
            pending_clients.clear();
            for (SocketHandle client : active_clients) {
                shutdownSocket(client);
            }
        }

        shutdownSocket(socket_to_close);
        closeSocket(socket_to_close);
        queue_changed.notify_all();
    }

    void completeDeferredJoin() noexcept {
        {
            std::lock_guard<std::mutex> lifecycle_lock(lifecycle_mutex);
            if (deferred_joins > 0U) {
                --deferred_joins;
            }
            if (deferred_joins == 0U) {
                std::lock_guard<std::mutex> lock(mutex);
                active_clients.clear();
                if (!running) {
                    stopping = false;
                }
#ifdef _WIN32
                if (!running && winsock_started) {
                    WSACleanup();
                    winsock_started = false;
                }
#endif
            }
        }
        lifecycle_changed.notify_all();
    }

    void deferSelfJoin(std::thread& thread) {
        {
            std::lock_guard<std::mutex> lock(lifecycle_mutex);
            ++deferred_joins;
        }
        const auto self = shared_from_this();
        ThreadReaper::instance().reap(
            std::move(thread), [self]() noexcept { self->completeDeferredJoin(); });
    }

    bool joinThreads() noexcept {
        const std::thread::id caller_id = std::this_thread::get_id();
        bool deferred_self = false;

        // The lifecycle transition, not a held mutex, gives this caller
        // exclusive ownership of the std::thread handles. No mutex needed by
        // accept/workers is held across join(). A self handle is transferred
        // to the process-wide join reaper; it is never detached.
        if (accept_thread.joinable()) {
            if (accept_thread.get_id() == caller_id) {
                deferSelfJoin(accept_thread);
                deferred_self = true;
            } else {
                accept_thread.join();
            }
        }
        for (auto& worker : workers) {
            if (!worker.joinable()) {
                continue;
            }
            if (worker.get_id() == caller_id) {
                deferSelfJoin(worker);
                deferred_self = true;
            } else {
                worker.join();
            }
        }

        if (!accept_thread.joinable()) {
            accept_thread = std::thread{};
        }
        workers.erase(
            std::remove_if(workers.begin(), workers.end(),
                           [](const std::thread& worker) { return !worker.joinable(); }),
            workers.end());
        return deferred_self;
    }

    void finishStoppedState(bool deferred_self) noexcept {
        if (deferred_self) {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex);
        active_clients.clear();
        stopping = false;
#ifdef _WIN32
        if (winsock_started) {
            WSACleanup();
            winsock_started = false;
        }
#endif
    }
};

// TemplateEngine implementation
TemplateEngine::TemplateEngine() = default;
TemplateEngine::~TemplateEngine() = default;

bool TemplateEngine::loadTemplate(const std::string& name,
                                  const std::filesystem::path& template_path) {
    if (name.empty() || template_path.empty()) {
        return false;
    }
    try {
        std::ifstream file(template_path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        std::ostringstream buffer;
        buffer << file.rdbuf();
        if (!file.good() && !file.eof()) {
            return false;
        }
        templates_[name] = buffer.str();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool TemplateEngine::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

std::string TemplateEngine::render(
    const std::string& template_name,
    const std::unordered_map<std::string, std::string>& variables) const {
    const auto iterator = templates_.find(template_name);
    if (iterator == templates_.end()) {
        return {};
    }
    return substituteVariables(iterator->second, variables);
}

std::string TemplateEngine::renderString(
    const std::string& template_content,
    const std::unordered_map<std::string, std::string>& variables) const {
    return substituteVariables(template_content, variables);
}

void TemplateEngine::setGlobalVariable(const std::string& key, const std::string& value) {
    if (!key.empty()) {
        global_variables_[key] = value;
    }
}

std::string TemplateEngine::getGlobalVariable(const std::string& key) const {
    const auto iterator = global_variables_.find(key);
    return iterator != global_variables_.end() ? iterator->second : std::string{};
}

std::string TemplateEngine::substituteVariables(
    const std::string& content,
    const std::unordered_map<std::string, std::string>& variables) const {
    auto all_variables = global_variables_;
    for (const auto& variable : variables) {
        all_variables[variable.first] = variable.second;
    }

    const std::regex variable_regex(R"(\{\{(\w+)\}\})");
    std::smatch matches;
    std::string::const_iterator start = content.cbegin();
    std::ostringstream output;
    while (std::regex_search(start, content.cend(), matches, variable_regex)) {
        output << std::string(start, matches[0].first);
        const auto variable = all_variables.find(matches[1].str());
        output << (variable != all_variables.end() ? variable->second : matches[0].str());
        start = matches[0].second;
    }
    output << std::string(start, content.cend());
    return output.str();
}

// ContentManager implementation
ContentManager::ContentManager(const WebsiteConfig& config) : config_(config) {}
ContentManager::~ContentManager() = default;

bool ContentManager::addPage(const WebPage& page) {
    if (page.id.empty() || page.id == "." || page.id == ".." ||
        page.id.find('/') != std::string::npos || page.id.find('\\') != std::string::npos) {
        return false;
    }
    pages_[page.id] = std::make_shared<WebPage>(page);
    return true;
}

bool ContentManager::removePage(const std::string& page_id) {
    return pages_.erase(page_id) > 0U;
}

std::shared_ptr<WebPage> ContentManager::getPage(const std::string& page_id) const {
    const auto iterator = pages_.find(page_id);
    return iterator != pages_.end() ? iterator->second : nullptr;
}

std::vector<std::shared_ptr<WebPage>> ContentManager::getAllPages() const {
    std::vector<std::shared_ptr<WebPage>> result;
    result.reserve(pages_.size());
    for (const auto& entry : pages_) {
        result.push_back(entry.second);
    }
    return result;
}

std::vector<std::shared_ptr<WebPage>> ContentManager::getPagesByTemplate(
    const std::string& template_name) const {
    std::vector<std::shared_ptr<WebPage>> result;
    for (const auto& entry : pages_) {
        if (entry.second->template_name == template_name) {
            result.push_back(entry.second);
        }
    }
    return result;
}

bool ContentManager::loadPagesFromDirectory(const std::filesystem::path& directory) {
    try {
        std::error_code error;
        if (!std::filesystem::is_directory(directory, error) || error) {
            return false;
        }
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (!entry.is_regular_file() ||
                (!isMarkdownFile(entry.path()) && !isHtmlFile(entry.path()))) {
                continue;
            }
            std::ifstream file(entry.path(), std::ios::binary);
            if (!file.is_open()) {
                continue;
            }
            std::ostringstream buffer;
            buffer << file.rdbuf();
            const std::string content = buffer.str();
            const std::string page_id = generatePageId(entry.path());
            const auto metadata = parsePageMetadata(content);
            const std::string clean_content = stripMetadata(content);
            const auto title = metadata.find("title");
            WebPage page(page_id,
                         title != metadata.end() ? title->second : entry.path().stem().string(),
                         clean_content);
            page.source_path = entry.path();
            page.metadata = metadata;
            const auto page_template = metadata.find("template");
            page.template_name = page_template != metadata.end()
                                     ? page_template->second
                                     : (isMarkdownFile(entry.path()) ? "markdown" : "html");
            if (!addPage(page)) {
                return false;
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ContentManager::savePage(const WebPage& page,
                              const std::filesystem::path& output_path) const {
    if (output_path.empty()) {
        return false;
    }
    try {
        const auto parent = output_path.parent_path();
        if (!parent.empty() && !ensureDirectory(parent)) {
            return false;
        }
        std::ofstream file(output_path, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }
        file << page.content;
        return file.good();
    } catch (const std::exception&) {
        return false;
    }
}

std::unordered_map<std::string, std::string> ContentManager::parsePageMetadata(
    const std::string& content) const {
    std::unordered_map<std::string, std::string> metadata;
    if (content.rfind("---\n", 0U) != 0U && content.rfind("---\r\n", 0U) != 0U) {
        return metadata;
    }

    const std::size_t opening_size = content.rfind("---\r\n", 0U) == 0U ? 5U : 4U;
    const std::string delimiter = opening_size == 5U ? "\r\n---\r\n" : "\n---\n";
    const auto end_position = content.find(delimiter, opening_size);
    if (end_position == std::string::npos) {
        return metadata;
    }

    std::istringstream stream(content.substr(opening_size, end_position - opening_size));
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        const auto colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        const std::string key = trim(line.substr(0, colon));
        const std::string value = trim(line.substr(colon + 1U));
        if (!key.empty()) {
            metadata[key] = value;
        }
    }
    return metadata;
}

std::string ContentManager::stripMetadata(const std::string& content) const {
    if (content.rfind("---\n", 0U) != 0U && content.rfind("---\r\n", 0U) != 0U) {
        return content;
    }
    const std::size_t opening_size = content.rfind("---\r\n", 0U) == 0U ? 5U : 4U;
    const std::string delimiter = opening_size == 5U ? "\r\n---\r\n" : "\n---\n";
    const auto end_position = content.find(delimiter, opening_size);
    return end_position == std::string::npos
               ? content
               : content.substr(end_position + delimiter.size());
}

std::string ContentManager::generatePageId(const std::filesystem::path& file_path) const {
    std::string id = file_path.stem().string();
    std::replace(id.begin(), id.end(), ' ', '_');
    std::transform(id.begin(), id.end(), id.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return id;
}

bool ContentManager::isMarkdownFile(const std::filesystem::path& file_path) const {
    std::string extension = file_path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return extension == ".md" || extension == ".markdown";
}

bool ContentManager::isHtmlFile(const std::filesystem::path& file_path) const {
    std::string extension = file_path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return extension == ".html" || extension == ".htm";
}

// StaticSiteGenerator implementation
StaticSiteGenerator::StaticSiteGenerator(const WebsiteConfig& config) : config_(config) {}
StaticSiteGenerator::~StaticSiteGenerator() = default;

bool StaticSiteGenerator::generateSite() {
    const auto start_time = std::chrono::steady_clock::now();
    last_stats_ = GenerationStats{};
    if (!ensureOutputDirectory()) {
        ++last_stats_.errors;
        last_stats_.error_messages.push_back("Failed to create output directory");
        return false;
    }
    if (!content_manager_ || !template_engine_) {
        ++last_stats_.errors;
        last_stats_.error_messages.push_back("Content manager or template engine not set");
        return false;
    }

    for (const auto& page : content_manager_->getAllPages()) {
        if (page && generatePageFile(*page)) {
            ++last_stats_.pages_generated;
        } else {
            ++last_stats_.errors;
            last_stats_.error_messages.push_back(
                "Failed to generate page: " + (page ? page->id : std::string{"<null>"}));
        }
    }
    if (!copyAssets()) {
        ++last_stats_.errors;
        last_stats_.error_messages.push_back("Failed to copy assets");
    }
    last_stats_.generation_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time);
    return last_stats_.errors == 0U;
}

bool StaticSiteGenerator::generatePage(const std::string& page_id) {
    if (!content_manager_) {
        return false;
    }
    const auto page = content_manager_->getPage(page_id);
    return page && generatePageFile(*page);
}

bool StaticSiteGenerator::copyAssets() {
    try {
        std::error_code error;
        if (!std::filesystem::exists(config_.assets_dir, error)) {
            return !error;
        }
        if (error || !std::filesystem::is_directory(config_.assets_dir, error) || error) {
            return false;
        }
        const auto target_assets_directory = config_.output_dir / "assets";
        if (!copyDirectory(config_.assets_dir, target_assets_directory)) {
            return false;
        }
        std::size_t copied = 0U;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(config_.assets_dir)) {
            if (entry.is_regular_file()) {
                ++copied;
            }
        }
        last_stats_.assets_copied = copied;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool StaticSiteGenerator::cleanOutputDirectory() {
    if (!isSafeDirectoryPath(config_.output_dir)) {
        return false;
    }
    try {
        std::error_code error;
        if (std::filesystem::exists(config_.output_dir, error)) {
            if (error || !std::filesystem::is_directory(config_.output_dir, error) || error) {
                return false;
            }
            std::filesystem::remove_all(config_.output_dir, error);
            if (error) {
                return false;
            }
        }
        return ensureDirectory(config_.output_dir);
    } catch (const std::exception&) {
        return false;
    }
}

void StaticSiteGenerator::setContentManager(
    std::shared_ptr<ContentManager> content_manager) {
    content_manager_ = std::move(content_manager);
}

void StaticSiteGenerator::setTemplateEngine(
    std::shared_ptr<TemplateEngine> template_engine) {
    template_engine_ = std::move(template_engine);
}

bool StaticSiteGenerator::ensureOutputDirectory() {
    return ensureDirectory(config_.output_dir);
}

bool StaticSiteGenerator::generatePageFile(const WebPage& page) {
    if (!content_manager_ || !template_engine_ || page.id.empty() ||
        !template_engine_->hasTemplate(page.template_name) || !ensureOutputDirectory()) {
        return false;
    }

    std::unordered_map<std::string, std::string> variables;
    variables["title"] = page.title;
    variables["content"] = page.template_name == "markdown"
                               ? markdownToHtml(page.content)
                               : page.content;
    for (const auto& metadata : page.metadata) {
        variables[metadata.first] = metadata.second;
    }
    variables["site_title"] = config_.site_title;
    variables["site_description"] = config_.site_description;
    variables["base_url"] = config_.base_url;

    const std::string rendered = template_engine_->render(page.template_name, variables);
    if (rendered.empty()) {
        return false;
    }

    std::error_code error;
    const auto output_root = std::filesystem::absolute(config_.output_dir, error).lexically_normal();
    if (error) {
        return false;
    }
    const auto configured_output = page.output_path.empty()
                                       ? config_.output_dir / (page.id + ".html")
                                       : page.output_path;
    const auto output_path = std::filesystem::absolute(configured_output, error).lexically_normal();
    if (error || !hasPathPrefix(output_path, output_root)) {
        return false;
    }

    WebPage rendered_page(page.id, page.title, rendered);
    return content_manager_->savePage(rendered_page, output_path);
}

bool StaticSiteGenerator::copyFile(const std::filesystem::path& source,
                                   const std::filesystem::path& destination) {
    try {
        const auto parent = destination.parent_path();
        if (!parent.empty() && !ensureDirectory(parent)) {
            return false;
        }
        std::error_code error;
        return std::filesystem::copy_file(source, destination,
                                          std::filesystem::copy_options::overwrite_existing,
                                          error) && !error;
    } catch (const std::exception&) {
        return false;
    }
}

bool StaticSiteGenerator::copyDirectory(const std::filesystem::path& source,
                                        const std::filesystem::path& destination) {
    try {
        std::error_code error;
        if (!std::filesystem::is_directory(source, error) || error ||
            !ensureDirectory(destination)) {
            return false;
        }
        for (const auto& entry : std::filesystem::recursive_directory_iterator(source)) {
            const auto relative = std::filesystem::relative(entry.path(), source, error);
            if (error) {
                return false;
            }
            const auto target = destination / relative;
            if (entry.is_directory()) {
                if (!ensureDirectory(target)) {
                    return false;
                }
            } else if (entry.is_regular_file() && !copyFile(entry.path(), target)) {
                return false;
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string StaticSiteGenerator::markdownToHtml(const std::string& markdown) const {
    std::string html = markdown;
    html = std::regex_replace(html, std::regex(R"((^|\n)# (.+)($|\n))"),
                              "$1<h1>$2</h1>$3");
    html = std::regex_replace(html, std::regex(R"((^|\n)## (.+)($|\n))"),
                              "$1<h2>$2</h2>$3");
    html = std::regex_replace(html, std::regex(R"((^|\n)### (.+)($|\n))"),
                              "$1<h3>$2</h3>$3");
    html = std::regex_replace(html, std::regex(R"(\n\n)"), "</p>\n<p>");
    html = "<p>" + html + "</p>";
    return std::regex_replace(html, std::regex(R"(\n)"), "<br>\n");
}

// Website implementation
Website::Website(const WebsiteConfig& config)
    : config_(config), development_server_(std::make_shared<DevelopmentServerState>()) {}

Website::~Website() {
    stopDevelopmentServer();
    if (development_server_) {
        std::lock_guard<std::mutex> lock(development_server_->mutex);
        development_server_->error_callback = {};
    }
}

bool Website::initialize() {
    if (initialized_) {
        return true;
    }
    if (!setupDirectories()) {
        return false;
    }

    auto content_manager = std::make_shared<ContentManager>(config_);
    auto template_engine = std::make_shared<TemplateEngine>();
    auto generator = std::make_shared<StaticSiteGenerator>(config_);
    generator->setContentManager(content_manager);
    generator->setTemplateEngine(template_engine);

    content_manager_ = std::move(content_manager);
    template_engine_ = std::move(template_engine);
    generator_ = std::move(generator);
    if (!loadDefaultTemplates()) {
        content_manager_.reset();
        template_engine_.reset();
        generator_.reset();
        return false;
    }
    (void)content_manager_->loadPagesFromDirectory(config_.source_dir);
    initialized_ = true;
    return true;
}

bool Website::generateSite() {
    return initialized_ && generator_ && generator_->generateSite();
}

bool Website::startDevelopmentServer(int port) {
    const auto state = development_server_;
    if (!state) {
        return false;
    }

    if (!state->beginLifecycle()) {
        state->publishStop();
        return false;
    }

    bool already_running = false;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        already_running = state->running;
    }
    if (already_running) {
        state->finishLifecycle();
        state->reportError("Development server is already running; stop it before restarting");
        return false;
    }

    // A callback self-stop leaves its own std::thread handle joinable. Reap
    // that completed server generation before creating the next one.
    if (state->accept_thread.joinable() || !state->workers.empty()) {
        if (state->ownsCurrentThread()) {
            state->finishLifecycle();
            return false;
        }
        state->publishStop();
        const bool retained_self = state->joinThreads();
        state->finishStoppedState(retained_self);
    }
    if (!initialized_ || !generator_) {
        state->finishLifecycle();
        state->reportError("Website must be initialized before starting the development server");
        return false;
    }
    if (port < 0 || port > 65535) {
        state->finishLifecycle();
        state->reportError("Development server port must be between 0 and 65535");
        return false;
    }

    std::error_code filesystem_error;
    const auto document_root = std::filesystem::canonical(config_.output_dir, filesystem_error);
    if (filesystem_error || !std::filesystem::is_directory(document_root, filesystem_error) ||
        filesystem_error) {
        state->finishLifecycle();
        state->reportError("Development server document root is not a readable directory");
        return false;
    }

#ifdef _WIN32
    WSADATA winsock_data{};
    if (WSAStartup(MAKEWORD(2, 2), &winsock_data) != 0) {
        state->finishLifecycle();
        state->reportError("Failed to initialize WinSock for the development server");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->winsock_started = true;
    }
#endif

    SocketHandle listen_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == kInvalidSocket) {
        const std::string message = socketErrorMessage("socket", lastSocketError());
#ifdef _WIN32
        WSACleanup();
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->winsock_started = false;
        }
#endif
        state->finishLifecycle();
        state->reportError(message);
        return false;
    }

    int reuse_address = 1;
#ifdef _WIN32
    (void)::setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR,
                       reinterpret_cast<const char*>(&reuse_address),
                       static_cast<int>(sizeof(reuse_address)));
#else
    (void)::setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_address,
                       static_cast<socklen_t>(sizeof(reuse_address)));
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(static_cast<std::uint16_t>(port));
    if (::bind(listen_socket, reinterpret_cast<const sockaddr*>(&address),
               static_cast<socklen_t>(sizeof(address))) != 0) {
        const std::string message = socketErrorMessage("bind", lastSocketError());
        closeSocket(listen_socket);
#ifdef _WIN32
        WSACleanup();
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->winsock_started = false;
        }
#endif
        state->finishLifecycle();
        state->reportError(message);
        return false;
    }
    if (::listen(listen_socket, SOMAXCONN) != 0) {
        const std::string message = socketErrorMessage("listen", lastSocketError());
        closeSocket(listen_socket);
#ifdef _WIN32
        WSACleanup();
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->winsock_started = false;
        }
#endif
        state->finishLifecycle();
        state->reportError(message);
        return false;
    }

    sockaddr_in selected_address{};
    socklen_t selected_address_size = static_cast<socklen_t>(sizeof(selected_address));
    if (::getsockname(listen_socket, reinterpret_cast<sockaddr*>(&selected_address),
                      &selected_address_size) != 0) {
        const std::string message = socketErrorMessage("getsockname", lastSocketError());
        closeSocket(listen_socket);
#ifdef _WIN32
        WSACleanup();
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->winsock_started = false;
        }
#endif
        state->finishLifecycle();
        state->reportError(message);
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->stopping = false;
        state->running = true;
        state->listen_socket = listen_socket;
        state->port = ntohs(selected_address.sin_port);
        state->document_root = document_root;
        state->last_error.clear();
    }

    try {
        state->workers.reserve(kWorkerCount);
        for (std::size_t worker_index = 0; worker_index < kWorkerCount; ++worker_index) {
            state->workers.emplace_back([state]() {
                ServerThreadScope server_thread_scope;
                for (;;) {
                    SocketHandle client = kInvalidSocket;
                    std::filesystem::path root;
                    {
                        std::unique_lock<std::mutex> lock(state->mutex);
                        state->queue_changed.wait(lock, [&state]() {
                            return state->stopping || !state->pending_clients.empty();
                        });
                        if (state->pending_clients.empty()) {
                            if (state->stopping) {
                                break;
                            }
                            continue;
                        }
                        client = state->pending_clients.front();
                        state->pending_clients.pop_front();
                        state->active_clients.insert(client);
                        root = state->document_root;
                    }
                    serveClient(client, root);
                    {
                        std::lock_guard<std::mutex> lock(state->mutex);
                        state->active_clients.erase(client);
                    }
                    shutdownSocket(client);
                    closeSocket(client);
                }
            });
        }

        state->accept_thread = std::thread([state, listen_socket]() {
            ServerThreadScope server_thread_scope;
            for (;;) {
                sockaddr_in peer{};
                socklen_t peer_size = static_cast<socklen_t>(sizeof(peer));
                const SocketHandle client = ::accept(
                    listen_socket, reinterpret_cast<sockaddr*>(&peer), &peer_size);
                if (client == kInvalidSocket) {
                    const int error_code = lastSocketError();
                    bool stopping = false;
                    std::string accept_error;
                    {
                        std::lock_guard<std::mutex> lock(state->mutex);
                        stopping = state->stopping;
                        if (!stopping && !isInterruptedSocketError(error_code)) {
                            accept_error = socketErrorMessage("accept", error_code);
                            state->running = false;
                            state->stopping = true;
                        }
                    }
                    if (stopping) {
                        break;
                    }
                    if (isInterruptedSocketError(error_code)) {
                        continue;
                    }
                    state->queue_changed.notify_all();
                    state->reportError(accept_error);
                    break;
                }

                (void)configureClientTimeouts(client);
                bool stopping = false;
                bool queue_full = false;
                {
                    std::lock_guard<std::mutex> lock(state->mutex);
                    stopping = state->stopping;
                    if (!stopping &&
                        state->pending_clients.size() >= kMaximumQueuedClients) {
                        queue_full = true;
                    } else if (!stopping) {
                        state->pending_clients.push_back(client);
                    }
                }
                if (stopping) {
                    shutdownSocket(client);
                    closeSocket(client);
                    break;
                }
                if (queue_full) {
                    (void)transmitResponse(client, errorResponse(503), false);
                    shutdownSocket(client);
                    closeSocket(client);
                    state->reportError("Development server client queue is full");
                } else {
                    state->queue_changed.notify_one();
                }
            }
        });
    } catch (const std::exception& exception) {
        state->publishStop();
        const bool retained_self = state->joinThreads();
        state->finishStoppedState(retained_self);
        const std::string message =
            std::string{"Failed to create development server threads: "} + exception.what();
        state->finishLifecycle();
        state->reportError(message);
        return false;
    }

    const std::uint16_t selected_port = ntohs(selected_address.sin_port);
    state->finishLifecycle();
    g_website_logger.log("Development server started on http://127.0.0.1:" +
                             std::to_string(selected_port),
                         "", "website", LogLevel::INFO);
    return true;
}

bool Website::serveDevelopmentSite(int port) {
    return startDevelopmentServer(port);
}

void Website::stopDevelopmentServer() noexcept {
    const auto state = development_server_;
    if (!state) {
        return;
    }

    if (!state->beginLifecycle()) {
        state->publishStop();
        return;
    }
    state->publishStop();
    const bool retained_self = state->joinThreads();
    state->finishStoppedState(retained_self);
    state->finishLifecycle();
}

bool Website::isDevelopmentServerRunning() const noexcept {
    const auto state = development_server_;
    if (!state) {
        return false;
    }
    std::lock_guard<std::mutex> lock(state->mutex);
    return state->running;
}

std::uint16_t Website::getDevelopmentServerPort() const noexcept {
    const auto state = development_server_;
    if (!state) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(state->mutex);
    return state->port;
}

std::filesystem::path Website::getDevelopmentServerDocumentRoot() const {
    const auto state = development_server_;
    if (!state) {
        return {};
    }
    std::lock_guard<std::mutex> lock(state->mutex);
    return state->document_root;
}

std::string Website::getLastDevelopmentServerError() const {
    const auto state = development_server_;
    if (!state) {
        return "Development server state is unavailable";
    }
    std::lock_guard<std::mutex> lock(state->mutex);
    return state->last_error;
}

void Website::setDevelopmentServerErrorCallback(DevelopmentServerErrorCallback callback) {
    const auto state = development_server_;
    if (!state) {
        return;
    }
    std::lock_guard<std::mutex> lock(state->mutex);
    state->error_callback = std::move(callback);
}

bool Website::watchForChanges(bool enable) {
    if (!enable) {
        watching_ = false;
        return true;
    }
    watching_ = false;
    g_website_logger.log(
        "File watching is unavailable because no filesystem-watcher adapter is configured",
        "", "website", LogLevel::WARNING);
    return false;
}

bool Website::updateConfig(const WebsiteConfig& config) {
    const auto state = development_server_;
    if (!state || !state->beginLifecycle()) {
        return false;
    }
    struct LifecycleCompletion {
        std::shared_ptr<DevelopmentServerState> state;
        ~LifecycleCompletion() { state->finishLifecycle(); }
    } lifecycle_completion{state};

    {
        std::lock_guard<std::mutex> lock(state->mutex);
        if (state->running || state->stopping || state->accept_thread.joinable() ||
            !state->workers.empty()) {
            return false;
        }
    }
    if (!isSafeDirectoryPath(config.source_dir) || !isSafeDirectoryPath(config.output_dir) ||
        !isSafeDirectoryPath(config.templates_dir) || !isSafeDirectoryPath(config.assets_dir)) {
        return false;
    }
    config_ = config;
    if (content_manager_) {
        content_manager_->updateConfig(config);
    }
    if (generator_) {
        generator_->updateConfig(config);
    }
    if (template_engine_) {
        template_engine_->setGlobalVariable("site_title", config_.site_title);
        template_engine_->setGlobalVariable("site_description", config_.site_description);
        template_engine_->setGlobalVariable("base_url", config_.base_url);
    }
    return true;
}

StaticSiteGenerator::GenerationStats Website::getGenerationStats() const {
    return generator_ ? generator_->getLastGenerationStats()
                      : StaticSiteGenerator::GenerationStats{};
}

bool Website::setupDirectories() {
    return ensureDirectory(config_.source_dir) && ensureDirectory(config_.output_dir) &&
           ensureDirectory(config_.templates_dir) && ensureDirectory(config_.assets_dir);
}

bool Website::loadDefaultTemplates() {
    if (!template_engine_) {
        return false;
    }

    const std::string default_html_template = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{title}} - {{site_title}}</title>
    <meta name="description" content="{{site_description}}">
</head>
<body>
    <header>
        <h1>{{site_title}}</h1>
    </header>
    <main>
        <h1>{{title}}</h1>
        {{content}}
    </main>
    <footer>
        <p>&copy; 2024 {{site_title}}. All rights reserved.</p>
    </footer>
</body>
</html>)";

    const auto default_template_path = config_.templates_dir / "default.html";
    if (!std::filesystem::exists(default_template_path)) {
        std::ofstream file(default_template_path, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }
        file << default_html_template;
        if (!file.good()) {
            return false;
        }
    }

    if (!template_engine_->loadTemplate("html", default_template_path) ||
        !template_engine_->loadTemplate("markdown", default_template_path)) {
        return false;
    }
    template_engine_->setGlobalVariable("site_title", config_.site_title);
    template_engine_->setGlobalVariable("site_description", config_.site_description);
    template_engine_->setGlobalVariable("base_url", config_.base_url);
    for (const auto& variable : config_.global_vars) {
        template_engine_->setGlobalVariable(variable.first, variable.second);
    }
    return true;
}

} // namespace elizaos
