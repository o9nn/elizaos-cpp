#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct NotificationPreferences {
    bool enabled;
    bool sound;
    bool browserNotifications;
    bool overdue;
    bool upcoming;
    bool daily;
    double start;
    double end;
};

struct NotificationData {
    std::string title;
    std::string body;
    std::string type;
    std::optional<std::string> priority;
    std::optional<UUID> taskId;
    std::optional<UUID> roomId;
    std::string label;
    std::string action;
};

/**
 * Manager for handling notifications across different channels
 */
class NotificationManager {
public:
    NotificationManager(IAgentRuntime runtime);
    void initialize();
    void startQueueProcessor();
    void processNotificationQueue();
    void queueNotification(NotificationData notification);
    void sendNotification(NotificationData notification);
    void sendInAppNotification(NotificationData notification);
    void sendBrowserNotification(NotificationData notification);
    bool shouldSendBrowserNotification(NotificationData notification);
    bool isInQuietHours(std::optional<UUID> roomId);
    NotificationPreferences getUserPreferences(UUID userOrRoomId);
    std::future<void> updateUserPreferences(UUID userOrRoomId, const std::optional<NotificationPreferences>& preferences);
    std::future<void> loadUserPreferences();
    std::future<void> saveUserPreferences();
    void stop();

private:
    IAgentRuntime runtime_;
};


} // namespace elizaos
