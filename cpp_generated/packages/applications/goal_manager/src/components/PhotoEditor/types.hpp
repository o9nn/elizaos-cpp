#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_HAT_SRC_COMPONENTS_PHOTOEDITOR_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_HAT_SRC_COMPONENTS_PHOTOEDITOR_TYPES_H
#include "core.h"

class Position;
class Transform;
class TouchData;

class Position : public object, public std::enable_shared_from_this<Position> {
public:
    using std::enable_shared_from_this<Position>::shared_from_this;
    double x;

    double y;
};

class Transform : public object, public std::enable_shared_from_this<Transform> {
public:
    using std::enable_shared_from_this<Transform>::shared_from_this;
    std::shared_ptr<Position> position;

    double rotation;

    double scale;

    boolean flipX;
};

class TouchData : public object, public std::enable_shared_from_this<TouchData> {
public:
    using std::enable_shared_from_this<TouchData>::shared_from_this;
    double initialDistance;

    double initialRotation;

    double initialScale;
};

#endif
