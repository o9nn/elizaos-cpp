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

declare module 'canvas' {
  class Canvas {
    constructor(width: number, height: number);
    getContext(contextType: '2d'): CanvasRenderingContext2D;
    width: number;
    height: number;
  }

  class Image {
    src: string;
    width: number;
    height: number;
    onload: () => void;
    onerror: (err: Error) => void;
  }

  class ImageData {
    constructor(data: Uint8ClampedArray, width: number, height: number);
    data: Uint8ClampedArray;
    width: number;
    height: number;
  }

  struct CanvasRenderingContext2D {
};

}

} // namespace elizaos
