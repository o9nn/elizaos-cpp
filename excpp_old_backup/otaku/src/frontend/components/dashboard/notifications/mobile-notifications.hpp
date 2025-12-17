#include "notification-item.hpp"
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



struct MobileNotificationsProps {
    std::vector<Notification> initialNotifications;
};

struct SwipeableWrapperProps {
    React.ReactNode children;
    () => void onDelete;
};

void SwipeableWrapper(auto { children, SwipeableWrapperProps onDelete });


} // namespace elizaos
