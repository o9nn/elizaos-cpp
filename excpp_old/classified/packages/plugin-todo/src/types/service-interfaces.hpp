#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Interface definitions for service methods used in tests
 */

struct TodoReminderService {
};

struct TodoNotificationService {
};

struct TodoDailyResetService {
};

struct ContentWithText {
    std::optional<std::string> text;
};

struct ServiceClass {
    std::optional<std::string> serviceType;
    std::optional<std::string> serviceName;
};

struct MockFunction {
    std::vector<std::vector<unknown>> calls;
};


} // namespace elizaos
