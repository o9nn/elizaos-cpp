#pragma once
/**
 * @file utils.hpp
 * @brief Common utility functions for ElizaOS C++ implementation
 * 
 * This header provides fundamental utility functions used across the ElizaOS
 * framework, including string manipulation, UUID generation, time utilities,
 * logging, and common algorithms.
 */

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// ============================================================================
// UUID Generation
// ============================================================================

/**
 * @brief Generate a UUID v4 string
 * @return A UUID string in the format xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
 */
inline std::string generateUUID() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;
    
    uint64_t ab = dis(gen);
    uint64_t cd = dis(gen);
    
    // Set version to 4
    ab = (ab & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    // Set variant to RFC 4122
    cd = (cd & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
    
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    oss << std::setw(8) << ((ab >> 32) & 0xFFFFFFFF) << "-";
    oss << std::setw(4) << ((ab >> 16) & 0xFFFF) << "-";
    oss << std::setw(4) << (ab & 0xFFFF) << "-";
    oss << std::setw(4) << ((cd >> 48) & 0xFFFF) << "-";
    oss << std::setw(12) << (cd & 0xFFFFFFFFFFFFULL);
    
    return oss.str();
}

/**
 * @brief Check if a string is a valid UUID
 */
inline bool isValidUUID(const std::string& str) {
    static const std::regex uuid_regex(
        "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"
    );
    return std::regex_match(str, uuid_regex);
}

// ============================================================================
// String Utilities
// ============================================================================

/**
 * @brief Trim whitespace from both ends of a string
 */
inline std::string trim(const std::string& str) {
    const auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    const auto end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

/**
 * @brief Trim whitespace from the left side of a string
 */
inline std::string trimLeft(const std::string& str) {
    const auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    return str.substr(start);
}

/**
 * @brief Trim whitespace from the right side of a string
 */
inline std::string trimRight(const std::string& str) {
    const auto end = str.find_last_not_of(" \t\n\r\f\v");
    if (end == std::string::npos) return "";
    return str.substr(0, end + 1);
}

/**
 * @brief Convert string to lowercase
 */
inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

/**
 * @brief Convert string to uppercase
 */
inline std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

/**
 * @brief Split a string by delimiter
 */
inline std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

/**
 * @brief Split a string by string delimiter
 */
inline std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    
    return tokens;
}

/**
 * @brief Join a vector of strings with a delimiter
 */
inline std::string join(const std::vector<std::string>& parts, const std::string& delimiter) {
    if (parts.empty()) return "";
    
    std::ostringstream oss;
    oss << parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        oss << delimiter << parts[i];
    }
    return oss.str();
}

/**
 * @brief Replace all occurrences of a substring
 */
inline std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return str;
    
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

/**
 * @brief Check if string starts with prefix
 */
inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

/**
 * @brief Check if string ends with suffix
 */
inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

/**
 * @brief Check if string contains substring
 */
inline bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

/**
 * @brief Pad string on the left
 */
inline std::string padLeft(const std::string& str, size_t width, char padChar = ' ') {
    if (str.size() >= width) return str;
    return std::string(width - str.size(), padChar) + str;
}

/**
 * @brief Pad string on the right
 */
inline std::string padRight(const std::string& str, size_t width, char padChar = ' ') {
    if (str.size() >= width) return str;
    return str + std::string(width - str.size(), padChar);
}

/**
 * @brief Truncate string to max length with ellipsis
 */
inline std::string truncate(const std::string& str, size_t maxLen, const std::string& ellipsis = "...") {
    if (str.size() <= maxLen) return str;
    if (maxLen <= ellipsis.size()) return ellipsis.substr(0, maxLen);
    return str.substr(0, maxLen - ellipsis.size()) + ellipsis;
}

// ============================================================================
// Time Utilities
// ============================================================================

/**
 * @brief Get current timestamp in milliseconds since epoch
 */
inline int64_t currentTimeMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

/**
 * @brief Get current timestamp in seconds since epoch
 */
inline int64_t currentTimeSeconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

/**
 * @brief Get current ISO 8601 timestamp string
 */
inline std::string currentISOTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count() % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms << "Z";
    return oss.str();
}

/**
 * @brief Format timestamp to string
 */
inline std::string formatTimestamp(std::chrono::system_clock::time_point tp, 
                                   const std::string& format = "%Y-%m-%d %H:%M:%S") {
    auto time_t_val = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t_val), format.c_str());
    return oss.str();
}

/**
 * @brief Sleep for specified milliseconds
 */
inline void sleepMs(int64_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

/**
 * @brief Sleep for specified seconds
 */
inline void sleepSec(double seconds) {
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

// ============================================================================
// Logging Utilities
// ============================================================================

namespace detail {
    inline std::mutex& getLogMutex() {
        static std::mutex mutex;
        return mutex;
    }
    
    inline std::string getLogPrefix(const std::string& level) {
        return "[" + currentISOTimestamp() + "] [" + level + "] ";
    }
}

/**
 * @brief Log an info message
 */
inline void logInfo(const std::string& message) {
    std::lock_guard<std::mutex> lock(detail::getLogMutex());
    std::cout << detail::getLogPrefix("INFO") << message << std::endl;
}

/**
 * @brief Log a debug message
 */
inline void logDebug(const std::string& message) {
    std::lock_guard<std::mutex> lock(detail::getLogMutex());
    std::cout << detail::getLogPrefix("DEBUG") << message << std::endl;
}

/**
 * @brief Log a warning message
 */
inline void logWarning(const std::string& message) {
    std::lock_guard<std::mutex> lock(detail::getLogMutex());
    std::cerr << detail::getLogPrefix("WARN") << message << std::endl;
}

/**
 * @brief Log an error message
 */
inline void logError(const std::string& message) {
    std::lock_guard<std::mutex> lock(detail::getLogMutex());
    std::cerr << detail::getLogPrefix("ERROR") << message << std::endl;
}

/**
 * @brief Log a success message
 */
inline void logSuccess(const std::string& message) {
    std::lock_guard<std::mutex> lock(detail::getLogMutex());
    std::cout << detail::getLogPrefix("SUCCESS") << message << std::endl;
}

/**
 * @brief Log a trace message
 */
inline void logTrace(const std::string& message) {
    std::lock_guard<std::mutex> lock(detail::getLogMutex());
    std::cout << detail::getLogPrefix("TRACE") << message << std::endl;
}

// ============================================================================
// Number Utilities
// ============================================================================

/**
 * @brief Clamp a value between min and max
 */
template<typename T>
inline T clamp(T value, T minVal, T maxVal) {
    return std::max(minVal, std::min(value, maxVal));
}

/**
 * @brief Linear interpolation
 */
template<typename T>
inline T lerp(T a, T b, double t) {
    return static_cast<T>(a + (b - a) * t);
}

/**
 * @brief Map a value from one range to another
 */
inline double mapRange(double value, double inMin, double inMax, double outMin, double outMax) {
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

/**
 * @brief Generate a random integer in range [min, max]
 */
inline int randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

/**
 * @brief Generate a random double in range [min, max)
 */
inline double randomDouble(double min = 0.0, double max = 1.0) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

// ============================================================================
// Hash Utilities
// ============================================================================

/**
 * @brief Simple string hash (djb2)
 */
inline uint64_t hashString(const std::string& str) {
    uint64_t hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + static_cast<uint64_t>(c);
    }
    return hash;
}

/**
 * @brief Combine two hash values
 */
inline uint64_t hashCombine(uint64_t h1, uint64_t h2) {
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

// ============================================================================
// Container Utilities
// ============================================================================

/**
 * @brief Check if a container contains an element
 */
template<typename Container, typename T>
inline bool contains(const Container& container, const T& value) {
    return std::find(container.begin(), container.end(), value) != container.end();
}

/**
 * @brief Get value from map with default
 */
template<typename K, typename V>
inline V getOrDefault(const std::unordered_map<K, V>& map, const K& key, const V& defaultValue) {
    auto it = map.find(key);
    return it != map.end() ? it->second : defaultValue;
}

/**
 * @brief Get optional value from map
 */
template<typename K, typename V>
inline std::optional<V> getOptional(const std::unordered_map<K, V>& map, const K& key) {
    auto it = map.find(key);
    return it != map.end() ? std::optional<V>(it->second) : std::nullopt;
}

/**
 * @brief Remove duplicates from a vector (preserves order)
 */
template<typename T>
inline std::vector<T> unique(const std::vector<T>& vec) {
    std::vector<T> result;
    std::unordered_set<T> seen;
    for (const auto& item : vec) {
        if (seen.find(item) == seen.end()) {
            seen.insert(item);
            result.push_back(item);
        }
    }
    return result;
}

// ============================================================================
// Retry Utilities
// ============================================================================

/**
 * @brief Retry a function with exponential backoff
 */
template<typename Func>
inline bool retryWithBackoff(Func func, int maxRetries = 3, int baseDelayMs = 100) {
    for (int attempt = 0; attempt < maxRetries; ++attempt) {
        try {
            if (func()) return true;
        } catch (...) {
            // Continue to retry
        }
        
        if (attempt < maxRetries - 1) {
            int delay = baseDelayMs * (1 << attempt);
            sleepMs(delay);
        }
    }
    return false;
}

// ============================================================================
// Scope Guard
// ============================================================================

/**
 * @brief RAII scope guard for cleanup actions
 */
class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> cleanup) : cleanup_(std::move(cleanup)), active_(true) {}
    
    ~ScopeGuard() {
        if (active_) cleanup_();
    }
    
    void dismiss() { active_ = false; }
    
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
    ScopeGuard(ScopeGuard&& other) noexcept : cleanup_(std::move(other.cleanup_)), active_(other.active_) {
        other.active_ = false;
    }
    
private:
    std::function<void()> cleanup_;
    bool active_;
};

/**
 * @brief Create a scope guard
 */
inline ScopeGuard makeScopeGuard(std::function<void()> cleanup) {
    return ScopeGuard(std::move(cleanup));
}

// ============================================================================
// Environment Utilities
// ============================================================================

/**
 * @brief Get environment variable with default
 */
inline std::string getEnv(const std::string& name, const std::string& defaultValue = "") {
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : defaultValue;
}

/**
 * @brief Check if environment variable is set
 */
inline bool hasEnv(const std::string& name) {
    return std::getenv(name.c_str()) != nullptr;
}

/**
 * @brief Get environment variable as integer
 */
inline int getEnvInt(const std::string& name, int defaultValue = 0) {
    const char* value = std::getenv(name.c_str());
    if (!value) return defaultValue;
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

/**
 * @brief Get environment variable as boolean
 */
inline bool getEnvBool(const std::string& name, bool defaultValue = false) {
    const char* value = std::getenv(name.c_str());
    if (!value) return defaultValue;
    std::string str = toLower(value);
    return str == "true" || str == "1" || str == "yes" || str == "on";
}

} // namespace elizaos
