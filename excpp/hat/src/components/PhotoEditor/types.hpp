#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct Position {
    double x;
    double y;
};

struct Transform {
    Position position;
    double rotation;
    double scale;
    bool flipX;
};

struct TouchData {
    double initialDistance;
    double initialRotation;
    double initialScale;
};

} // namespace elizaos
