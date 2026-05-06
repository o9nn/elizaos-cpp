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

  

  struct NodeObject {
    std::optional<string | number> id;
    std::optional<double> x;
    std::optional<double> y;
    std::optional<double> vx;
    std::optional<double> vy;
    std::optional<number | null> fx;
    std::optional<number | null> fy;
};

  struct LinkObject {
    string | number | NodeObject source;
    string | number | NodeObject target;
};

  struct ForceGraphMethods {
};


} // namespace elizaos
