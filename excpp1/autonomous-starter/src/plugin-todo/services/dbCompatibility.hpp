#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Database compatibility service to handle differences between SQLite and PostgreSQL
 */
class DatabaseCompatibilityService {
public:
    DatabaseCompatibilityService();
    void detectDatabaseType();
    std::variant<std::string, std::any> formatUuid(const std::string& uuid);
    std::variant<bool, double> formatBoolean(bool value);
    bool parseBoolean(const std::any& value);
    std::string formatDate(const std::variant<Date, std::string>& date);
    std::optional<Date> parseDate(const std::any& value);
    std::variant<std::string, std::any> formatJson(const std::any& obj);
    std::any parseJson(const std::any& value);
    std::variant<std::string, std::vector<std::string>> formatArray(const std::vector<std::string>& arr);
    std::vector<std::string> parseArray(const std::any& value);
    std::string buildCaseInsensitiveSearch(const std::string& column, const std::string& value);
    std::variant<"sqlite", "postgres", "unknown"> getDatabaseType();
};

// Export singleton instance
} // namespace elizaos
