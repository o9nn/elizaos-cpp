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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * A simple in-memory message bus for distributing messages from the server
 *
 * For multi-process or multi-server deployments, this would need to be replaced
 * with a more robust solution like Redis Pub/Sub, Kafka, RabbitMQ, etc.
 *
 * Uses Bun-native EventTarget internally but maintains EventEmitter-like API.
 *
 * NOTE: This implementation uses a class extending EventTarget rather than functional
 * patterns because EventTarget is a native browser/Bun API that requires class inheritance.
 * This is an intentional architectural decision to leverage Bun's native capabilities
 * instead of Node.js EventEmitter for better compatibility.
 *
 * NOTE: Unlike standard EventEmitter, this implementation prevents duplicate handler
 * registration. This is an intentional design choice to prevent memory leaks and
 * unintended multiple executions of the same handler.
 */
class InternalMessageBus {
public:
    bool emit(const std::string& event, const std::any& data);
    void setMaxListeners(double n);
    void removeAllListeners(std::optional<std::string> event);
};

// Increase the default max listeners if many agents might be running in one process


} // namespace elizaos
