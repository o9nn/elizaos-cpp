#pragma once

#include "elizaos/livevideochat.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace elizaos {

/** A bounded HTTP request as presented to a registered route handler. */
struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> query_params;
};

/** An HTTP response. Server-owned framing headers override handler values. */
struct HttpResponse {
    int status_code = 200;
    std::string body;
    std::map<std::string, std::string> headers;

    HttpResponse() {
        headers["Content-Type"] = "application/json";
        headers["Access-Control-Allow-Origin"] = "*";
        headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, DELETE, OPTIONS";
        headers["Access-Control-Allow-Headers"] = "Content-Type, Authorization";
    }
};

using HttpHandler = std::function<HttpResponse(const HttpRequest&)>;
using AgentMessageHandler = std::function<HttpResponse(const HttpRequest&)>;
using TranscriptionHandler = std::function<HttpResponse(const HttpRequest&)>;
using SignalingHandler = std::function<HttpResponse(const HttpRequest&)>;
using WebSocketHandler =
    std::function<void(const std::string& client_id, const std::string& message)>;

/** Injectable acknowledged outbound WebSocket connection boundary. */
class WebSocketClientAdapter {
public:
    virtual ~WebSocketClientAdapter() = default;
    virtual bool send(const std::string& message) = 0;
};

/**
 * Small loopback HTTP/1.1 server. initialize(0) requests an ephemeral port.
 * Requests are bounded and each connection is closed after one response.
 */
class LiveVideoChatServer {
public:
    LiveVideoChatServer();
    ~LiveVideoChatServer();

    LiveVideoChatServer(const LiveVideoChatServer&) = delete;
    LiveVideoChatServer& operator=(const LiveVideoChatServer&) = delete;

    bool initialize(int port = 3000);
    bool start();
    void stop();

    /** Stores only a weak reference; callers retain ownership. */
    void setVideoChatInstance(std::shared_ptr<LiveVideoChat> video_chat);

    /** Registers an exact or ECMAScript-regex path for an uppercase method. */
    void registerRoute(const std::string& method, const std::string& path, HttpHandler handler);

    /** Direct dispatch through the same validation and routes as socket requests. */
    HttpResponse handleRequest(const HttpRequest& request) const;

    void setWebSocketHandler(WebSocketHandler handler);

    /** Registration and acknowledgment are deliberately separate operations. */
    bool registerWebSocketClient(
        const std::string& client_id,
        std::shared_ptr<WebSocketClientAdapter> adapter);
    bool acknowledgeWebSocketClient(const std::string& client_id);
    void unregisterWebSocketClient(const std::string& client_id);
    bool handleWebSocketMessage(const std::string& client_id, const std::string& message);

    /** Returns true only when an acknowledged adapter accepts the message. */
    bool sendWebSocketMessage(const std::string& client_id, const std::string& message);
    void broadcastMessage(const std::string& message);

    bool isRunning() const;
    int getPort() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/** LiveVideoChat plus synchronized local HTTP API state. */
class LiveVideoChatWithServer : public LiveVideoChat {
public:
    LiveVideoChatWithServer();
    ~LiveVideoChatWithServer();

    LiveVideoChatWithServer(const LiveVideoChatWithServer&) = delete;
    LiveVideoChatWithServer& operator=(const LiveVideoChatWithServer&) = delete;

    bool initialize(const VideoChatConfig& config = {}, int server_port = 3000);
    bool startServer();
    void stopServer();
    std::shared_ptr<LiveVideoChatServer> getServer();

    void setAgentMessageHandler(AgentMessageHandler handler);
    void setTranscriptionHandler(TranscriptionHandler handler);
    void setSignalingHandler(SignalingHandler handler);

    HttpResponse handleAgentMessage(const HttpRequest& request);
    HttpResponse handleWhisperTranscription(const HttpRequest& request);
    HttpResponse handleWebRTCSignaling(const HttpRequest& request);
    HttpResponse handleSessionManagement(const HttpRequest& request);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace elizaos
