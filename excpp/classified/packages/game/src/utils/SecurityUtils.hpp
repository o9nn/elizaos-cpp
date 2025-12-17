#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
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
 * Security utilities for the ELIZA game application
 */

class SecurityUtils {
public:
    static std::string getInstanceId();
    static std::string getSessionId();
    static std::string generateSecureId();
    static std::future<std::string> encryptData(const std::string& data, const std::string& _key);
    static std::future<std::string> decryptData(const std::string& encryptedData, const std::string& _key);
    static std::string sanitizeHtml(const std::string& html);
    static bool validateApiResponse(const std::any& response);
    static std::future<std::string> hashString(const std::string& str);
    static bool isSecureContext();
    static void storeApiKey(const std::string& provider, const std::string& key);
    static std::string getApiKey(const std::string& provider);
    static void clearApiKeys();
    static bool isRunningInTauri();
    static  getEnvironmentInfo();
};

/**
 * Input validation utilities
 */
class InputValidator {
public:
    static bool validateSafeText(const std::string& input);
    static bool validateApiKey(const std::string& key);
    static bool validateUuid(const std::string& uuid);
    static std::string sanitizeInput(const std::string& input);
    static  validateUserInput(const std::string& input);
    static  validateConfigValue(const std::string& key, const std::any& value);
    static  validateFileUpload(File file);
};

/**
 * Security logging utilities
 */
class SecurityLogger {
public:
    static void info(const std::string& message, std::optional<std::any> details);
    static void warn(const std::string& message, std::optional<std::any> details);
    static void error(const std::string& message, std::optional<std::any> details);
    static Array< getLogs(double limit = 100);
    static void clearLogs();
    static void addLog(const std::string& level, const std::string& message, std::optional<std::any> details);

private:
    double timestamp_;
    std::string level_;
    std::string message_;
};


} // namespace elizaos
