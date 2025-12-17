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

  class Canvas {
public:

private:
    double width_;
    double height_;
};

  class Image {
public:

private:
    std::string src_;
    double width_;
    double height_;
};

  class ImageData {
public:

private:
    Uint8ClampedArray data_;
    double width_;
    double height_;
};

  struct CanvasRenderingContext2D {
};


} // namespace elizaos
