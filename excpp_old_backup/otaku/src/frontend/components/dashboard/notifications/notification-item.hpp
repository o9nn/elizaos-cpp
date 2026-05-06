#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct NotificationItemProps {
    Notification notification;
    (id: string) => void onMarkAsRead;
    (id: string) => void onDelete;
};


} // namespace elizaos
