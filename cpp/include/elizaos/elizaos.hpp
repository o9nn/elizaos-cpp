#pragma once

/**
 * ElizaOS C++ - Master Header
 * 
 * This header provides stub declarations for all ElizaOS modules.
 * These are placeholder declarations to allow tests to compile.
 * Actual implementations are in the respective module directories.
 */

#include <any>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// Forward declarations for common types
class Agent;
class Memory;
class Action;
class Plugin;
class Character;

/**
 * UUID generator utility
 */
std::string generateUUID();

/**
 * Base configuration structure
 */
struct Config {
    std::unordered_map<std::string, std::string> settings;
};

/**
 * Base result type for operations
 */
template<typename T>
struct Result {
    bool success = false;
    std::optional<T> value;
    std::optional<std::string> error;
};

} // namespace elizaos
