#include "notifications.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void notify(std::optional<std::any> newNotification) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        notifications,
        set: setNotificationStore,
        } = useNotificationStore.getState();

        setNotificationStore((state: { notifications: any[] }) => {
            state.notifications = [;
            ...notifications,
            { type: 'success', ...newNotification },
            ];
            });

}

} // namespace elizaos
