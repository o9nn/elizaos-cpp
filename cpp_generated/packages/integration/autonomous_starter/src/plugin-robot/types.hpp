#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ROBOT_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ROBOT_TYPES_H
#include "core.h"

namespace @elizaos/core {
    class ServiceTypeRegistry;
}
class ScreenObject;
class ScreenActionStep;
class ScreenDescription;
class ScreenContext;
class ChangeDetectionConfig;
class RobotServiceConfig;

namespace std::string("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string ROBOT;
    };

}
extern object RobotServiceType, satisfies, Partial;
class ScreenObject : public object, public std::enable_shared_from_this<ScreenObject> {
public:
    using std::enable_shared_from_this<ScreenObject>::shared_from_this;
    string label;

    object bbox;
};

class ScreenActionStep : public object, public std::enable_shared_from_this<ScreenActionStep> {
public:
    using std::enable_shared_from_this<ScreenActionStep>::shared_from_this;
    any action;

    double x;

    double y;

    string text;

    any button;
};

class ScreenDescription : public object, public std::enable_shared_from_this<ScreenDescription> {
public:
    using std::enable_shared_from_this<ScreenDescription>::shared_from_this;
    string description;

    double timestamp;

    string relativeTime;
};

class ScreenContext : public object, public std::enable_shared_from_this<ScreenContext> {
public:
    using std::enable_shared_from_this<ScreenContext>::shared_from_this;
    std::shared_ptr<Buffer> screenshot;

    string currentDescription;

    array<std::shared_ptr<ScreenDescription>> descriptionHistory;

    string ocr;

    array<std::shared_ptr<ScreenObject>> objects;

    double timestamp;

    boolean changeDetected;

    double pixelDifferencePercentage;
};

class ChangeDetectionConfig : public object, public std::enable_shared_from_this<ChangeDetectionConfig> {
public:
    using std::enable_shared_from_this<ChangeDetectionConfig>::shared_from_this;
    double threshold;

    boolean enabled;
};

class RobotServiceConfig : public object, public std::enable_shared_from_this<RobotServiceConfig> {
public:
    using std::enable_shared_from_this<RobotServiceConfig>::shared_from_this;
    double cacheTTL;

    std::shared_ptr<ChangeDetectionConfig> changeDetection;

    double maxHistoryEntries;
};

namespace @elizaos/core {
}
#endif
