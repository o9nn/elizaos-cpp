#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "env.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Custom WebSocket wrapper to maintain Socket.io-like API
class SocketWrapper {
public:
    SocketWrapper(const std::string& url);
    void connect();
    void attemptReconnect();
    void triggerEvent(const std::string& event, const std::any& data);
    std::function<void()> async(const std::string& event, std::optional<std::any> data);

private:
    std::string url_;
};

// Create a type that matches the Socket.io Socket interface
using Socket = SocketWrapper;

    // Get apiUrl from environment and ensure it's defined
    // Pass the guaranteed string to the constructor

} // namespace elizaos
