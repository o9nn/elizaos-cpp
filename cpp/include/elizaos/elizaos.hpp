#pragma once

/**
 * ElizaOS C++ - Master Header
 *
 * Provides all foundational type definitions used across modules.
 * Include this header (or the individual module headers that include it)
 * to access the full ElizaOS C++ API.
 */

#include <any>
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// ---------------------------------------------------------------------------
// Fundamental type aliases
// ---------------------------------------------------------------------------

/** Universally-unique identifier string (e.g. "550e8400-e29b-41d4-a716-…") */
using UUID = std::string;

/** Dense floating-point embedding vector */
using EmbeddingVector = std::vector<float>;

/** Timestamp */
using Timestamp = std::chrono::system_clock::time_point;

/**
 * Generic JSON-like value type.
 * Keys are always strings; values can be strings, numbers, booleans, null,
 * nested maps, or vectors of the same.
 */
using JsonValue = std::unordered_map<std::string, std::string>;

// ---------------------------------------------------------------------------
// Common result wrapper
// ---------------------------------------------------------------------------

template<typename T>
struct Result {
    bool success = false;
    std::optional<T> value;
    std::optional<std::string> error;

    static Result<T> ok(T v) { return {true, std::move(v), std::nullopt}; }
    static Result<T> fail(const std::string& msg) { return {false, std::nullopt, msg}; }
};

// ---------------------------------------------------------------------------
// Base configuration
// ---------------------------------------------------------------------------

struct Config {
    std::unordered_map<std::string, std::string> settings;

    std::string get(const std::string& key, const std::string& def = "") const {
        auto it = settings.find(key);
        return it != settings.end() ? it->second : def;
    }
    void set(const std::string& key, const std::string& value) {
        settings[key] = value;
    }
};

// ---------------------------------------------------------------------------
// UUID generation (single canonical implementation)
// ---------------------------------------------------------------------------

std::string generateUUID();
std::string generateSimpleUUID();   // alias for backward-compat

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------

class Agent;
class Memory;
class Action;
class Plugin;
class Character;
class AgentMemoryManager;

} // namespace elizaos

// ---------------------------------------------------------------------------
// operator<< for std::chrono::system_clock::time_point
// Needed to make gtest EXPECT_EQ/EXPECT_NE print timestamps in failure msgs
// ---------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& os,
                                 const std::chrono::system_clock::time_point& tp) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch()).count();
    return os << "<time_point:" << ms << "ms>";
}
